#! /bin/bash
#
# Changes every filename in working directory to all lowercase.
#
# Inspired by a script of John Dubois,
# which was translated into into Bash by Chet Ramey,
# and considerably simplified by Mendel Cooper, author of this document.


for filename in *                # Traverse all files in directory.
do
   fname=`basename $filename`
   n=`echo $fname | tr A-Z a-z`  # Change name to lowercase.
   if [ "$fname" != "$n" ]       # Rename only files not already lowercase.
   then
     mv -iv $fname $n
   fi
done

