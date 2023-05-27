#!/bin/bash

rm test
touch test
dmesg --level 7 | tail -n 1000 > test