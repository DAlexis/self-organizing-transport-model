#!/bin/bash

echo Running memory-leak test for $1...
reportFile=valgrind-report-$1.txt
valgrind $1 > $reportFile
cat reportFile | grep lost:
echo done.