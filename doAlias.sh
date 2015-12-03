#!/bin/bash

alias gopwm="cd /sys/class/pwm"
alias goocp="cd /sys/devices/platform/ocp"


!!!!!!!scratchpad below

pin 42

ls /sys/devices/platform/ocp/subsystem/devices/48304100.ecap/pwm | grep -o '[0-9]'