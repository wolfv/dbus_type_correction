#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <tuple>

#include "third_party/android_prediction/suggest/core/dictionary/dictionary.h"
#include "third_party/android_prediction/suggest/core/result/suggestion_results.h"
#include "third_party/android_prediction/suggest/core/session/dic_traverse_session.h"
#include "third_party/android_prediction/suggest/core/session/prev_words_info.h"
#include "third_party/android_prediction/suggest/core/suggest_options.h"
#include "third_party/android_prediction/suggest/policyimpl/dictionary/structure/dictionary_structure_with_buffer_policy_factory.h"

#include "dictionary_service.h"

namespace py = pybind11;


PYBIND11_MODULE(suggestr, m) {
    m.doc() = "Python module to the suggestr"; // optional module docstring

    static prediction::DictionaryService* dictionary_service = new prediction::DictionaryService();

    py::class_<latinime::Dictionary>(m, "Dictionary");
    py::class_<latinime::DicTraverseSession>(m, "DicTraverseSession");

    m.def("load_dictionary", [](const std::string& path, int offset, int size, bool is_updatable)
    {
        using structure_policy_ptr = latinime::DictionaryStructureWithBufferPolicy::StructurePolicyPtr;
        structure_policy_ptr dict_ptr = latinime::DictionaryStructureWithBufferPolicyFactory::
                    newPolicyForExistingDictFile(path.c_str(), offset, size, is_updatable);

        if (!dict_ptr) {
            return std::make_tuple(reinterpret_cast<latinime::Dictionary*>(NULL), 
                                   reinterpret_cast<latinime::DicTraverseSession*>(NULL));
        }

        latinime::Dictionary* dictionary = new latinime::Dictionary(
            std::move(dict_ptr));

        latinime::DicTraverseSession* session = reinterpret_cast<latinime::DicTraverseSession*>(
                latinime::DicTraverseSession::getSessionInstance("en", size - offset));
        latinime::PrevWordsInfo empty_prev_words;
        session->init(dictionary, &empty_prev_words, 0);

        return std::make_tuple(dictionary, session);
    });

    m.def("get_suggestion", [](const std::string& stump, const std::vector<std::string>& context,
        const latinime::Dictionary* dict,  latinime::DicTraverseSession* sess) {

        prediction::PredictionInfo * prediction_info = new prediction::PredictionInfo();

        for (auto context_word : context) {
            prediction_info->previous_words.push_back(
                new prediction::PrevWordInfo(context_word, /*is_beginning_of_sentence?*/false)
            );
        }

        prediction_info->current_word = stump;
        prediction::ProximityInfoFactory fac = prediction::ProximityInfoFactory();
        latinime::ProximityInfo* proximity_info = fac.GetNativeProximityInfo();
        
        auto suggestions = dictionary_service->GetDictionarySuggestion(dict, sess, prediction_info, proximity_info);

        return suggestions;
        // std::string result = "";
        // for (auto s : suggestions) {
        //  std::cout << s << std::endl;
        //  result += s + " ";
        // }
        // std::cout << "Got suggestions" << std::endl;
        // // return "Hello " + result + "!";
        // return suggestions;
    });
}
