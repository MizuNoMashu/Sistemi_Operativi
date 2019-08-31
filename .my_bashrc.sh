#!/bin/bash

la () {
	echo 'ls -A' > .temp_bashrc
	return -1
}

ll () {
  	echo 'ls -l' > .temp_bashrc
  	return -1
}

aggiornami () {
	echo 'sudo apt update && sudo apt upgrade' > .temp_bashrc
	return -1
}

if declare -f "$1" > /dev/null
	then
		"$@"
		exit 2
else
	exit 1
fi
