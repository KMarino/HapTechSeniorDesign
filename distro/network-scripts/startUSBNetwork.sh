#!/bin/sh
echo "Setup script for the EE402 module - Derek Molloy"
echo "Setting up the default gateway"
ifconfig usb0 192.168.7.2
route add default gw 192.168.7.1

echo "Updating the nameserver entry"
echo "nameserver 8.8.8.8" >> /etc/resolv.conf

echo "Setting the time using the Irish ntp pool"
ntpdate -b -s -u us.pool.ntp.org
