#!/bin/bash

# chmod u+x SetPebble.sh
echo Setting Pebble Development Environment

echo 'export PATH=~/pebble-dev/PebbleSDK-2.7/bin:$PATH' >> ~/.bash_profile
. ~/.bash_profile

pebble --version

# Install Sdk
# 	http://developer.getpebble.com/sdk/install/linux/
# How to build info
# 	http://developer.getpebble.com/sdk/migration-guide/
#	http://developer.getpebble.com/guides/publishing-tools/pebble-tool
# pebble new-project helloworld
# pebble install --phone 192.168.1.5 --logs
