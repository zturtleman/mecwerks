#!/bin/sh

make

cd build/release-linux-x86_64/baseq3

zip pak9.pk3 vm/*

mv pak9.pk3 ../../../../tests/baseq3/

cd ../

cp * ../../../tests/

cd ../../code/

zip -rq pak10.pk3 botfiles/

mv pak9.pk3 ../../tests/baseq3/
