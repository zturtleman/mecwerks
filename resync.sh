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
                echo "A patch is put in patches/diff_<oldrev:<newrev>.patch"
                echo ""
                echo ""
                echo "  OPTIONS"
                echo "    -help         Show this help"
                echo "    -h"
                echo ""
                echo "    -patch       apply patch after created"
                exit 1
        fi
	
	#
	# Check other arguements
	#
	if [ "$ARG" == "-newrev" ] || [ "$ARG" == "-oldrev" ] || [ "$ARG" == "-patch" ]
	then
		NEXT_ARG="$ARG"
	fi

	case "$NEXT_ARG" in
		-newrev)
			NEW_REV="$ARG"
			;;
		-oldrev)
			OLD_REV="$ARG"
			;;
		-patch)
			PATCH=1
			;;
		*)
			echo "Unkown arguement '$ARG'"
			exit 1
			;;
	esac
	NEXT_ARG=""
done

#
# Error checking for current revision
#
if [ "$OLD_REV" = 0 ]
then
	echo "No old revision set. Check REVISION"
	error 1
fi

#
# Make sure we are in the spearmint code directory
#
cd ../spear

#
# Set new revision to compare with
#
if [ "$NEW_REV" = "0" ]
then
	NEW_REV=`svn log -rHEAD | grep "^r[0-9]" | cut -f1 -d' ' | cut -c 2-`
fi

#
# Make sure we aren't comparing same revision
#
if [ "$OLD_REV" = "$NEW_REV" ]
then
	echo "Revision Already Newest"
	exit 1
fi

#
# Make sure we have a new revision to sync to
#
if [ "$NEW_REV" = "" ]
then
	echo "No New Revision Set"
	exit 1
fi

#
# Update spearmint code
#
echo "Updating Spearmint code"
svn update > checkout.log

#
# If there's a commitlog there move it to a temp file
#
if [ -f ../mecwerks/commitlog ]
then
	mv ../mecwerks/commitlog ../mecwerks/tmpcommit
	echo "Found old commit log and moved it to tmpcommit"
fi

#
# Add logs to commit log
#
for (( i = $OLD_REV; i <= $NEW_REV; i++ ))
do
	svn log -r$i >> ../mecwerks/commitlog
	echo "Added commit log message for revision $i to commitlog"
done

#
# Clean up the commit log
#
grep -v - ../mecwerks/commitlog > ../mecwerks/commitlog.tmp
rm -rf ../mecwerks/commitlog
mv ../mecwerks/commitlog.tmp ../mecwerks/commitlog

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
	# Update REVISION
	#
	cd ../mecwerks
	echo "SPEAR_REV = $NEW_REV" > $OLDCFG
	echo "REVISION updated"

	echo "Patching"
	patch -p0 < patches/diff_$OLD_REV-$NEW_REV.patch

	git add patches/diff_$OLD_REV-$NEW_REV.patch
	echo "Added Patch to repo"
fi
