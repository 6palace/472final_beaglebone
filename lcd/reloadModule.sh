#!/bin/ash

rmmod gpioKern
rmmod lcdmod
rmmod srmod
insmod srmod.ko
insmod lcdmod.ko
insmod gpioKern.ko