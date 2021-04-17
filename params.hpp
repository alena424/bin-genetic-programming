#define NEIGHBORHOOD 1 // number of cells from left and right of the main cell that will be computed with current cell
#define NUM_CONFIG 100 // number of random configuration that will be used to somlue fitness, space 2^cellular_length
#define GENERATIONS 30 // number of generations
#define POPSIZE 1000   // size of population
#define STEPS 20       // number of steps for cullular automata to count fitness
#define GLENGTH 11     // length of input configuration
#define MUTAGENES 2    // number of mutations
#define PCROSS 89      // probability of crossover
#define PMUT 2         // probability of mutation

#define TOUR 4 // number of individuals in turnament selection

typedef unsigned int UINT;

const int rules_length = pow(2, 2 * NEIGHBORHOOD + 1); // number of rules (combination of zeros and ones in neighborhood))
int statistics[2];                                     // count major black and white for statistics

typedef struct
{
    int chromosome[rules_length]; // chromosone content = rules in our case
    UINT fitness;                 // fitness of individual
    bool evaluate;                // if it s necessary to evaluate fitness again
} GA_chromosome;