#!/bin/bash
find . -maxdepth 2 -name .git -type d -exec grep -q ericherman \{}/config \; -print | cut -d'/' -f2 | grep -v '^x-'
