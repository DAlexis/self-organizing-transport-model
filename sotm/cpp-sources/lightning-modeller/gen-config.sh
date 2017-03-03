#!/bin/bash

cd $1

cp config.hpp config.hpp.backup

commit=`git rev-parse HEAD`
build=`git rev-list --count HEAD`
build_time=`date +%Y-%m-%d_%H:%M:%S`

cat config.hpp.in | sed s/unknown-tag/$commit/ | sed s/unknown-build/$build/ | sed s/unknown-time/$build_time/ > config.hpp

#cp config.hpp.backup config.hpp


