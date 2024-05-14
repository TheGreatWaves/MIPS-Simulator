#!/bin/bash
inpt_name="$1"
new_ext=".x"
opt_name="${inpt_name/.s/"$new_ext"}"    
cd spim_assembler
python3 ./asm2hex "../tests/$1"
cd ../src
make clean && make
./sim "../tests/$opt_name"


