#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dbus_server.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <limits.h>

#include "third_party/android_prediction/suggest/core/dictionary/dictionary.h"
#include "third_party/android_prediction/suggest/core/result/suggestion_results.h"
#include "third_party/android_prediction/suggest/core/session/dic_traverse_session.h"
#include "third_party/android_prediction/suggest/core/session/prev_words_info.h"
#include "third_party/android_prediction/suggest/core/suggest_options.h"
#include "third_party/android_prediction/suggest/policyimpl/dictionary/structure/dictionary_structure_with_buffer_policy_factory.h"

static const char *ECHO_SERVER_NAME = "org.freedesktop.DBus.Examples.Echo";
static const char *ECHO_SERVER_PATH = "/org/freedesktop/DBus/Examples/Echo";

ECServer::ECServer(DBus::Connection &connection)
: DBus::ObjectAdaptor(connection, ECHO_SERVER_PATH)
{
	this->ds = new prediction::DictionaryService();
}

int32_t ECServer::Random()
{
	return rand();
}

std::vector<std::string> ECServer::Hello(const std::string &word, const std::vector<std::string>& prev_words)
{
	std::cout << "HELLO" << std::endl;
	std::cout << word << std::endl;
	prediction::PredictionInfo * prediction_info = new prediction::PredictionInfo();

	for (auto prev_word : prev_words) {
		std::cout << prev_word << std::endl;
		prediction_info->previous_words.push_back(
			new prediction::PrevWordInfo(prev_word, false)
		);
	}

	prediction_info->current_word = word;
	prediction::ProximityInfoFactory fac = prediction::ProximityInfoFactory();
	latinime::ProximityInfo* proximity_info = fac.GetNativeProximityInfo();

 //    using structure_policy_ptr = latinime::DictionaryStructureWithBufferPolicy::StructurePolicyPtr;
 //    structure_policy_ptr dict_ptr = latinime::DictionaryStructureWithBufferPolicyFactory::
 //                newPolicyForExistingDictFile("../dicts/main_en.dict", 0, 0x0, true);

 //    if (!dict_ptr) {
 //        return std::make_tuple(reinterpret_cast<latinime::Dictionary*>(NULL), 
 //                               reinterpret_cast<latinime::DicTraverseSession*>(NULL));
 //    }

 //    latinime::Dictionary* dictionary = new latinime::Dictionary(
 //        std::move(dict_ptr));

 //    latinime::DicTraverseSession* session = reinterpret_cast<latinime::DicTraverseSession*>(
 //            latinime::DicTraverseSession::getSessionInstance("en", size - offset));
 //    latinime::PrevWordsInfo empty_prev_words;
 //    session->init(dictionary, &empty_prev_words, 0);

	// auto suggestions = ds->GetDictionarySuggestion(dict, sess, prediction_info, proximity_info);

	// std::string result = "";
	// for (auto s : suggestions) {
	// 	std::cout << s << std::endl;
	// 	result += s + " ";
	// }
	// std::cout << "Got suggestions" << std::endl;
	// // return "Hello " + result + "!";
	std::vector<std::string> suggestions = {"Does", "not", "work", "right", "now"};
	return suggestions;

}

DBus::Variant ECServer::Echo(const DBus::Variant &value)
{
	this->Echoed(value);

	return value;
}

std::vector< uint8_t > ECServer::Cat(const std::string &file)
{
	FILE *handle = fopen(file.c_str(), "rb");

	if (!handle) throw DBus::Error("org.freedesktop.DBus.EchoDemo.ErrorFileNotFound", "file not found");

	uint8_t buff[1024];

	size_t nread = fread(buff, 1, sizeof(buff), handle);

	fclose(handle);

	return std::vector< uint8_t > (buff, buff + nread);
}

int32_t ECServer::Sum(const std::vector<int32_t>& ints)
{
	int32_t sum = 100;

	for (size_t i = 0; i < ints.size(); ++i) sum += ints[i];

		return sum;
}

std::map< std::string, std::string > ECServer::Info()
{
	std::map< std::string, std::string > info;
	char hostname[HOST_NAME_MAX];

	gethostname(hostname, sizeof(hostname));
	info["hostname"] = hostname;
	info["username"] = getlogin();

	return info;
}


DBus::BusDispatcher dispatcher;

void niam(int sig)
{
	dispatcher.leave();
}

int main()
{
	signal(SIGTERM, niam);
	signal(SIGINT, niam);

	DBus::default_dispatcher = &dispatcher;

	DBus::Connection conn = DBus::Connection::SessionBus();
	conn.request_name(ECHO_SERVER_NAME);

	ECServer server(conn);

	dispatcher.enter();

	return 0;
}
