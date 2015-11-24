#!/bin/ash

pidof adc | xargs kill -9
pidof control | xargs kill -9
pidof motor | xargs kill -9