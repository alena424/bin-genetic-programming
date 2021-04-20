/**
 * BIN - cellular automata - majority problem
 * 
 * @author Alena Tesařová (xtesar36)
 * @date 16.4.2021
 **/

#include <iostream>
#include <fstream>
#include <tuple>
#include <math.h>
#include <unistd.h>
#include <ctype.h>
#include "params.hpp"
#include "utils.cpp"
#include "ca_sim.hpp"

using namespace std;

UINT best_fitness_global;        // fitness of the best one
int best_fitness_generation = 0; // generation with the best fitness (for global analysis)
bool stats_mode = false;         // statistics mode (for algorithm evaluation)

/**
 * @brief Compute fitness for candidate solution
 * @param sim simulator (must be init and run)
 * @param candidateRule 1D array with proposed ruke
 * @param steps number of steps for which we will run the simulation
 * @return [fitness, best step] - fitness is in range [0-length_config_arr * NUM_CONFIG]
 **/
tuple<int, int> calculate_fitness(CAsim &sim, int *candidateRule)
{
    // set configuration array
    int *configuration = new int[CONFIG_LENGTH];
    int fitness_max_final = 0; // max fitness counter (sum of all fitness from all configurations)
    int best_step_global = 0;  // average of the best steps

    for (UINT config = 0; config < NUM_CONFIG; config++)
    {
        // random init configuration
        for (UINT j = 0; j < CONFIG_LENGTH; j++)
            configuration[j] = rand() % 2;

        int expectedValue = computeMajorValue(configuration); // expected value will be one if there is more ones, 0 otherwise
        statistics[expectedValue]++;                          // add to statistics to know the distribution of data
        // cout << "expected: " << expectedValue << endl;
        sim.set_init(candidateRule, configuration); // init simulator with init configuration and candidte rule
        sim.run_sim(STEPS);                         // run simulator (fce sim.run_sim)
        // printRules(cout, candidateRule, RULES_LENGTH);

        UINT fitness = 0;
        UINT fitness_max = 0;
        int *data = nullptr;         // pointer to computed data
        int *previousData = nullptr; // pointer to computed data
        int *pomData = nullptr;      // pointer to computed data
        bool stable = false;
        int best_step = 0; // the step in which we found the best solution
        for (UINT j = 1; j < STEPS; j++)
        {
            pomData = sim.get_states(j);
            if (previousData && isSameArray(previousData, pomData, CONFIG_LENGTH))
            {
                stable = true; // we want the result to be stable and not recursive
                break;
            }
            data = pomData;
            fitness = 0;
            for (UINT i = 0; i < CONFIG_LENGTH; i++)
            {
                if (data[i] == expectedValue)
                {
                    fitness++; // increment fitness if values match
                }
            }
            if (fitness > fitness_max) // save the best fitness score
            {
                fitness_max = fitness;
                best_step = j;
            }
            previousData = data;
        }
        // we want only stable rules and the correct ones
        if (stable && fitness_max == CONFIG_LENGTH)
        {
            fitness_max_final += fitness_max;
        }
        best_step_global += best_step;
    }
    delete[] configuration;
    return make_tuple(fitness_max_final, best_step_global / NUM_CONFIG);
}

