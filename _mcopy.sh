#!/bin/bash
TARGET=$1
mkdir mntdir
wait
sudo mount tizen-image/rootfs.img ./mntdir
wait
sudo cp "$TARGET" ./mntdir/root
wait
sudo umount ./mntdir
wait
rm -r mntdir
