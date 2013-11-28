Provisioning Service
====================

Compile with
sb2 /usr/lib/qt5/bin/qmake
sb2 make

Usage:
 * Install mmsd
 * Copy .service file to D-Bus service directory (/usr/share/dbus-1/services/)
 * Copy .conf file to mmsd push configuration directory (/usr/lib/mms/push.d/)
 * Copy executable to /usr/libexec/
 * Start mmsd
 * Receive provisioning message from operator
 * mmsd will start provisioning service via D-Bus
 * provisioning service will create new oFono context(s)

