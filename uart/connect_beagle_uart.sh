#!/bin/bash

set -ex

TTY=ttyUSB0

AMOUNT_OF_CONNECTIONS_W_GREP=$(ps -ef | grep -c $TTY)
if [ $AMOUNT_OF_CONNECTIONS_W_GREP -gt 1 ]; then
    echo "Warning: $(($AMOUNT_OF_CONNECTIONS_W_GREP-1)) other connections found to /dev/$TTY. Might see garbled/slowed/weird behavior."
    sleep 5
fi

screen='screen -S BEAGLEUART'

$screen /dev/$TTY 115200
