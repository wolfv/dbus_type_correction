#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tuple>

#define HOST_TOOL
#define FLAG_DO_PROFILE
#define FLAG_DBG

#include "third_party/android_prediction/suggest/core/dictionary/dictionary.h"
// #include "third_party/android_prediction/suggest/core/dictionary/property/unigram_property.h"
#include "third_party/android_prediction/suggest/core/result/suggestion_results.h"
#include "third_party/android_prediction/suggest/core/session/dic_traverse_session.h"
#include "third_party/android_prediction/suggest/core/session/prev_words_info.h"
#include "third_party/android_prediction/suggest/core/suggest_options.h"
#include "third_party/android_prediction/suggest/policyimpl/dictionary/structure/dictionary_structure_with_buffer_policy_factory.h"

#include "dictionary_service.h"

namespace py = pybind11;

#include "third_party/utfcpp/utf8.h"

namespace utils
{
    std::vector<int> s2v(const std::string& utf8_string)
    {
        std::vector<int> res;
        utf8::utf8to32(utf8_string.begin(), utf8_string.end(), std::back_inserter(res));
        return res;
    }

    std::string v2s(const std::vector<int>& vec)
    {
        std::string res;
        utf8::utf32to8(vec.begin(), vec.end(), std::back_inserter(res));
        return res;
    }
}


auto get_session(latinime::DictionaryStructureWithBufferPolicy::StructurePolicyPtr&& dict_ptr) {
    if (!dict_ptr) {
        return std::make_tuple(reinterpret_cast<latinime::Dictionary*>(NULL), 
                               reinterpret_cast<latinime::DicTraverseSession*>(NULL));
    }

    latinime::Dictionary* dictionary = new latinime::Dictionary(std::move(dict_ptr));

    latinime::DicTraverseSession* session = reinterpret_cast<latinime::DicTraverseSession*>(
            latinime::DicTraverseSession::getSessionInstance("en", 0));
    latinime::PrevWordsInfo empty_prev_words;
    session->init(dictionary, &empty_prev_words, 0);

    return std::make_tuple(std::move(dictionary), std::move(session));
}

auto makeBigramProperty(const std::string& str, const int probability, 
                        const int timestamp, const int level, const int count)
{
    const std::vector<int> cpv = utils::s2v(str);
    return latinime::BigramProperty(&cpv, probability, timestamp, level, count);
}

auto makeWordProperty(const std::string& str, const latinime::UnigramProperty& unigramProp,
                      const std::vector<latinime::BigramProperty>& bigrams)
{
    auto vec = utils::s2v(str);
    return latinime::WordProperty(&vec, &unigramProp, &bigrams);
}

auto makeShortcutProperty(const std::string& str, const int probability)
{
    const std::vector<int> cpv = utils::s2v(str);
    return latinime::UnigramProperty::ShortcutProperty(&cpv, probability);
}

auto toPrevWordsInfo(const std::vector<prediction::PrevWordInfo>& pwi)
{
    std::vector<int[MAX_WORD_LENGTH]> codePoints(pwi.size());
    std::vector<int> counts(pwi.size());
    std::vector<char> isBeginningOfSentence(pwi.size());
    for (std::size_t i = 0; i < pwi.size(); ++i) {
        auto v = utils::s2v(pwi[i].word);
        std::copy(v.begin(), v.end(), std::begin(codePoints[i]));
        counts[i] = v.size();
        isBeginningOfSentence[i] = pwi[i].is_beginning_of_sentence;
    }
    return latinime::PrevWordsInfo(codePoints.data(), counts.data(), (const bool*)isBeginningOfSentence.data(), pwi.size());
}

