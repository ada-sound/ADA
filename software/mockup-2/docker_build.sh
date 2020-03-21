#!/bin/sh

# generate full xtimecomposer archive (split because of github max size)
rm -rf tools/xTIMEcomposer-Community_14-Linux64-Installer_Community_14.4.1.tgz
cat tools/xTIMEcomposer-Community_14-Linux64-Installer_Community_14.4.1.tgz.a* >> tools/xTIMEcomposer-Community_14-Linux64-Installer_Community_14.4.1.tgz

# build docker image
docker build -t ada_mockup-2 .
