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
POPSIZE=1000

echo "Epoche;Neighborhood;Total configurations;Configuration length;Steps;Total generation;Population size;Fitness;Max fitness;Success;Best step;Best generation;Rules" > $statsFileName;

for (( i=0;i<RUNS;i++ ))
do
    echo -n "$i;">> $statsFileName;
    ./ca_major -n $NEIGHBORHOOD -c $NUM_CONFIG -l $CONFIG_LENGTH -s $STEPS -g $GENERATIONS -p $POPSIZE;
done

NEIGHBORHOOD=$((NEIGHBORHOOD+1))

for (( i=0;i<RUNS;i++ ))
do
    echo -n "$i;">> $statsFileName;
    ./ca_major -n $NEIGHBORHOOD -c $NUM_CONFIG -l $CONFIG_LENGTH -s $STEPS -g $GENERATIONS -p $POPSIZE;
done

NEIGHBORHOOD=$((NEIGHBORHOOD+1))

for (( i=0;i<RUNS;i++ ))
do
    echo -n "$i;">> $statsFileName;
    ./ca_major -n $NEIGHBORHOOD -c $NUM_CONFIG -l $CONFIG_LENGTH -s $STEPS -g $GENERATIONS -p $POPSIZE;
done