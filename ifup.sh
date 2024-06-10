#!/bin/bash

tdn=tap0
host=10.0.2.1
mask=255.255.255.0

ip link set ${tdn} down
ifconfig ${tdn} ${host} netmask ${mask}
ip link set ${tdn} up

