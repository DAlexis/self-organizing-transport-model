#!/bin/bash

set -e

if [ "$#" -lt 2 ]; then
    echo "Please specify input file and config file"
    exit 1
fi

binary="../utilities/field-calculator/field-calc"

if [ -f ./field-calc ]
then
    binary="./field-calc"
fi

if [ ! -f $binary ]
then
    echo "field-calc binary not found!"
    exit 1
fi


input=$1
config=$2

output=${input/\./-}-field
field=`cat $config | awk '{if ($1 == "field") print $NF}'`

cat $config | awk '{if ($1 == "field") print $NF}'
$binary --input=$input --output=$output --ez=$field $3
