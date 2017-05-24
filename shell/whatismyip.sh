#!/bin/sh
IP=`curl --silent http://api.ipaddress.com/myip?format=txt`
echo "${IP}"
