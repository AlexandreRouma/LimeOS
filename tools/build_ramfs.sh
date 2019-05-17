#!/bin/sh

# Cleaning up modules
rm ../ramfs/mod/*

# Copy modules
cp ../bin/i686/modules/*.elf ../ramfs/mod/

# Build tar archive
cd ../ramfs
tar -cvf ../bin/i686/ramfs.tar *