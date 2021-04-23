#!/bin/bash
echo "Lets start..."

RUNS=10
statsFileName="statistics.txt"

# Configuration no. 1
NEIGHBORHOOD=1
NUM_CONFIG=2000
CONFIG_LENGTH=11
STEPS=20
GENERATIONS=20
POPSIZE=1000

echo "Epoche;Neighborhood;Total configurations;Configuration length;Steps;Total generation;Population size;Fitness;Max fitness;Success;Best step;Best generation;Rules" > $statsFileName;

for (( i=0;i<RUNS;i++ ))
do
    echo -n "$i;">> $statsFileName;
    ./ca_major;
done