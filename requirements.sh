#!/bin/bash
set -e

sudo apt-get update
sudo apt-get -y upgrade
sudo apt-get -y install build-essential git qemu-system-mips python3 cmake verilator libboost-dev parallel