PYBIND11_MODULE(suggestr, m) {
    m.doc() = "Python module to the suggestr"; // optional module docstring

    static prediction::DictionaryService* dictionary_service = new prediction::DictionaryService();
    
    py::class_<prediction::PrevWordInfo>(m, "PrevWordInfo")
        .def(py::init<std::string, bool>())
    ;

    py::class_<latinime::DicTraverseSession>(m, "DicTraverseSession");

    py::class_<latinime::Dictionary>(m, "Dictionary")
        // .def("add_unigram", &latinime::Dictionary::addUnigramEntry)
        .def("add_unigram", [](latinime::Dictionary& self, 
                               const std::string& str,
                               const latinime::UnigramProperty& prop) {
            auto v = utils::s2v(str);
            return self.addUnigramEntry(v.data(), v.size(), &prop);
        })
        .def("remove_unigram", [](latinime::Dictionary& self, 
                               const std::string& str) {
            auto v = utils::s2v(str);
            self.removeUnigramEntry(v.data(), v.size());
        })
        .def("add_ngram", [](latinime::Dictionary& self,
                             const std::vector<prediction::PrevWordInfo>& pwi,
                             latinime::BigramProperty bp) {
            auto pxi = toPrevWordsInfo(pwi);
            return self.addNgramEntry(&pxi, &bp);
        })
        .def("get_ngram_probability", [](const latinime::Dictionary& self,
                                         std::string& word,
                                         const std::vector<prediction::PrevWordInfo>& pwi) {
            auto v = utils::s2v(word);
            auto pxi = toPrevWordsInfo(pwi);
            return self.getNgramProbability(&pxi, v.data(), v.size());
        })
        .def("get_probability", [](const latinime::Dictionary& self,
                                  std::string& word) {
            auto v = utils::s2v(word);
            return self.getProbability(v.data(), v.size());
        })
        .def("save", [](latinime::Dictionary& self, const std::string& path) {
            self.flush(path.c_str());
        })
        .def("flush_with_gc", [](latinime::Dictionary& self, const std::string& path) {
            self.flushWithGC(path.c_str());
        })
        .def("get_word_prop", [](latinime::Dictionary& self, const std::string& word) {
            auto vec = utils::s2v(word);
            return self.getWordProperty(vec.data(), vec.size());
        }, py::return_value_policy::copy)
        .def("get_next_word_and_token", [](latinime::Dictionary& self, int token) {
            std::vector<int> vec(MAX_WORD_LENGTH);
            int word_length = 0;
            int next_token = self.getNextWordAndNextToken(token, vec.data(), &word_length);
            vec.resize(word_length);
            std::string res = utils::v2s(vec);
            return std::make_tuple(next_token, res);
        })
    ;

    py::class_<latinime::UnigramProperty::ShortcutProperty>(m, "ShortcutProperty")
        .def(py::init(&makeShortcutProperty))
    ;

    py::class_<latinime::UnigramProperty>(m, "UnigramProperty")
        .def(py::init<>())
        // .def(py::init<>(const bool representsBeginningOfSentence, const bool isNotAWord,
        //     const bool isBlacklisted, const int probability, const int timestamp, const int level,
        //     const int count, const std::vector<ShortcutProperty> *const shortcuts))
        .def(py::init<bool, bool, bool, int, int, int, int, const std::vector<latinime::UnigramProperty::ShortcutProperty>*>(),
            py::arg("representsBeginningOfSentence")=false,
            py::arg("isNotAWord")=false,
            py::arg("isBlacklisted")=false,
            py::arg("probability")=0,
            py::arg("timestamp")=0,
            py::arg("level")=0,
            py::arg("count")=0,
            py::arg("shortcuts")=nullptr
        )
        .def("get_probability", [](const latinime::UnigramProperty& self) {
            return self.getProbability();
        })
        .def("is_blacklisted", [](const latinime::UnigramProperty& self) {
            return self.isBlacklisted();
        })
        .def("is_not_a_word", [](const latinime::UnigramProperty& self) {
            return self.isNotAWord();
        })
        .def("has_shortcuts", [](const latinime::UnigramProperty& self) {
            return self.hasShortcuts();
        })
        .def("get_probability", [](const latinime::UnigramProperty& self) {
            return self.getProbability();
        })
        .def("get_timestamp", [](const latinime::UnigramProperty& self) {
            return self.getTimestamp();
        })
        .def("get_level", [](const latinime::UnigramProperty& self) {
            return self.getLevel();
        })
        .def("get_count", [](const latinime::UnigramProperty& self) {
            return self.getCount();
        })
    ;

    py::class_<latinime::WordProperty>(m, "WordProperty")
        .def(py::init<>())
        .def(py::init(&makeWordProperty))
        .def("get_bigram_props", [](const latinime::WordProperty& self) { return self.getBigramProperties(); }, py::return_value_policy::copy)
        .def("get_unigram_prop", [](const latinime::WordProperty& self) { return self.getUnigramProperty(); }, py::return_value_policy::copy)
    ;

    py::class_<latinime::BigramProperty>(m, "BigramProperty")
        .def(py::init(&makeBigramProperty))
        .def("get_target", [](const latinime::BigramProperty& self) { return utils::v2s(*self.getTargetCodePoints()); })
        .def("get_prob", [](const latinime::BigramProperty& self) { return self.getProbability(); })
        .def("get_timestamp", [](const latinime::BigramProperty& self) { return self.getTimestamp(); })
        .def("get_level", [](const latinime::BigramProperty& self) { return self.getLevel(); })
        .def("get_count", [](const latinime::BigramProperty& self) { return self.getCount(); })
    ;

    m.def("make_dictionary", []() {
        using structure_policy_ptr = latinime::DictionaryStructureWithBufferPolicy::StructurePolicyPtr;
        // newPolicyForOnMemoryDict(const int formatVersion, const std::vector<int> &locale,
        // const DictionaryHeaderStructurePolicy::AttributeMap *const attributeMap);

        latinime::DictionaryHeaderStructurePolicy::AttributeMap attributes;

        structure_policy_ptr dict_ptr = latinime::DictionaryStructureWithBufferPolicyFactory::newPolicyForOnMemoryDict(
            latinime::FormatUtils::VERSION_4,
            utils::s2v("en_US"),
            &attributes
        );
        return get_session(std::move(dict_ptr));
    });

    m.def("load_dictionary", [](const std::string& path, int offset, int size, bool is_updatable)
    {
        using structure_policy_ptr = latinime::DictionaryStructureWithBufferPolicy::StructurePolicyPtr;
        structure_policy_ptr dict_ptr = latinime::DictionaryStructureWithBufferPolicyFactory::
                    newPolicyForExistingDictFile(path.c_str(), offset, size, is_updatable);
        if (dict_ptr == nullptr) {
            throw std::runtime_error("No dictionary found.");
        }
        return get_session(std::move(dict_ptr));
    });

    m.def("get_suggestion", [](const std::string& stump, 
        const std::vector<prediction::PrevWordInfo*>& context,
        const latinime::Dictionary* dict, latinime::DicTraverseSession* sess) {

        prediction::PredictionInfo * prediction_info = new prediction::PredictionInfo {
            stump, context
        };

        prediction::ProximityInfoFactory fac = prediction::ProximityInfoFactory();

        latinime::ProximityInfo* proximity_info = fac.GetNativeProximityInfo();

        auto suggestions = dictionary_service->GetDictionarySuggestion(
            dict, sess, prediction_info, proximity_info);

        return suggestions;
    });
}
