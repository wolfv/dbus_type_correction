// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_PREDICTION_DICTIONARY_SERVICE_H_
#define SERVICES_PREDICTION_DICTIONARY_SERVICE_H_

#include "proximity_info_factory.h"
#include "context_info.h"
#include <memory>

namespace latinime {


class Dictionary;
class DicTraverseSession;
class PrevWordsInfo;
}  // namespace latinime

namespace prediction {

class DictionaryService {
 public:
  DictionaryService();
  ~DictionaryService();

  std::vector<std::string> GetDictionarySuggestion(
      const latinime::Dictionary* dict,
      latinime::DicTraverseSession* sess,
      PredictionInfoPtr prediction_info,
      latinime::ProximityInfo* proximity_info);

 private:

  latinime::Dictionary* const OpenDictionary(const std::string path,
                                             const int start_offset,
                                             const int size,
                                             const bool is_updatable);

  latinime::PrevWordsInfo ProcessPrevWord(
      std::vector<PrevWordInfoPtr>& prev_words);

  int max_suggestion_size_;
  std::unique_ptr<latinime::Dictionary> default_dictionary_;
  std::unique_ptr<latinime::DicTraverseSession> default_session_;

  DISALLOW_COPY_AND_ASSIGN(DictionaryService);
};

}  // namespace prediction

#endif  // SERVICES_PREDICTION_DICTIONARY_SERVICE_H_
