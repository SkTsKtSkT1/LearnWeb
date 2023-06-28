#!/bin/sh

set -x

cd build \
	&& make clean\
	&& cmake ..\
	&& make -j4\
	&& ../bin/test_utils
