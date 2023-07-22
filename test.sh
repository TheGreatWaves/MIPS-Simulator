#!/bin/bash
cd spim_assembler
python3 ./asm2hex "../all.s"
cd ../src
make clean && make
./sim "../all.x"


