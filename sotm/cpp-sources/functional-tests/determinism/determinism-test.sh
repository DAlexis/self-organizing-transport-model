#!/bin/bash

my_dir=$(dirname $0)
source "../coloring.sh"
config="det-test.conf"

echo "Running determinism test for $config..."

rm -rf run1 run2
mkdir -p run1 run2
cp ../../lightning-modeller/lightmod run1
cp ../../lightning-modeller/lightmod run2
cp $config run1
cp $config run2

echo -n "${yellow}Running sample 1${normal}... "
(
    cd run1
    ./lightmod --no-gui --ini-load=$config 1>/dev/null
)
echo "${yellow}${bold}done${normal}"

echo -n "${yellow}Running sample 2${normal}..."
(
    cd run2
    ./lightmod --no-gui --ini-load=$config 1>/dev/null
)
echo "${yellow}${bold}done${normal}"

files_count1=`ls run1 | wc -l`
files_count2=`ls run2 | wc -l`

if [ "$files_count1" -ne "$files_count2" ]; then
    echo "${red}${bold}FAILED:${normal} generated files count is not equal"
    exit 0
fi

last_csv1=`ls run1/*.csv | tail -1`
last_csv2=`ls run2/*.csv | tail -1`

different=0
cmp --silent "$last_csv1" "$last_csv2" > /dev/null || different=1

if [ "$different" == "1" ]; then
    echo "${red}${bold}FAILED:${normal} last csv files contains different data, files are $last_csv1 vs $last_csv2"
    exit 0
fi

echo "${green}${bold}OK${normal}"
