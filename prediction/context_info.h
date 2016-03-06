#ifndef CONTEXTINFO_PREDICTION_DICTIONARY_SERVICE_H_
#define CONTEXTINFO_PREDICTION_DICTIONARY_SERVICE_H_

namespace prediction {
class PrevWordInfo {
public:
	std::string word;
	bool is_beginning_of_sentence;
	PrevWordInfo(std::string wd, bool is_beginning_of_sentence) : 
		word(wd), is_beginning_of_sentence(is_beginning_of_sentence)
		{}
};
typedef PrevWordInfo* PrevWordInfoPtr;

class PredictionInfo {
public:
	std::string current_word;
	std::vector<PrevWordInfoPtr> previous_words;
};
typedef PredictionInfo* PredictionInfoPtr;
}
#endif