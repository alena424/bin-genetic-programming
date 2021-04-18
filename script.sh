#!/bin/sh
echo "Lets start..."

RUNS=1
statsFileName="statistics.txt"

# Configuration no. 1
NEIGHBORHOOD=1
NUM_CONFIG=2000
CONFIG_LENGTH=11
STEPS=20
GENERATIONS=20
POPSIZE_START=50

echo "Epoche, " >> statsFileName
for ((i=0;i<RUNS;i++)); do
    echo $i
done
