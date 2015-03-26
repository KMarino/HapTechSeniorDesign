#!/bin/bash
ifconfig wlp2s0 192.168.7.1
iptables --table nat --append POSTROUTING --out-interface enp3s0u1 -j MASQUERADE
iptables --append FORWARD --in-interface wlp2s0 -j ACCEPT
echo 1 > /proc/sys/net/ipv4/ip_forward
