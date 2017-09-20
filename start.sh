#!/bin/bash
cd /home/pi/flypi
sudo ./flypi.out &
cd /home/pi/clipi/server
node index.js &

#if you want to view camera,comment out following command
# bash ./cam.sh &
