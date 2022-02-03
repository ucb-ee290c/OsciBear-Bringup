#!/bin/bash

TTY=ttyUSB1

screen='screen -S OSCIUART'

$screen /dev/${TTY} 115200
