#!/bin/sh

# Cleaning up drivers
rm ../ramfs/mod/*

# Copy drivers
cp ../bin/i686/drivers/* ../ramfs/mod/

# Build tar archive
cd ../ramfs
tar -cvf ../bin/i686/ramfs.tar *