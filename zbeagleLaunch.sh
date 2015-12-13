#!/bin/sh

### BEGIN INIT INFO
# Provides:             robotstuff
# Required-Start:       $network
# Required-Stop:        
# Should-Start:         
# Should-Stop:
# Default-Start:        2 3 4 5
# Default-Stop:         0 1 6
# Short-Description:    
# Description:          
### END INIT INFO

echo BeagleBone Launch Script For Robot
# Carry out specific functions when asked to by the system
case "$1" in
  start)
    echo "Rev1"
    echo "Pinmuxing..."
    echo uart > /sys/devices/platform/ocp/ocp\:P9_24_pinmux/state
    echo uart > /sys/devices/platform/ocp/ocp\:P9_26_pinmux/state
    sleep 10
    echo "WiFi Config"
    hostapd /etc/hostapd/hostapd.conf
    echo WiFi AP Created
    ;;
  stop)
    echo "Stopping script blah"
    echo "Could do more here"
    ;;
  *)
    echo "Usage: /etc/init.d/blah {start|stop}"
    exit 1
    ;;
esac

exit 0
