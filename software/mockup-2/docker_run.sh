#!/bin/sh
xhost +
docker run --rm -it --net host --privileged -v /dev/bus/usb:/dev/bus/usb -v $(realpath src):/root/workspace ada_mockup-2
