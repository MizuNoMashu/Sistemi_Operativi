#!/bin/bash
BASEDIR=$(dirname "$0")
FILE=".temp_bashrc"
FULL_PATH="$BASEDIR/$FILE"

la () {
	echo 'ls -A' > $FULL_PATH
	return -1
}

ll () {
  	echo 'ls -l' > $FULL_PATH
  	return -1
}

aggiornami () {
	echo 'sudo apt update && sudo apt upgrade' > $FULL_PATH
	return -1
}

if declare -f "$1" > /dev/null
	then
		"$@"
		exit 2
else
	exit 1
fi
