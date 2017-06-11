#!/bin/bash

echo -n -e "\nwhat is your name? "

read NAME

if [ "_${NAME}_" = "__" ]; then
	NAME="world";
fi

echo -e "\nhello, $NAME\n"
