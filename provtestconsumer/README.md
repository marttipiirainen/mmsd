Provisioning test consumer
==========================

Testing OMA settings provisioning via WAP Push. This consumer receives provisioning
messages from mmsd, and just prints the results to file /tmp/prov.out.

The files need to be copied to these locations in the device:
  /usr/libexec/prov-test-consumer
  /usr/share/dbus-1/services/prov-test-consumer.service
  /usr/lib/mms/push.d/push.conf

