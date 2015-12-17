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
    sleep 10
    echo "Pinmuxing..."
    echo uart > /sys/devices/platform/ocp/ocp\:P9_24_pinmux/state
    echo uart > /sys/devices/platform/ocp/ocp\:P9_26_pinmux/state
    i2cset -y 2 0x68 0x6b 0x00
    i2cdump -y 2 0x68
    stty -F /dev/ttyO1 115200
    echo "WiFi Config"
    hostapd /etc/hostapd/hostapd.conf & 
    sleep 4
    echo Setting IP
    ifconfig wlan0 192.168.0.40 &
    echo WiFi AP Created
    /root/videostream/reloadff.sh > /dev/null &
    echo Webcam Server Started
    sleep 2
    /root/ws/wsServer &
    echo Websocket Server Started
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
