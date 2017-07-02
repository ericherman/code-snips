#!/bin/bash
SIZE_OF_FILE=$(ls -l $1 | awk '{print $5}')
LAST_CHAR_POSITION=$(expr $SIZE_OF_FILE - 1)
LAST_CHAR=$(dd bs=1 if=$1 iseek=$LAST_CHAR_POSITION count=1)
if [ $LAST_CHAR != '\n' ]
then
   echo RESULT is _$LASTCHAR_
   echo '' >> $1
fi
