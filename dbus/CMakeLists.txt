

add_custom_command(OUTPUT dbus_server_glue.h 
    COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/dbus_server_introspect.xml --adaptor=${CMAKE_CURRENT_SOURCE_DIR}/dbus_server_glue.h)
# target_link

include_directories(
	/usr/include/dbus-c++-1
	..
)

add_executable(dbus_server
  EXCLUDE_FROM_ALL
	dbus_server.cpp
	dbus_server.h
	dbus_server_glue.h
)

target_link_libraries(dbus_server
	dbus-c++-1
	prediction_lib
)