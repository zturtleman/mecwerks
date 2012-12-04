#!/bin/bash

for ARG in `echo $*`
do
	if [ "$ARG" = "" ]
	then
		echo "No patch file"
		exit 1;
	fi
done

echo "Patching"
patch -p0 < "$ARG"
