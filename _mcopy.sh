#!/bin/bash
TARGET=$1

sudo mount tizen-image/rootfs.img ./mntdir
sudo cp "$TARGET" ./mntdir/root
sudo umount ./mntdir