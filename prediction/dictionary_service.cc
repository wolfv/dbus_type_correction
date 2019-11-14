// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <deque>
#include <fstream>
#include <string>

// #include "base/base_paths.h"
// #include "base/files/file_path.h"
// #include "base/path_service.h"
// #include "base/strings/string16.h"
// #include "base/strings/utf_string_conversions.h"

#include "dictionary_service.h"
#include "input_info.h"

// Otherwise embedded
// #include "services/prediction/kDictFile.h"

#include "context_info.h"
#include "key_set.h"
#include "proximity_info_factory.h"
#include "third_party/android_prediction/suggest/core/dictionary/dictionary.h"
#include "third_party/android_prediction/suggest/core/result/suggestion_results.h"
#include "third_party/android_prediction/suggest/core/session/dic_traverse_session.h"
#include "third_party/android_prediction/suggest/core/session/prev_words_info.h"
#include "third_party/android_prediction/suggest/core/suggest_options.h"
#include "third_party/android_prediction/suggest/policyimpl/dictionary/structure/dictionary_structure_with_buffer_policy_factory.h"

#include <codecvt>
#include <iostream>
#include <locale>
#include <memory>
#include <sys/stat.h>

std::wstring s2ws(const std::string &str) {
  typedef std::codecvt_utf8<wchar_t> convert_typeX;
  std::wstring_convert<convert_typeX, wchar_t> converterX;

  return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring &wstr) {
  typedef std::codecvt_utf8<wchar_t> convert_typeX;
  std::wstring_convert<convert_typeX, wchar_t> converterX;

  return converterX.to_bytes(wstr);
}

