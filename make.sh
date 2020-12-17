#!/bin/bash
aclocal
autoheader 
autoconf
automake --add-missing
automake
./configure
make