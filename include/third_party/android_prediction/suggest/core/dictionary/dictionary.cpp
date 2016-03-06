/*
 * Copyright (C) 2009, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LatinIME: dictionary.cpp"

#include "third_party/android_prediction/suggest/core/dictionary/dictionary.h"

#include "third_party/android_prediction/defines.h"
#include "third_party/android_prediction/suggest/core/dictionary/dictionary_utils.h"
#include "third_party/android_prediction/suggest/core/policy/dictionary_header_structure_policy.h"
#include "third_party/android_prediction/suggest/core/result/suggestion_results.h"
#include "third_party/android_prediction/suggest/core/session/dic_traverse_session.h"
#include "third_party/android_prediction/suggest/core/session/prev_words_info.h"
#include "third_party/android_prediction/suggest/core/suggest.h"
#include "third_party/android_prediction/suggest/core/suggest_options.h"
#include "third_party/android_prediction/suggest/policyimpl/gesture/gesture_suggest_policy_factory.h"
#include "third_party/android_prediction/suggest/policyimpl/typing/typing_suggest_policy_factory.h"
#include "third_party/android_prediction/utils/time_keeper.h"

namespace latinime {

const int Dictionary::HEADER_ATTRIBUTE_BUFFER_SIZE = 32;

Dictionary::Dictionary(DictionaryStructureWithBufferPolicy::StructurePolicyPtr
        dictionaryStructureWithBufferPolicy)
        : mDictionaryStructureWithBufferPolicy(std::move(dictionaryStructureWithBufferPolicy)),
          mGestureSuggest(new Suggest(GestureSuggestPolicyFactory::getGestureSuggestPolicy())),
          mTypingSuggest(new Suggest(TypingSuggestPolicyFactory::getTypingSuggestPolicy())) {
}

void Dictionary::getSuggestions(ProximityInfo *proximityInfo, DicTraverseSession *traverseSession,
        int *xcoordinates, int *ycoordinates, int *times, int *pointerIds, int *inputCodePoints,
        int inputSize, const PrevWordsInfo *const prevWordsInfo,
        const SuggestOptions *const suggestOptions, const float languageWeight,
        SuggestionResults *const outSuggestionResults) const {
    TimeKeeper::setCurrentTime();
    traverseSession->init(this, prevWordsInfo, suggestOptions);
    const auto &suggest = suggestOptions->isGesture() ? mGestureSuggest : mTypingSuggest;
    suggest->getSuggestions(proximityInfo, traverseSession, xcoordinates,
            ycoordinates, times, pointerIds, inputCodePoints, inputSize,
            languageWeight, outSuggestionResults);
    if (DEBUG_DICT) {
        outSuggestionResults->dumpSuggestions();
    }
}

Dictionary::NgramListenerForPrediction::NgramListenerForPrediction(
        const PrevWordsInfo *const prevWordsInfo, SuggestionResults *const suggestionResults,
        const DictionaryStructureWithBufferPolicy *const dictStructurePolicy)
    : mPrevWordsInfo(prevWordsInfo), mSuggestionResults(suggestionResults),
      mDictStructurePolicy(dictStructurePolicy) {}

void Dictionary::NgramListenerForPrediction::onVisitEntry(const int ngramProbability,
        const int targetPtNodePos) {
    if (targetPtNodePos == NOT_A_DICT_POS) {
        return;
    }
    if (mPrevWordsInfo->isNthPrevWordBeginningOfSentence(1 /* n */)
            && ngramProbability == NOT_A_PROBABILITY) {
        return;
    }
    int targetWordCodePoints[MAX_WORD_LENGTH];
    int unigramProbability = 0;
    const int codePointCount = mDictStructurePolicy->
            getCodePointsAndProbabilityAndReturnCodePointCount(targetPtNodePos,
                    MAX_WORD_LENGTH, targetWordCodePoints, &unigramProbability);
    if (codePointCount <= 0) {
        return;
    }
    const int probability = mDictStructurePolicy->getProbability(
            unigramProbability, ngramProbability);
    mSuggestionResults->addPrediction(targetWordCodePoints, codePointCount, probability);
}

