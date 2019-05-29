#!/bin/bash
MY_REPOS=$(my-repos.sh)
for dir in $MY_REPOS; do pushd $dir > /dev/null; git status 2>/dev/null| grep -q modified && echo $dir; popd > /dev/null; done
