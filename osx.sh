#!/bin/sh

make

./make-macosx.sh x86_64

cd build/release-darwin-i386/baseq3/

zip pak9.pk3 vm/*

mv pak9.pk3 ../../release-darwin-x86_64/mecwerks.app/Contents/MacOS/baseq3

cd ../../release-darwin-x86_64/

cp -a mecwerks.app ../../../tests/

cd ../../

./bot-sync.sh

cd code/

zip -rq pak10.pk3 botfiles/

mv pak10.pk3 ../../tests/baseq3/
