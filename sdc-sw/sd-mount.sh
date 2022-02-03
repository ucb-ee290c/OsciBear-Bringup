#!/bin/bash

set -ex

if [ $# -eq 0 ]; then
    echo "usage: sd-mount.sh <PARTITION> <MOUNT_DIR>"
    exit 1
fi

PARTITION=$1
MOUNT_DIR=$2

# fix partition (if broken)
fsck.hfsplus -r $PARTITION

# mount
mount $PARTITION $MOUNT_DIR

echo "Mounted $PARTITION to $MOUNT_DIR"
