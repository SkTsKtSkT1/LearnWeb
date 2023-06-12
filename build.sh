#!/bin/sh

set -x

cd build \
	&& cmake ..\
	&& make \
	&& ../bin/tests
