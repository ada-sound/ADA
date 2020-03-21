# ADA_mockup-2

## build and run a docker image running XMOS xTIMEcomposer

### build

    cd ADA/software/mockup-2
    docker build -t ada_mockup-2 .

### run

    xhost +
    docker run --rm -it --net host ada_mockup-2

###
cf https://github.com/tom91136/coms20001_xtime_cli

    cat /etc/udev/rules.d/92-xmos-x200.rules
    SUBSYSTEM=="usb", ATTR{idVendor}=="20b1", ATTR{idProduct}=="f7d4", ACTION=="add", OWNER="<name>", MODE="0664"

    sudo udevadm control --reload-rules && udevadm trigger
