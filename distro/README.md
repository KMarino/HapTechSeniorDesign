It's a pain in the ass though. `c8:a0:30:ae:02:e0` is allowed on ResNet

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

(maybe do a reboot after just to be save)

Aquila requires `sfml` version 2, but the repos only provide 1.6. Also `c++11` needs `gcc` 4.7+. So before installing, go ahead and add

        deb http://ftp.us.debian.org/debian sid main
        
to `/etc/apt/sources.list` and `apt-get update`

and then reinstall gcc

        apt-get install gcc g++ make cmake
        
and all the rest of the software. 


For aquila, before building, change `CMakeLists.txt` to use an older version of cmake (it will still work):

        cmake_minimum_required(VERSION 2.8.9)
