#!/bin/bash

USE=0
FAIL=0

for ARG in `echo $*`
do
	if [ "$ARG" = "-alt" ]
	then
		USE=1
	fi
done

echo "Patching"
if [ "$USE" = "1" ]
then
	patch -p1 < "$ARG"
	exit 1
fi

patch -p0 < "$ARG"
