// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// To test, run "out/Debug//mojo_shell mojo:prediction_apptests"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "mojo/public/cpp/application/application_impl.h"
#include "mojo/public/cpp/application/application_test_base.h"
#include "mojo/services/prediction/interfaces/prediction.mojom.h"

namespace prediction {

void GetPredictionListAndEnd(std::vector<std::string>* output_list,
                             const mojo::Array<mojo::String>& input_list) {
  *output_list = input_list.To<std::vector<std::string>>();
  base::MessageLoop::current()->Quit();
}

class PredictionApptest : public mojo::test::ApplicationTestBase {
 public:
  PredictionApptest() : ApplicationTestBase() {}

  ~PredictionApptest() override {}

  void SetUp() override {
    mojo::test::ApplicationTestBase::SetUp();
    application_impl()->ConnectToService("mojo:prediction_service",
                                         &prediction_);
  }

  std::vector<std::string> GetPredictionListClient(
      mojo::Array<PrevWordInfoPtr>& prev_words,
      const mojo::String& cur_word) {
    PredictionInfoPtr prediction_info = PredictionInfo::New();
    prediction_info->previous_words = prev_words.Pass();
    prediction_info->current_word = cur_word;

    std::vector<std::string> prediction_list;
    prediction_->GetPredictionList(
        prediction_info.Pass(),
        base::Bind(&GetPredictionListAndEnd, &prediction_list));
    base::MessageLoop::current()->Run();
    return prediction_list;
  }

 private:
  PredictionServicePtr prediction_;

  DISALLOW_COPY_AND_ASSIGN(PredictionApptest);
};

TEST_F(PredictionApptest, CurrentSpellcheck) {
  mojo::Array<PrevWordInfoPtr> prev_words =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction = GetPredictionListClient(prev_words, "tgis")[0];
  EXPECT_EQ(prediction, "this");

  mojo::Array<PrevWordInfoPtr> prev_words1 =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction1 = GetPredictionListClient(prev_words1, "aplle")[0];
  EXPECT_EQ(prediction1, "Apple");
}

TEST_F(PredictionApptest, CurrentSuggest) {
  mojo::Array<PrevWordInfoPtr> prev_words =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction = GetPredictionListClient(prev_words, "peac")[0];
  EXPECT_EQ(prediction, "peace");

  mojo::Array<PrevWordInfoPtr> prev_words1 =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction1 = GetPredictionListClient(prev_words1, "fil")[0];
  EXPECT_EQ(prediction1, "film");

  mojo::Array<PrevWordInfoPtr> prev_words2 =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction2 = GetPredictionListClient(prev_words2, "entert")[0];
  EXPECT_EQ(prediction2, "entertainment");
}

TEST_F(PredictionApptest, CurrentSuggestCont) {
  mojo::Array<PrevWordInfoPtr> prev_words =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction = GetPredictionListClient(prev_words, "a")[0];
  EXPECT_EQ(prediction, "and");

  mojo::Array<PrevWordInfoPtr> prev_words1 =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction1 = GetPredictionListClient(prev_words1, "ab")[0];
  EXPECT_EQ(prediction1, "an");
}

TEST_F(PredictionApptest, CurrentSuggestUp) {
  mojo::Array<PrevWordInfoPtr> prev_words =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction = GetPredictionListClient(prev_words, "Beau")[0];
  EXPECT_EQ(prediction, "Beat");

  mojo::Array<PrevWordInfoPtr> prev_words1 =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction1 = GetPredictionListClient(prev_words1, "THis")[0];
  EXPECT_EQ(prediction1, "This");
}

TEST_F(PredictionApptest, CurrentNoSuggest) {
  mojo::Array<PrevWordInfoPtr> prev_words =
      mojo::Array<PrevWordInfoPtr>::New(0);
  std::string prediction = GetPredictionListClient(
      prev_words,
      "hjlahflgfagfdafaffgruhgadfhjklghadflkghjalkdfjkldfhrshrtshtsrhkra")[0];
  EXPECT_EQ(prediction, "homage offered a faded rugged should had dough");
}

TEST_F(PredictionApptest, EmptyCurrent) {
  PrevWordInfoPtr prev_word = PrevWordInfo::New();
  prev_word->word = "This";
  prev_word->is_beginning_of_sentence = true;
  mojo::Array<PrevWordInfoPtr> prev_words =
      mojo::Array<PrevWordInfoPtr>::New(0);
  prev_words.push_back(prev_word.Pass());
  EXPECT_EQ((int)GetPredictionListClient(prev_words, "").size(), 0);
}

}  // namespace prediction
