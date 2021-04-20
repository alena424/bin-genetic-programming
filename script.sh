#!/bin/bash
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

echo "Epoche,Neighborhood,Total congigurations,Configuration length,Steps,Total generation,Population size,Fitness,Max fitness,Success,Best step, Best generation" > $statsFileName;

for (( i=0;i<RUNS;i++ ))
do
    echo -n "$i,$NEIGHBORHOOD,$NUM_CONFIG,$CONFIG_LENGTH,$STEPS,$GENERATIONS,$POPSIZE_START">> $statsFileName;
    ./ca_major -n $NEIGHBORHOOD
done