#!/bin/bash

my_dir=$(dirname $0)
source "../coloring.sh"

echo -n "Running memory-leak test for $1... "

reportFile=$(echo "$1" | awk -F\/ '{ print $NF }').txt
rm -f $reportFile
valgrind $1 2> $reportFile || {
	echo "${bold}${red}Valgring tool not installed,${normal} cannot continue"
	exit 0
}
results=$(cat "$reportFile" | grep "lost:")
lost=($(echo "$results" | cut -d: -f2 | cut -d' ' -f2))

definitely=${lost[0]}
indirectly=${lost[1]}
possibly=${lost[2]}

if [[ $definitely -ne 0 || $indirectly -ne 0 ]]; then
	echo "${bold}${red}MEMORY LEAK${normal} on $1. Valgrind details: $reportFile"
	exit 0
fi

if [[ $possibly -ne 0 ]]; then
	echo "${red}POSSIBLY LEAK${normal} on $1. Valgrind details: $reportFile"
	exit 0
fi

echo "${green}${bold}OK${normal}"
