/**
 * BIN - cellular automata - majority problem
 * 
 * @author Alena Tesařová (xtesar36)
 * @date 16.4.2021
 **/

#include <iostream>
#include <tuple>
#include <math.h>
#include "params.hpp"
#include "ca_sim.hpp"

using namespace std;

// population - only odd size
GA_chromosome population[POPSIZE];
GA_chromosome next_population[POPSIZE];
const unsigned int unit = 100; // unit
GA_chromosome best_population; // the best solution
UINT best_ever;                // fitness of the best one
/**
 * @brief Print rules in JSON format to web browser application
 * @param out output
 * @param rules rules to be printed in data field of json
 **/
void printRules(ostream &out, int *rules, int length)
{
    out << "JSON data: " << endl
        << "{"
        << "\"neighborhood\":" << NEIGHBORHOOD << ", "
        << "\"cellular_length\":" << GLENGTH << ", "
        << "\"steps\":" << STEPS << ", ";
    out << "\""
        << "data"
        << "\": [";

    for (int i = 0; i < length; i++)
    {
        out << rules[i];
        if (i != length - 1)
        {
            cout << ", ";
        }
    }
    out << "]}" << endl;
}

/**
 * @brief Print one row of specific length
 * @param arr array that we want to print
 * @param lengthArr the length of array
 **/
void printRow(int *arr, int lengthArr)
{
    cout << "[";
    for (int i = 0; i < lengthArr; i++)
    {
        cout << arr[i];
        if (i != lengthArr - 1)
        {
            cout << ", ";
        }
    }
    cout << "]" << endl;
}
/**
 * @brief Compute major value in config array
 * @return Return either one or zero
 **/
