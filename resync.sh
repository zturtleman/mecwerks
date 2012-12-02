#!/bin/bash

OLD_REV=0
NEW_REV=0
PATCH=0
#
# Read command line arguments
#
for ARG in `echo $*`
do
        #
        # Arguments that have a token after it
        #

        if [ "$ARG" = "-old" ] || [ "$ARG" = "-new" ] || [ "$ARG" = "-patch" ]
        then
                NEXT_ARG="$ARG"
                continue
        fi

        case "$NEXT_ARG" in
                -old)
                        OLD_REV="$ARG"
                        ;;
                -new)
                        NEW_REV="$ARG"
                        ;;
                -patch)
                        PATCH="1"
                        ;;
        esac
        NEXT_ARG=""

done

if [ "$OLD_REV" = "0" ] || [ "$NEW_REV" = "0" ]
then
	echo "error:need arguements for old and new revision, -old ### and -new ###\n"
	exit 1;
fi

echo "Checking out current spearmint revision\n"
cd ../spear
svn update > checkout.log

echo "Making patch\n"
svn diff -r $OLD_REV:$NEW_REV > ../mecwerks/diffs/diff_$OLD_REV-$NEW_REV.diff

if [ "$patch" = "1" ]
then
	echo "Patching\n"
	cd ../mecwerks
	patch -p0 < diffs/diff_$OLD_REV-$NEW_REV.diff
fi
