#!/bin/bash
TARGET=$1

./factorizeLoop_hide 1 100000 &
./factorizeLoop_hide 2 100000 &
./factorizeLoop_hide 3 100000 &
./factorizeLoop_hide 4 100000 &
./factorizeLoop_hide 5 100000 &
./factorizeLoop_hide 6 100000 &
./factorizeLoop_hide 7 100000 &
./factorizeLoop_hide 8 100000 &
./factorizeLoop_hide 9 100000 &
./factorizeLoop_hide 10 100000 &
./factorizeLoop_hide 11 100000 &
./factorizeLoop_hide 12 100000 &
./factorizeLoop_hide 13 100000 &
./factorizeLoop_hide 14 100000 &
./factorizeLoop_hide 15 100000 &
./factorizeLoop_hide 16 100000 &
./factorizeLoop_hide 17 100000 &
./factorizeLoop_hide 18 100000 &
./factorizeLoop_hide 19 100000 &
./factorizeLoop_hide 20 100000 &

./factorizeLoop "$TARGET" 10000 