void Dictionary::getPredictions(const PrevWordsInfo *const prevWordsInfo,
        SuggestionResults *const outSuggestionResults) const {
    TimeKeeper::setCurrentTime();
    NgramListenerForPrediction listener(prevWordsInfo, outSuggestionResults,
            mDictionaryStructureWithBufferPolicy.get());
    int prevWordsPtNodePos[MAX_PREV_WORD_COUNT_FOR_N_GRAM];
    prevWordsInfo->getPrevWordsTerminalPtNodePos(
            mDictionaryStructureWithBufferPolicy.get(), prevWordsPtNodePos,
            true /* tryLowerCaseSearch */);
    mDictionaryStructureWithBufferPolicy->iterateNgramEntries(prevWordsPtNodePos, &listener);
}

int Dictionary::getProbability(const int *word, int length) const {
    return getNgramProbability(nullptr /* prevWordsInfo */, word, length);
}

int Dictionary::getMaxProbabilityOfExactMatches(const int *word, int length) const {
    TimeKeeper::setCurrentTime();
    return DictionaryUtils::getMaxProbabilityOfExactMatches(
            mDictionaryStructureWithBufferPolicy.get(), word, length);
}

int Dictionary::getNgramProbability(const PrevWordsInfo *const prevWordsInfo, const int *word,
        int length) const {
    TimeKeeper::setCurrentTime();
    int nextWordPos = mDictionaryStructureWithBufferPolicy->getTerminalPtNodePositionOfWord(word,
            length, false /* forceLowerCaseSearch */);
    if (NOT_A_DICT_POS == nextWordPos) return NOT_A_PROBABILITY;
    if (!prevWordsInfo) {
        return getDictionaryStructurePolicy()->getProbabilityOfPtNode(
                nullptr /* prevWordsPtNodePos */, nextWordPos);
    }
    int prevWordsPtNodePos[MAX_PREV_WORD_COUNT_FOR_N_GRAM];
    prevWordsInfo->getPrevWordsTerminalPtNodePos(
            mDictionaryStructureWithBufferPolicy.get(), prevWordsPtNodePos,
            true /* tryLowerCaseSearch */);
    return getDictionaryStructurePolicy()->getProbabilityOfPtNode(prevWordsPtNodePos, nextWordPos);
}

bool Dictionary::addUnigramEntry(const int *const word, const int length,
        const UnigramProperty *const unigramProperty) {
    if (unigramProperty->representsBeginningOfSentence()
            && !mDictionaryStructureWithBufferPolicy->getHeaderStructurePolicy()
                    ->supportsBeginningOfSentence()) {
        AKLOGE("The dictionary doesn't support Beginning-of-Sentence.");
        return false;
    }
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->addUnigramEntry(word, length, unigramProperty);
}

bool Dictionary::removeUnigramEntry(const int *const codePoints, const int codePointCount) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->removeUnigramEntry(codePoints, codePointCount);
}

bool Dictionary::addNgramEntry(const PrevWordsInfo *const prevWordsInfo,
        const BigramProperty *const bigramProperty) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->addNgramEntry(prevWordsInfo, bigramProperty);
}

bool Dictionary::removeNgramEntry(const PrevWordsInfo *const prevWordsInfo,
        const int *const word, const int length) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->removeNgramEntry(prevWordsInfo, word, length);
}

bool Dictionary::flush(const char *const filePath) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->flush(filePath);
}

bool Dictionary::flushWithGC(const char *const filePath) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->flushWithGC(filePath);
}

bool Dictionary::needsToRunGC(const bool mindsBlockByGC) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->needsToRunGC(mindsBlockByGC);
}

void Dictionary::getProperty(const char *const query, const int queryLength, char *const outResult,
        const int maxResultLength) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->getProperty(query, queryLength, outResult,
            maxResultLength);
}

const WordProperty Dictionary::getWordProperty(const int *const codePoints,
        const int codePointCount) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->getWordProperty(
            codePoints, codePointCount);
}

int Dictionary::getNextWordAndNextToken(const int token, int *const outCodePoints,
        int *const outCodePointCount) {
    TimeKeeper::setCurrentTime();
    return mDictionaryStructureWithBufferPolicy->getNextWordAndNextToken(
            token, outCodePoints, outCodePointCount);
}

} // namespace latinime