int main(int argc, char **argv)
{
    parseArguments(argc, argv);
    // cout << "NEIGHBORHOOD: " << NEIGHBORHOOD << ", NUM_CONFIG: " << NUM_CONFIG << ", GENERATIONS: " << GENERATIONS << ", POPSIZE: " << POPSIZE << ", STEPS: " << STEPS << ", CONFIG_LENGTH: " << CONFIG_LENGTH << endl;

    ofstream statsFile;
    statsFile.open("statistics.txt", ios_base::app);

    // population - only odd size
    GA_chromosome *best_chromosome = new GA_chromosome(RULES_LENGTH, true); // the best solution
    // GA_chromosome *population = new GA_chromosome[POPSIZE];
    GA_chromosome **population = new GA_chromosome *[POPSIZE];
    GA_chromosome **next_population = new GA_chromosome *[POPSIZE];
    srand(time(NULL));
    best_fitness_global = 0;

    CAsim sim(CONFIG_LENGTH, NEIGHBORHOOD, STEPS); // init new simulator

    GA_chromosome *ind1_new = new GA_chromosome(RULES_LENGTH, true);
    GA_chromosome *ind2_new = new GA_chromosome(RULES_LENGTH, true);
    UINT i1;
    // initializace population
    for (UINT i = 0; i < POPSIZE; i++)
    {
        population[i] = new GA_chromosome(RULES_LENGTH, false);
        cout << "init> ";
        printRules(cout, population[i]->chromosome, RULES_LENGTH);

        next_population[i] = new GA_chromosome(RULES_LENGTH, false);
        population[i]->evaluate = 1;
    }

    for (UINT gen = 0; gen < GENERATIONS; gen++)
    {
        // cout << "Running generation number " << gen << endl;
        // evaluate fitness
        for (UINT i = 0; i < POPSIZE; i++)
        {
            if (population[i]->evaluate)
            {
                std::tie(population[i]->fitness, population[i]->best_step) = calculate_fitness(sim, population[i]->chromosome);
                cout << "fitness" << population[i]->fitness << endl;
                if (population[i]->fitness >= best_chromosome->fitness)
                {
                    cout << "chamged!" << population[i]->fitness << ">=" << best_chromosome->fitness << endl;
                    best_chromosome = population[i];
                }
                population[i]->evaluate = 0;
            }
        }

        // elitizmus
        next_population[0] = best_chromosome; // so far the best found individual
        GA_chromosome *mutant = best_chromosome;
        mutator(mutant, unit);
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
                    ind1 = population[i1];
                else if (ind2 == NULL)
                    ind2 = population[i1];
                else if (population[i1]->fitness > ind1->fitness)
                    ind1 = population[i1];
                else if (population[i1]->fitness > ind2->fitness)
                    ind2 = population[i1];
            }

            // crossover
            if (urandom(0, unit) < PCROSS)
            {
                crossover(ind1, ind2, ind1_new, ind2_new);
                ind1_new->evaluate = 1;
                ind2_new->evaluate = 1;
            }
            else //no cross over
            {
                ind1_new = ind1;
                ind2_new = ind2;
            }
            //mutation
            if (mutator(ind1_new, PMUT))
                ind1_new->evaluate = 1;
            if (mutator(ind2_new, PMUT))
                ind2_new->evaluate = 1;

            next_population[i] = ind1_new;
            next_population[i + 1] = ind2_new;
        }

        if (best_chromosome->fitness > best_fitness_global)
        {
            best_fitness_global = best_chromosome->fitness;
            best_fitness_generation = gen;
            cout
                << "Gen # " << gen << " fitness " << best_fitness_global << endl;
            printRules(cout, best_chromosome->chromosome, RULES_LENGTH);
        }
        if (best_fitness_global == MAX_FITNESS)
        {
            printf("Solution found; generation=%d\n", gen);
            break;
        }

        // place new generaton to new population
        swapPointers(&population, &next_population);
    }

    cout << "Search ended" << endl;
    cout << "Best fitness " << best_fitness_global << " in " << best_fitness_generation << " generations." << endl;

    printRules(cout, best_chromosome->chromosome, RULES_LENGTH);
    cout << "Rules: ";
    printRow(best_chromosome->chromosome, RULES_LENGTH);
    // int br = calculate_fitness(sim, rules);
    UINT bf = best_chromosome->fitness;
    UINT bstep = best_chromosome->best_step;
    save_statistics(statsFile, best_chromosome, best_fitness_generation);
    printf("Best fitness %d/%d (%.2f%%) in step (average) %d.\n", bf, MAX_FITNESS, ((float)bf / (float)MAX_FITNESS) * 100, bstep);
    printf("Statistics in training: major black: %d, major white: %d\n", statistics[1], statistics[0]);

    delete[] population;
    delete[] next_population;

    statsFile.close();
}
