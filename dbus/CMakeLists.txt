

add_custom_command(OUTPUT dbus_server_glue.h 
    COMMAND dbusxx-xml2cpp dbus_server_introspect.xml --adaptor=dbus_server_glue.h)
# target_link

include_directories(
	/usr/include/dbus-c++-1
	..
)

add_executable(dbus_server
	dbus_server.cpp
	dbus_server.h
	dbus_server_glue.h
)

target_link_libraries(dbus_server
	/usr/lib64/libdbus-c++-1.so
	prediction_lib
	)