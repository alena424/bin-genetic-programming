#define MUTAGENES 2 // number of mutations
#define PCROSS 80   // probability of crossover
#define PMUT 10     // probability of mutation
#define TOUR 4      // number of individuals in turnament selection

typedef unsigned int UINT;
UINT NEIGHBORHOOD = 1;   // number of cells from left and right of the main cell that will be computed with current cell
UINT NUM_CONFIG = 2;     // number of random configuration that will be used to somlue fitness, space 2^cellular_length
UINT GENERATIONS = 30;   // number of generations
UINT POPSIZE = 1000;     // size of population
UINT STEPS = 20;         // number of steps for cullular automata to count fitness
UINT CONFIG_LENGTH = 11; // length of input configuration

const unsigned int unit = 100; // unit

UINT MAX_FITNESS = CONFIG_LENGTH * NUM_CONFIG;
UINT RULES_LENGTH = pow(2, 2 * NEIGHBORHOOD + 1); // number of rules (combination of zeros and ones in neighborhood))
int statistics[2];                                // count major black and white for statistics

// typedef struct
// {
//     int *chromosome; // chromosone content = rules in our case
//     int best_step;   // the step where the fitness was calculated
//     UINT fitness;    // fitness of individual
//     bool evaluate;   // if it s necessary to evaluate fitness again
// } GA_chromosome;

class GA_chromosome
{
public:
    int *chromosome; // chromosone content = rules in our case
    int best_step;   // the step where the fitness was calculated
    UINT fitness;    // fitness of individual
    bool evaluate;   // if it s necessary to evaluate fitness again
    GA_chromosome(UINT chromosome_length)
    {
        chromosome = new int[chromosome_length];
        best_step = 0;
        fitness = 0;

        for (UINT i = 0; i < chromosome_length; i++)
        {
            chromosome[i] = urandom(0, 1);
        }
    };
    GA_chromosome(){

    };
    ~GA_chromosome()
    {
        delete[] chromosome;
    };
    /**
 * @brief Generate random number in range [low, high]
 * @author (c) MICHAL BIDLO, 2011 (VZOROVA IMPLEMENTACE JEDNODUCHEHO GENETICKEHO ALGORITMU) 
 **/
    UINT urandom(int low, int high)
    {
        return rand() % (high - low + 1) + low;
    }
};