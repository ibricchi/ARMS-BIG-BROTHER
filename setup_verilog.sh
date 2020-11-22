#!/bin/bash
set -e

#git clone https://github.com/steveicarus/iverilog.git
cd iverilog

sudo apt-get install -y gperf autoconf bison gcc make flex

sh autoconf.sh

./configure

make

sudo make install

cd ../
rm -rf iverilog