#!/bin/bash

ADDED=0;
REMOVED=0;

while read line; 
do
  echo "$line" | grep -q "^+" && ADDED=$(($ADDED+1)) 
  echo "$line" | grep "^-" | grep -vq "^---" && REMOVED=$(($REMOVED+1))
done < $1 

echo "Added: $ADDED"
echo "Removed: $REMOVED"
