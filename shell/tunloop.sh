#!/bin/sh
while true
do
  #ssh -N -L6667:irc-new.mysql.com:6667 eherman@intranet.mysql.com
  #ssh -N -L6667:irc-new.mysql.com:6667 eherman@mail.mysql.com
  ssh -N $1
  date
  sleep 10
done
