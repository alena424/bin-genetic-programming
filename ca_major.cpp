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
#include "utils.cpp"
#include "ca_sim.hpp"

using namespace std;

GA_chromosome best_population; // the best solution
UINT best_ever;                // fitness of the best one

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
    int *configuration = new int[GLENGTH];
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
        // we want only stable rules and the correct ones
        if (stable && fitnessMax == GLENGTH)
        {
            fitnessMaxFinal += fitnessMax;
        }
    }
    delete[] configuration;
    return fitnessMaxFinal;
    // return std::make_tuple(fitnessMaxFinal, best_step);
}

int main(int argc, char **argv)
{
    // population - only odd size
    GA_chromosome *population = new GA_chromosome[POPSIZE];
    GA_chromosome *next_population = new GA_chromosome[POPSIZE];
    srand(time(NULL));
    best_ever = 0;

    CAsim sim(GLENGTH, NEIGHBORHOOD, STEPS); // init new simulator

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
        // cout << "Running generation number " << gen << endl;
        // evaluate fitness
        for (int i = 0; i < POPSIZE; i++)
        {
            if (population[i].evaluate)
            {
                population[i].fitness = calculate_fitness(sim, population[i].chromosome);
                if (population[i].fitness >= best_population.fitness)
                {
                    best_population = population[i];
                }
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

            next_population[i] = ind1_new;
            next_population[i + 1] = ind2_new;
            //population = next_population;
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
            break;
        }
        // place new generaton to new population
        swapPointers(&population, &next_population);
    }

    cout << "Search ended" << endl;
    cout << "Best fitness " << best_ever << " in " << gen << " generations " << endl;

    printRules(cout, best_population.chromosome, rules_length);
    cout << "Rules: ";
    printRow(best_population.chromosome, rules_length);
    // int br = calculate_fitness(sim, rules);
    UINT bf = best_population.fitness;

    printf("Best fitness %d/%d (%.2f%%).\n", bf, max_fitness, ((float)bf / (float)max_fitness) * 100);
    printf("Statistics in training: major black: %d, major white: %d\n", statistics[1], statistics[0]);
    delete[] population;
    delete[] next_population;
}
