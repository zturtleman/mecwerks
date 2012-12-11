#!/bin/bash

#
# Set zero's for error checking
#
OLD_REV=0
NEW_REV=0
PATCH=0

#
# Makefile.local is used to get current revision number 
#
OLDCFG=REVISION
OLD_REV=`grep "$SPEAR_REV =" "$OLDCFG" | cut -f3 -d' '`

#
# Read command line arguments
#
for ARG in `echo $*`
do
	#
	# See if we show help
	#
        if [ "$ARG" = "--help" ] || [ "$ARG" = "-help" ] || [ "$ARG" = "-h" ]
        then
                                        echo ""
                echo "This script auto updates code to syncronize with the spearmint code"
                echo "A patch is put in diffs/diff_<oldrev:<newrev>.patch"
                echo ""
                echo ""
                echo "  OPTIONS"
                echo "    -help         Show this help"
                echo "    -h"
                echo ""
                echo "    -patch       apply patch after created"
                exit 1
                exit 1;
        fi

	#
	# See if we apply patch or just create it
	#
	if [ "$ARG" = "-patch" ]
	then
		PATCH=1
		break
	fi
done

#
# Error checking for current revision
#
if [ "$OLD_REV" = 0 ]
then
	echo "No old revision set. Make a line in Makefile.local as SPEAR_REV = <current updated revision>"
	error 1;
fi

#
# Make sure we are in the spearmint code directory
#
cd ../spear

#
# Set new revision to compare with
#
NEW_REV=`svn log -rHEAD | grep "^r[0-9]" | cut -f1 -d' ' | cut -c 2-`

#
# Make sure we aren't comparing same revision
#
if [ "$OLD_REV" = "$NEW_REV" ]
then
	echo "Revision Already Current"
	exit 1;
fi

#
# Update spearmint code
#
svn update > checkout.log

#
# Make the patch
#
echo "Making patch"
svn diff -r $OLD_REV:$NEW_REV > ../mecwerks/patches/diff_$OLD_REV-$NEW_REV.patch


#
# Patch if commandline was passed
#
if [ "$PATCH" = "1" ]
then
	#
	# Update Makefile.local
	#
	cd ../mecwerks
	echo "SPEAR_REV = $NEW_REV" > $OLDCFG
	echo "Makefile.local updated"

	echo "Patching"
	patch -p0 < patches/diff_$OLD_REV-$NEW_REV.patch

	git add patches/diff_$OLD_REV-$NEW_REV.patch
	echo "Added Patch to repo"
fi
