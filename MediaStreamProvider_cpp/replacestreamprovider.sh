#!/bin/bash
#

systemctl stop fxn-camera-MediaProvider
sleep 3
mv fxnMediaStreamProvider /usr/bin
sync
systemctl start fxn-camera-MediaProvider

