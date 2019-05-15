#!/bin/sh

# Copy files
cp ../bin/i686/LimeOS.bin ../bin/i686/iso/boot/LimeOS.bin
cp ../bin/i686/ramfs.tar ../bin/i686/iso/ramfs.tar

# Make iso
grub-mkrescue -o ../bin/i686/LimeOS.iso ../bin/i686/iso