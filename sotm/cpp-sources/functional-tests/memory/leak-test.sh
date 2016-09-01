#!/bin/bash

# check if stdout is a terminal...
if test -t 1; then

    # see if it supports colors...
    ncolors=$(tput colors)

    if test -n "$ncolors" && test $ncolors -ge 8; then
        bold="$(tput bold)"
        underline="$(tput smul)"
        standout="$(tput smso)"
        normal="$(tput sgr0)"
        black="$(tput setaf 0)"
        red="$(tput setaf 1)"
        green="$(tput setaf 2)"
        yellow="$(tput setaf 3)"
        blue="$(tput setaf 4)"
        magenta="$(tput setaf 5)"
        cyan="$(tput setaf 6)"
        white="$(tput setaf 7)"
    fi
fi


echo "Running memory-leak test for $1..."

reportFile=$(echo "$1" | awk -F\/ '{ print $NF }').txt
valgrind $1 2> $reportFile
results=$(cat "$reportFile" | grep "lost:")
lost=($(echo "$results" | cut -d: -f2 | cut -d' ' -f2))

definitely=${lost[0]}
indirectly=${lost[1]}
possibly=${lost[2]}

if [[ $definitely -ne 0 || $indirectly -ne 0 ]]; then
	echo "${bold}${red}MEMORY LEAK${normal} on $1. Valgrind details: $reportFile"
	exit 1
fi

if [[ $possibly -ne 0 ]]; then
	echo "${red}POSSIBLY LEAK${normal} on $1. Valgrind details: $reportFile"
	exit 0
fi

echo "${green}${bold}OK${normal}"