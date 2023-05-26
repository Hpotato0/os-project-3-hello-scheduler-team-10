#!/bin/bash
rm ./test/log.txt
wait 
mkdir mntdir
wait
sudo mount tizen-image/rootfs.img mntdir
wait
sudo mv mntdir/root/test test/log.txt
wait
sudo umount mntdir
wait
rm -r mntdir
wait
python test/sch.py