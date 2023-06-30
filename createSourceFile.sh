#!/bin/bash
set -x
cd skt
mkdir "$1"
cd "$1"
touch "$1.cpp"
touch "$1.h"

filename=$1
filename=$(echo $filename | tr '[a-z]' '[A-Z]')
#echo $filename

echo "#ifndef LEARNWEB_${filename}_H"  >> "$1.h"
echo "#define LEARNWEB_${filename}_H" >> "$1.h"
echo "\n" >> "$1.h"
echo "#endif" >> "$1.h"
