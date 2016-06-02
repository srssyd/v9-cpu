#!/bin/sh
rm -f xc xem dis emhello funcall os0 os1 os2 os3 os4
gcc -o xc -O3 -m32 -Ilinux -Iroot/lib root/bin/c.c
gcc -o xem -O3 -m32 -Ilinux -Iroot/lib root/bin/em.c -lm
gcc -o dis -O3 root/bin/dis.c
./xc -o os2 -Iroot/lib root/usr/os/os2.c
./dis os2 -o os2.s -c
