#!/bin/bash

echo uart > /sys/devices/platform/ocp/ocp\:P9_24_pinmux/state
echo uart > /sys/devices/platform/ocp/ocp\:P9_26_pinmux/state

stty -F /dev/ttyO1 115200


hostapd /etc/hostapd/hostpad.conf
rfkill unblock wifi
ip link set dev wlan0 up



update-rc.d zbeagleLaunch.sh defaults


### BEGIN INIT INFO
# Provides:             robotstuff
# Required-Start:       $remote_fs
# Required-Stop:        $remote_fs
# Should-Start:         $network
# Should-Stop:
# Default-Start:        2 3 4 5
# Default-Stop:         0 1 6
# Short-Description:    
# Description:          
### END INIT INFO


/etc/network/if-up.d/beagleWifi.sh

i2cdetect -r 1 0x65 0x70

i2cdetect -r 2

i2cset -y 2 0x68 0x6b 0x00

i2cdump -y 2 0x68

#echo i2c > /sys/devices/platform/ocp/ocp\:P9_17_pinmux/state
#echo i2c > /sys/devices/platform/ocp/ocp\:P9_18_pinmux/state



ifconfig wlan0 192.168.0.40