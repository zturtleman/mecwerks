#!/bin/sh

make

./make-macosx.sh

cd build/release-darwin-i386/baseq3

zip pak9.pk3 vm/*

mv pak9.pk3 ../../release-darwin-x86_64/spearmint.app/Contents/MacOS/baseq3

cd ../../release-darwin-x86_64/

cp -a spearmint.app ../../../tests/
