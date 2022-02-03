#!/bin/bash

set -ex

if [ $# -eq 0 ]; then
    echo "usage: sync-to-mount.sh <MOUNT_DIR>"
    exit 1
fi

rsync -av --delete sync-files/ $1
