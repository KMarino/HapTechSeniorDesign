#### How to give BBB internet access through USB

http://robotic-controls.com/learn/beaglebone/beaglebone-internet-over-usb-only

It's a pain in the ass though. `c8:a0:30:ae:02:e0` is allowed on ResNet

#### Image src

https://eewiki.net/display/linuxonarm/BeagleBone+Black#BeagleBoneBlack-Debian7(smallflash)


remove and disable crap http://www.geekdroppings.com/tag/beaglebone-black/

        apt-get remove ruby nodejs

        systemctl disable bonescript.service
        systemctl disable bonescript.socket
        systemctl disable bonescript-autorun.service
        systemctl disable cloud9.socket

        update-rc.d apache2 disable
        systemctl disable apache2.service
        apt-get remove apache2

        apt-get autoremove
