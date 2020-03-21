# ADA_mockup-2

Table of Contents
- [ADA_mockup-2](#adamockup-2)
  - [build and run a docker image running XMOS xTIMEcomposer](#build-and-run-a-docker-image-running-xmos-xtimecomposer)
    - [why a docker image for running xtimecomposer?](#why-a-docker-image-for-running-xtimecomposer)
    - [building the docker image ..](#building-the-docker-image)
    - [.. and running xtimecomposer in the container](#and-running-xtimecomposer-in-the-container)

## build and run a docker image running XMOS xTIMEcomposer

xtimecomposer is the XMOS eclipse-based dev tool for the xCORE-200 eXplorerKIT


### why a docker image for running xtimecomposer?

.. because according to https://www.xmos.com/software/tools/ this tool runs on Centos 6.6 or Ubuntu 14.04 ..

And indeed the needed java version for eclipse cannot be installed on Ubuntu version earlier than 14.04.


### building the docker image ..

- Do it once only

        cd ADA/software/mockup-2
        ./docker_build.sh


### .. and running xtimecomposer in the container
    
- On your host PC

        cd ADA/software/mockup-2
        ./docker_run.sh

- In the container

        /xtimecontainer.sh


###
cf https://github.com/tom91136/coms20001_xtime_cli

    #cat /etc/udev/rules.d/92-xmos-x200.rules
    #SUBSYSTEM=="usb", ATTR{idVendor}=="20b1", ATTR{idProduct}=="f7d4", ACTION=="add", OWNER="<name>", MODE="0664"
    #sudo udevadm control --reload-rules && udevadm trigger
