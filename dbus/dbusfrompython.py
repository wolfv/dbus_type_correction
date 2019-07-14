import dbus

s = dbus.SessionBus()

proxy = s.get_object('org.freedesktop.DBus.Examples.Echo',
                       '/org/freedesktop/DBus/Examples/Echo')

props = proxy.Hello('teassing', ['a'], dbus_interface='org.freedesktop.DBus.EchoDemo')
print(props)
