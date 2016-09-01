#!/bin/bash

echo "Running functional tests..."
echo "Memory testing..."
./memory/leak-test.sh ./memory/simple-graph/simple-graph-memory-leak-test
