#!/bin/bash

ciaone () {
  echo "AAAAAAAA" > .temp_bashrc
}

la () {
	echo 'ls -A' > .temp_bashrc
}

ll () {
  ls -l "$@"
}
