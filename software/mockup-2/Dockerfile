FROM ubuntu:14.04

# exporting desktop apps
RUN apt update -y && \
    apt upgrade -y && \
    apt install -y net-tools x11-apps

# installing jdk + xTIMEcomposer
ENV DESKTOP_SESSION gnome
RUN apt install -y default-jre packagekit-gtk3-module libcanberra-gtk-module
ADD tools/xTIMEcomposer-Community_14-Linux64-Installer_Community_14.4.1.tgz /opt/

ENV DISPLAY unix:1
#CMD ["xeyes"]
