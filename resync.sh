#!/bin/bash

#Tool to add patch to repo with
TOOL=git
#File to store new commitlog to
COMMIT_LOG=commitlog
#File to store old commit log at if one exists
OLDCOMMIT_LOG=commit.old
#File to move commit log to when fixing it
TMPCOMMIT_LOG=commit.tmp
#Repository that the spearmint code is in
SCODE=http://ioq3ztm.googlecode.com/svn/trunk/
#Set this to zero for error checking
OLD_REV=0
#Set this to zero for error checking
NEW_REV=0
#This is the one-lined file that holds the current spearmint revision (SPEAR_REV = REVISION#)
OLDCFG=REVISION
#This sets the current revision using the cfg file
OLD_REV=`grep "$SPEAR_REV =" "$OLDCFG" | cut -f3 -d' '`
#This is normally set to one so the patch is applied after it is created
PATCH=1
#This is normally set to one so the patch will be added to the workng repository
ADD_PATCH=1
#The directory to put the patch file
PATCH_DIR=patches
#The prefix to put on the patch name
PATCH_PREFIX=diff_
#The extension of the patch file
PATCH_SUFFIX=patch

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
                echo "This script automatically updates code to syncronize with Spearmint"
                echo "A patch is put in $PATCH_DIR/$PATCH_PREFIX<oldrev>:<newrev>.$PATCH_SUFFIX"
                echo ""
		echo "  OPTIONS:"
                echo "    -scode <arg>          Set source code svn repo"
                echo "                                  (default: $SCODE)"
                echo "    -newrev <arg>         Manually set newest revision"
                echo "    -oldrev <arg>         Manually set current revision"
                echo "    -tmpcommitlog <arg>   Set temporary commit log"
                echo "                                  (default: $TMPCOMMIT_LOG)"
                echo "    -oldcommitlog <arg>   Set what to store old commit log as"
                echo "                                  (default: $OLDCOMMIT_LOG)"
                echo "    -newcommitlog <arg>   Set what to store new commit log as"
                echo "                                  (default: $COMMIT_LOG)"
                echo "    -tool <arg>           Set tool to be used for adding patch to repo (git, svn, etc.)"
                echo "                                  (default: $TOOL)"
                echo "    -cfgfile <arg>        Set cfg file to get current revision from"
                echo "                                  (default: $OLDCFG)"
                echo "    -patchdir <arg>       Set the directory to place the patch"
                echo "                                  (default: $PATCH_DIR)"
                echo "    -patchprefix <arg>    Set the prefix to add to the patches name"
                echo "                                  (default: $PATCH_PREFIX)"
                echo "    -patchsuffix <arg>    Set the extension to use for the patch"
                echo "                                  (default: $PATCH_SUFFIX)"
                echo "    -nopatch              Do not apply patch after it's created"
                echo "    -noadd                Do not add patch to repo"
                echo "    -help                 Show this help"
                echo "    -h"
                echo "To permanently set a variable edit this script"
                exit 1
        fi
	
	#
	# Check other arguements
	#
	if [ "$ARG" == "-newrev" ] || [ "$ARG" == "-oldrev" ] || [ "$ARG" == "-nopatch" ] || [ "$ARG" == "-tmpcommitlog" ] || [ "$ARG" == "-oldcommitlog" ] || [ "$ARG" == "-newcommitlog" ] || [ "$ARG" == "-tool" ] || [ "$ARG" == "-cfgfile" ] || [ "$ARG" == "-patchdir" ] || [ "$ARG" == "-patchprefix" ] || [ "$ARG" == "-patchsuffix" ] || [ "$ARG" == "-noadd" ] || [ "$ARG" == "-scode" ]
	then
		NEXT_ARG="$ARG"
	fi

	case "$NEXT_ARG" in
		-tmpcommitlog)
			TMPCOMMIT_LOG="$ARG"
			;;
		-oldcommitlog)
			OLDCOMMIT_LOG="$ARG"
			;;
		-newcommitlog)
			COMMIT_LOG="$ARG"
			;;
		-tool)
			TOOL="$ARG"
			;;
		-cfgfile)
			OLDCFG="$ARG"
			;;
		-patchdir)
			PATCH_DIR="$ARG"
			;;
		-patchprefix)
			PATCH_PREFIX="$ARG"
			;;
		-patchsuffix)
			PATCH_SUFFIX="$ARG"
			;;
		-noadd)
			ADD_PATCH="0"
			;;
		-scode)
			SCODE="$ARG"
			;;
		-newrev)
			NEW_REV="$ARG"
			;;
		-oldrev)
			OLD_REV="$ARG"
			;;
		-nopatch)
			PATCH=0
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
	echo "No old revision set. Make sure there is a line in $OLDCFG as 'SPEAR_REV = <REVISION###>'"
	error 1
fi

#
# Set new revision to compare with
#
if [ "$NEW_REV" = "0" ]
then
	NEW_REV=`svn log $SCODE -rHEAD | grep "^r[0-9]" | cut -f1 -d' ' | cut -c 2-`
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
# If there's a commitlog there move it to a temp file
#
if [ -f $COMMIT_LOG ]
then
	mv $COMMIT_LOG $OLDCOMMIT_LOG
	echo "Found old $COMMIT_LOG and moved it to $OLDCOMMIT_LOG"
fi

#
# Add logs to commit log
#
for (( i = $OLD_REV; i <= $NEW_REV; i++ ))
do
	if [ "$i" = "$OLD_REV" ]
	then
		echo "Updated to Spearmint $NEW_REV" >> $COMMIT_LOG
		echo "" >> $COMMIT_LOG
	else
		echo "- Spearmint $i" >> $COMMIT_LOG
		svn log $SCODE -r$i >> $COMMIT_LOG
		echo "Added commit log message for revision $i to $COMMIT_LOG"
	fi
done

#
# Clean up the commit log
#
grep -v -- -- $COMMIT_LOG > $TMPCOMMIT_LOG
rm -rf $COMMIT_LOG
grep -v ZTurtleMan $TMPCOMMIT_LOG > $COMMIT_LOG
rm -rf $TMPCOMMIT_LOG

#
# Make the patch
#
echo "Making patch"
svn diff $SCODE -r $OLD_REV:$NEW_REV > $PATCH_DIR/$PATCH_PREFIX$OLD_REV-$NEW_REV.$PATCH_SUFFIX


#
# Patch if commandline was passed
#
if [ "$PATCH" = "1" ]
then
	#
	# Update REVISION
	#
	echo "SPEAR_REV = $NEW_REV" > $OLDCFG
	echo "$OLDCFG updated"

	#
	# Apply Patch
	#
	echo "Patching"
	patch -p0 < $PATCH_DIR/$PATCH_PREFIX$OLD_REV-$NEW_REV.$PATCH_SUFFIX

	#
	# Add Patch to repo
	#
	if [ "$ADD_PATCH" = "1" ]
	then
		$TOOL add $PATCH_DIR/$PATCH_PREFIX$OLD_REV-$NEW_REV.$PATCH_SUFFIX
		echo "Added $PATCH_PREFIX$OLD_REV-$NEW_REV.$PATCH_SUFFIX to repo"
	fi

	#
	# Start the commit
	#
	$TOOL commit -a -t $COMMIT_LOG
fi
