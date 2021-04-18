#define NEIGHBORHOOD 2 // number of cells from left and right of the main cell that will be computed with current cell
#define NUM_CONFIG 1   // number of random configuration that will be used to somlue fitness, space 2^cellular_length
#define GENERATIONS 2  // number of generations
#define POPSIZE 4      // size of population
#define STEPS 20       // number of steps for cullular automata to count fitness
#define GLENGTH 11     // length of input configuration
#define MUTAGENES 2    // number of mutations
#define PCROSS 80      // probability of crossover
#define PMUT 1         // probability of mutation

#define TOUR 4 // number of individuals in turnament selection

typedef unsigned int UINT;

const unsigned int unit = 100; // unit
UINT max_fitness = GLENGTH * NUM_CONFIG;
const int rules_length = pow(2, 2 * NEIGHBORHOOD + 1); // number of rules (combination of zeros and ones in neighborhood))
int statistics[2];                                     // count major black and white for statistics

typedef struct
{
    int chromosome[rules_length]; // chromosone content = rules in our case
    int best_step;                // the step where the fitness was calculated
    UINT fitness;                 // fitness of individual
    bool evaluate;                // if it s necessary to evaluate fitness again
} GA_chromosome;