int computeMajorValue(int *configArr)
{
    int counterOnes = 0;
    int counterZeros = 0;
    for (int i = 0; i < GLENGTH; i++)
    {
        if (configArr[i])
        {
            counterOnes++;
        }
        else
        {
            counterZeros++;
        }
    }
    if (counterOnes > counterZeros)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief Compare 2 arrays if they are the same
 * @param in first array
 * @param out second array
 * @param length the length of arrays (must be the same size)
 **/
bool isSameArray(int *in, int *out, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (in[i] != out[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Compute fitness for candidate solution
 * @param sim simulator (must be init and run)
 * @param candidateRule 1D array with proposed ruke
 * @param steps number of steps for which we will run the simulation
 * @return [fitness, best step] - fitness is in range [0-length_config_arr * NUM_CONFIG]
 **/
int calculate_fitness(CAsim &sim, int *candidateRule)
{
    // set configuration array
    int configuration[GLENGTH];
    int fitnessMaxFinal = 0; // max fitness counter (sum of all fitness from all configurations)
    // int best_step = 0;       // the step in which we found the best solution

    for (int config = 0; config < NUM_CONFIG; config++)
    {
        // random init configuration
        for (int j = 0; j < GLENGTH; j++)
            configuration[j] = rand() % 2;

        // cout << "config: ";
        // printRow(configuration, GLENGTH);
        // cout << "Rules: ";
        // printRow(candidateRule, 8);

        int expectedValue = computeMajorValue(configuration); // expected value will be one if there is more ones, 0 otherwise
        statistics[expectedValue]++;
        // cout << "expected: " << expectedValue << endl;
        sim.set_init(candidateRule, configuration); // init simulator with init configuration and candidte rule
        sim.run_sim(STEPS);                         // run simulator (fce sim.run_sim)

        int fitness = 0;
        int fitnessMax = 0;
        int *data = nullptr;         // pointer to computed data
        int *previousData = nullptr; // pointer to computed data
        int *pomData = nullptr;      // pointer to computed data
        bool stable = false;
        for (int j = 1; j < STEPS; j++)
        {
            pomData = sim.get_states(j);
            if (previousData && isSameArray(previousData, pomData, GLENGTH))
            {
                stable = true; // we want the result to be stable and not recursive
            }
            data = pomData;
            fitness = 0;
            for (int i = 0; i < GLENGTH; i++)
            {
                if (data[i] == expectedValue)
                {
                    fitness++; // increment fitness if values match
                }
            }
            if (fitness > fitnessMax) // save the best fitness score
            {
                fitnessMax = fitness;
                // best_step = j;
            }
            previousData = data;
        }
        // we want only stable rules
        if (stable && fitnessMax == GLENGTH)
        {
            fitnessMaxFinal += fitnessMax;
        }
    }
    // return two values - fitness and the best step
    return fitnessMaxFinal;
    // return std::make_tuple(fitnessMaxFinal, best_step);
}

/**
 * @brief Generate random number in range [low, high]
 * @author (c) MICHAL BIDLO, 2011 (VZOROVA IMPLEMENTACE JEDNODUCHEHO GENETICKEHO ALGORITMU) 
 **/
UINT urandom(int low, int high)
{
    return rand() % (high - low + 1) + low;
}
/**
 * @brief Crossover 2 parents and get 2 crossed children
 * @author (c) MICHAL BIDLO, 2011 (VZOROVA IMPLEMENTACE JEDNODUCHEHO GENETICKEHO ALGORITMU) 
 **/
void crossover(GA_chromosome *parent1, GA_chromosome *parent2,
               GA_chromosome *child1, GA_chromosome *child2)
{
    // zde standardni jednobodove krizeni
    UINT cpoint = urandom(1, GLENGTH - 1);

    for (UINT i = 0; i < GLENGTH; i++)
    {
        if (i < cpoint)
        {
            child1->chromosome[i] = parent1->chromosome[i];
            child2->chromosome[i] = parent2->chromosome[i];
        }
        else
        {
            child1->chromosome[i] = parent2->chromosome[i];
            child2->chromosome[i] = parent1->chromosome[i];
        }
    }
}

/**
 * @brief Mutate
 * @author (c) MICHAL BIDLO, 2011 (VZOROVA IMPLEMENTACE JEDNODUCHEHO GENETICKEHO ALGORITMU) 
 **/
bool mutator(GA_chromosome *genome, UINT _pmut)
{
    if (urandom(0, unit) <= _pmut) // mutace s pravdepodobnosti _pmut
    {
        for (UINT i = 0; i < MUTAGENES; i++)
        {
            UINT g = urandom(0, GLENGTH - 1);
            genome->chromosome[g] = 1 - genome->chromosome[g]; // gene inversion
        }

        return 1; // probehla-li mutace, vratim true...
    }

    return 0; // ...jinak vracim false
}
/**
 * @brief Initialize array with zeros and ones
 * @author (c) MICHAL BIDLO, 2011 (VZOROVA IMPLEMENTACE JEDNODUCHEHO GENETICKEHO ALGORITMU) 
 **/
void initialize(GA_chromosome *genome)
{
    for (int i = 0; i < rules_length; i++)
        genome->chromosome[i] = urandom(0, 1);
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    // int rules[rules_length];
    // int offsprings[lambda][rules_length];
    best_ever = 0;

    // int parent_fit;
    // int offsprings_fits[lambda];

    // create random rule
    // for (int j = 0; j < rules_length; j++)
    // {
    //     rules[j] = rand() % 2;
    // }

    // cout << "Rules: ";
    // printRow(rules, rules_length);

    CAsim sim(GLENGTH, NEIGHBORHOOD, STEPS); // init new simulator

    // int br; // integer pro ukladani kroku, kdy se naslo nejlepsi reseni; casto jej nepotrebujeme
    // parent_fit = calculate_fitness(sim, rules);
    // return 0;

    // cout << "Initial:  fitness " << parent_fit << endl;
    UINT max_fitness = GLENGTH * NUM_CONFIG;
    GA_chromosome ind1_new, ind2_new;
    UINT i1;
    // initializace population
    for (int i = 0; i < POPSIZE; i++)
    {
        initialize(&population[i]);
        population[i].evaluate = 1;
    }

    int gen = 0;
    for (gen = 0; gen < GENERATIONS; gen++)
    {
        cout << "Running generation number " << gen << endl;
        // evaluate fitness
        for (int i = 0; i < POPSIZE; i++)
        {
            if (population[i].evaluate)
            {
                population[i].fitness = calculate_fitness(sim, population[i].chromosome);
                if (population[i].fitness >= best_population.fitness)
                    best_population = population[i];
                population[i].evaluate = 0;
            }
        }
        // elitizmus
        next_population[0] = best_population; // so far the best found individual
        GA_chromosome mutant = best_population;
        mutator(&mutant, unit);
        next_population[1] = mutant; // mutant of the best

        // Create new population
        for (UINT i = 2; i < POPSIZE; i += 2)
        {
            GA_chromosome *ind1 = NULL, *ind2 = NULL;
            // turnaments of individuals
            for (UINT t = 0; t < TOUR; t++)
            {
                i1 = urandom(0, POPSIZE - 1);
                if (ind1 == NULL)
                    ind1 = &population[i1];
                else if (ind2 == NULL)
                    ind2 = &population[i1];
                else if (population[i1].fitness > ind1->fitness)
                    ind1 = &population[i1];
                else if (population[i1].fitness > ind2->fitness)
                    ind2 = &population[i1];
            }

            // crossover
            if (urandom(0, unit) < PCROSS)
            {
                crossover(ind1, ind2, &ind1_new, &ind2_new);
                ind1_new.evaluate = 1;
                ind2_new.evaluate = 1;
            }
            else //no cross over
            {
                ind1_new = *ind1;
                ind2_new = *ind2;
            }
            // mutation
            if (mutator(&ind1_new, PMUT))
                ind1_new.evaluate = 1;
            if (mutator(&ind2_new, PMUT))
                ind2_new.evaluate = 1;
            // place new generaton to new population
            next_population[i] = ind1_new;
            next_population[i + 1] = ind2_new;
        }

        if (best_population.fitness > best_ever)
        {
            best_ever = best_population.fitness;
            cout << "Gen # " << gen << " fitness " << best_ever << endl;
            printRules(cout, best_population.chromosome, rules_length);
        }
        if (best_ever == max_fitness)
        {
            printf("Solution found; generation=%d\n", gen);
            // gprint(&best);
            break;
        }
    }

    cout << "Search ended" << endl;
    cout << "Best fitness " << best_ever << " in " << gen << " generations " << endl;

    printRules(cout, best_population.chromosome, rules_length);
    cout << "Rules: ";
    printRow(best_population.chromosome, rules_length);
    // int br = calculate_fitness(sim, rules);
    UINT bf = best_population.fitness;

    printf("Best fitness %d/%d (%.2f\%).\n", bf, max_fitness, ((float)bf / (float)max_fitness) * 100);
    printf("Statistics: major black: %d, major white: %d\n", statistics[1], statistics[0]);
    // cout
    //         << "Best fitness " << bf << "/" << (GLENGTH * NUM_CONFIG) << "(" < < < <
    //     ") in step " << br << endl;
}
