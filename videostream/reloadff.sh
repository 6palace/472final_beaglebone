#!/bin/bash
#pidof ffserver | xargs kill -9
sleep 15
ffserver -f /root/videostream/ffserver.conf > /dev/null &
sleep 5
ffmpeg -f video4linux2 -r 5 -s 176x144 -i /dev/video0 -vcodec mjpeg http://127.0.0.1:8090/feed1.ffm > /dev/null &
