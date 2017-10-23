#!/bin/bash

source functional-tests/coloring.sh

build_dir_prefix="./build"
build_dir=$1

if [ "$1" == "release" ];
then
    build_dir=$build_dir_prefix"/release"
fi
if [ "$1" == "debug" ] || [ "$1" == "" ];
then
    build_dir=$build_dir_prefix"/debug"
fi

echo "${cyan}${bold}Building...${normal}"
./build.sh $1
echo "${cyan}${bold}Running unit tests...${normal}"
(
	cd $build_dir
	make run-unit-tests
)
echo "${cyan}${bold}Running functional tests...${normal}"
(
	cd $build_dir
	make run-functional-tests
)
echo "${cyan}${bold}Test execution done, see information above${normal}"
