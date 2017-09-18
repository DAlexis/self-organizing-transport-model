#!/bin/bash

config_file=$1
directory=`dirname "$config_file"`

echo $config_file
echo $directory

for filename in "$directory"/*.csv; do
    echo "Processing $filename..."
    ./plot-field.sh "$filename" "$config_file" $2 $3 $4 $5 $6 $7
done
