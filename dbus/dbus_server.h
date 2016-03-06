#ifndef __DEMO_ECHO_SERVER_H
#define __DEMO_ECHO_SERVER_H

#include <dbus-c++/dbus.h>
#include "dbus_server_glue.h"
#include "prediction/dictionary_service.h"

class ECServer
: public org::freedesktop::DBus::EchoDemo_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:

	ECServer(DBus::Connection &connection);

	int32_t Random();

	std::vector<std::string> Hello(const std::string &word, const std::vector<std::string>& prev_words);

	DBus::Variant Echo(const DBus::Variant &value);

	std::vector< uint8_t > Cat(const std::string &file);

	int32_t Sum(const std::vector<int32_t> & ints);

	std::map< std::string, std::string > Info();
	prediction::DictionaryService * ds;
};

#endif//__DEMO_ECHO_SERVER_H
