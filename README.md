# Project 3: Weighted-Round-Robin 
============
- OS Team 10: Hyeon Seok kim, Jae Hyung Ju, Ju Won Hong
## Background
## How to test 
kernel build and run qmeu
```bash
~/sudo ./_build.sh
./sudo setup-images.sh
./mkdir mntdir
./sudo moount tizen-image/rootfs.img  mntdir
./sudo cp test/factorizLoop test/factorizLoop_hide test/log.sh test/setup.sh mntdir/root
./_run.sh
// in virtual machine
./setup.sh
./log.sh "test weight"
```
### WRR schedling test
### Load balancing test
### Error check
