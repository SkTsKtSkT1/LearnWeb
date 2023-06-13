#!/bin/sh

set -x

cd build \
	&& make clean\
	&& cmake ..\
	&& make -j2\
	&& ../bin/test_config