namespace prediction {

DictionaryService::DictionaryService() : max_suggestion_size_(50) {}

DictionaryService::~DictionaryService() {}

// void DictionaryService::CreatDictFromEmbeddedDataIfNotExist(
//     const std::string path) {
//   if (std::ifstream(path.c_str()))
//     return;
//   std::ofstream dic_file(path.c_str(),
//                          std::ofstream::out | std::ofstream::binary);
//   dic_file.write(prediction::kDictFile.data, prediction::kDictFile.size);
//   dic_file.close();
// }

latinime::Dictionary *const
DictionaryService::OpenDictionary(const std::string path,
                                  const int start_offset, const int size,
                                  const bool is_updatable) {
  latinime::DictionaryStructureWithBufferPolicy::StructurePolicyPtr
      dictionary_structure_with_buffer_policy(
          latinime::DictionaryStructureWithBufferPolicyFactory::
              newPolicyForExistingDictFile(path.c_str(), start_offset, size,
                                           is_updatable));
  if (!dictionary_structure_with_buffer_policy) {
    return nullptr;
  }

  latinime::Dictionary *const dictionary = new latinime::Dictionary(
      std::move(dictionary_structure_with_buffer_policy));
  return dictionary;
}

std::unique_ptr<latinime::Dictionary> OpenDictionarySimple(const std::string& path)
{

}

size_t getFilesize(const char *filename) {
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}

std::vector<std::string> DictionaryService::GetDictionarySuggestion(
    const latinime::Dictionary* dict,
    latinime::DicTraverseSession* sess,
    PredictionInfoPtr prediction_info,
    latinime::ProximityInfo *proximity_info) {
  std::vector<std::string> suggestion_words = std::vector<std::string>();

  // current word
  int input_size = std::min(
      static_cast<int>(prediction_info->current_word.size()), MAX_WORD_LENGTH);
  InputInfo input_info(prediction_info->current_word, input_size);
  input_size = input_info.GetRealSize();

  // previous words
  latinime::PrevWordsInfo prev_words_info =
      ProcessPrevWord(prediction_info->previous_words);

  // suggestion options
  // is_gesture, use_full_edit_distance,
  // block_offensive_words, space_aware gesture_enabled
  int options[] = {0, 0, 0, 0};

  latinime::SuggestOptions suggest_options(options, arraysize(options));
  latinime::SuggestionResults suggestion_results(max_suggestion_size_);

  if (input_size > 0)
  {
    // std::cout << "getting suggestions" << std::endl;
    dict->getSuggestions(
        proximity_info, sess, input_info.GetXCoordinates(),
        input_info.GetYCoordinates(), input_info.GetTimes(),
        input_info.GetPointerIds(), input_info.GetCodepoints(), input_size,
        &prev_words_info, &suggest_options, 1.0f, &suggestion_results);
  }
  else
  {
    dict->getPredictions(&prev_words_info, &suggestion_results);
  }

  suggestion_results.dumpSuggestions();
  // process suggestion results
  std::deque<std::string> suggestion_words_reverse;
  char cur_beginning;
  std::string lo_cur;
  std::string up_cur;
  if (input_size > 0) {
    cur_beginning = prediction_info->current_word[0];
    std::string cur_rest =
        std::string(prediction_info->current_word.data())
            .substr(1, prediction_info->current_word.size() - 1);
    lo_cur = std::string(1, (char)tolower(cur_beginning)) + cur_rest;
    up_cur = std::string(1, (char)toupper(cur_beginning)) + cur_rest;
  }
  while (!suggestion_results.mSuggestedWords.empty()) {
    const latinime::SuggestedWord &suggested_word =
        suggestion_results.mSuggestedWords.top();

    std::wstring word;
    for (int i = 0; i < suggested_word.getCodePointCount(); i++) {
      //   base::char16 code_point = suggested_word.getCodePoint()[i];
      word.push_back(suggested_word.getCodePoint()[i]);
    }

    std::string word_string = ws2s(word);
    std::cout << "Score: " << suggested_word.getScore() << " - " << word_string << std::endl;

    suggestion_words_reverse.push_front(word_string);
    suggestion_results.mSuggestedWords.pop();
  }

  // remove dups within suggestion words
  for (size_t i = 0; i < suggestion_words_reverse.size(); i++) {
    for (size_t j = i + 1; j < suggestion_words_reverse.size(); j++) {
      if (suggestion_words_reverse[i].compare(suggestion_words_reverse[j]) ==
          0) {
        suggestion_words_reverse.erase(suggestion_words_reverse.begin() + j);
        j--;
      }
    }
  }

  for (std::deque<std::string>::iterator it = suggestion_words_reverse.begin();
       it != suggestion_words_reverse.end(); ++it) {
    suggestion_words.push_back(std::string(*it));
  }

  return suggestion_words;
}

// modified from Android JniDataUtils::constructPrevWordsInfo
latinime::PrevWordsInfo
DictionaryService::ProcessPrevWord(std::vector<PrevWordInfoPtr> &prev_words) {
  int prev_word_codepoints[MAX_PREV_WORD_COUNT_FOR_N_GRAM][MAX_WORD_LENGTH] = {
      {0}};
  int prev_word_codepoint_count[MAX_PREV_WORD_COUNT_FOR_N_GRAM] = {0};
  bool are_beginning_of_sentence[MAX_PREV_WORD_COUNT_FOR_N_GRAM] = {false};
  int prevwords_count = std::min(
      prev_words.size(), static_cast<size_t>(MAX_PREV_WORD_COUNT_FOR_N_GRAM));

  for (int i = 0; i < prevwords_count; ++i) {
    prev_word_codepoint_count[i] = 0;
    are_beginning_of_sentence[i] = false;
    int prev_word_size = prev_words[i]->word.size();
    if (prev_word_size > MAX_WORD_LENGTH) {
      continue;
    }
    for (int j = 0; j < prev_word_size; j++) {
      prev_word_codepoints[i][j] = (int)((prev_words[i])->word)[j];
    }
    prev_word_codepoint_count[i] = prev_word_size;
    are_beginning_of_sentence[i] = prev_words[i]->is_beginning_of_sentence;
  }
  latinime::PrevWordsInfo prev_words_info =
      latinime::PrevWordsInfo(prev_word_codepoints, prev_word_codepoint_count,
                              are_beginning_of_sentence, prevwords_count);
  return prev_words_info;
}

} // namespace prediction