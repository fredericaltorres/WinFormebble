#!/bin/bash

echo Compile Application FredTime
echo ----------------------------
reset
clear
pebble build
pebble install --phone 192.168.1.5 --logs
