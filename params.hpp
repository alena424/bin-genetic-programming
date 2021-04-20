#define MUTAGENES 2 // number of mutations
#define PCROSS 80   // probability of crossover
#define PMUT 10     // probability of mutation
#define TOUR 4      // number of individuals in turnament selection

typedef unsigned int UINT;
UINT NEIGHBORHOOD = 2;   // number of cells from left and right of the main cell that will be computed with current cell
UINT NUM_CONFIG = 2000;  // number of random configuration that will be used to somlue fitness, space 2^cellular_length
UINT GENERATIONS = 30;   // number of generations
UINT POPSIZE = 1000;     // size of population
UINT STEPS = 20;         // number of steps for cullular automata to count fitness
UINT CONFIG_LENGTH = 11; // length of input configuration

const unsigned int unit = 100; // unit

UINT MAX_FITNESS;  //  max fitness that we can get = CONFIG_LENGTH * NUM_CONFIG
UINT RULES_LENGTH; // number of rules (combination of zeros and ones in neighborhood)) = pow(2, 2 * NEIGHBORHOOD + 1)
int statistics[2]; // count major black and white for statistics

typedef struct
{
    int *chromosome; // chromosone content = rules in our case
    int best_step;   // the step where the fitness was calculated
    UINT fitness;    // fitness of individual
    bool evaluate;   // if it s necessary to evaluate fitness again
} GA_chromosome;