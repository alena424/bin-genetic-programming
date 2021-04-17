/**
 * BIN - cellular automata - majority problem
 * 
 * @author Alena Tesařová (xtesar36)
 * @date 16.4.2021
 **/

#include <iostream>
#include <tuple>
#include <math.h>
#include "ca_sim.hpp"

using namespace std;

const int neighborhood = 1;                 // number of cells from left and right of the main cell that will be computed with current cell
const int cellular_length = 9;              // length of 1D cellular automata
const int number_of_configurations = 10000; // number of random configuration that will be used to somlue fitness
const int generation = 10000;               // number of generations
const int steps = 20;                       // number of steps for cullular automata to count fitness
const int lambda = 5;
const int mutations = 2;                               // number of mutation of random position in rules
const int rules_length = pow(2, 2 * neighborhood + 1); // number of rules (combination of zeros and ones in neighborhood))

/**
 * @brief Print rules in JSON format to web browser application
 * @param out output
 * @param rules rules to be printed in data field of json
 **/
void printRules(ostream &out, int *rules, int length)
{
    out << "JSON data: " << endl
        << "{"
        << "\"neighborhood\":" << neighborhood << ","
        << "\"cellular_length\":" << cellular_length << ",";
    out << "\""
        << "data"
        << "\":[";

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
    for (int i = 0; i < cellular_length; i++)
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
 * @return [fitness, best step] - fitness is in range [0-length_config_arr * number_of_configurations]
 **/
std::tuple<int, int> calculate_fitness(CAsim &sim, int *candidateRule, int steps)
{
    // set configuration array
    int configuration[cellular_length];
    int fitnessMaxFinal = 0; // max fitness counter (sum of all fitness from all configurations)
    int best_step = 0;       // the step in which we found the best solution
    for (int config = 0; config < number_of_configurations; config++)
    {
        // random init configuration
        for (int j = 0; j < cellular_length; j++)
            configuration[j] = rand() % 2;

        // cout << "config: ";
        // printRow(configuration, cellular_length);
        // cout << "Rules: ";
        // printRow(candidateRule, 8);

        int expectedValue = computeMajorValue(configuration); // expected value will be one if there is more ones, 0 otherwise
        sim.set_init(candidateRule, configuration);           // init simulator with init configuration and candidte rule
        sim.run_sim(steps);                                   // run simulator (fce sim.run_sim)

        int fitness = 0;
        int fitnessMax = 0;
        int *data = nullptr;         // pointer to computed data
        int *previousData = nullptr; // pointer to computed data
        bool stable = false;
        for (int j = 1; j < steps; j++)
        {
            if (isSameArray(previousData, sim.get_states(j), cellular_length))
            {
                stable = true;
            }
            data = sim.get_states(j);
            fitness = 0;
            for (int i = 0; i < cellular_length; i++)
            {
                if (data[i] == expectedValue)
                {
                    fitness++; // increment fitness if values match
                }
            }
            if (fitness > fitnessMax) // save the best fitness score
            {
                fitnessMax = fitness;
                best_step = j;
            }
            previousData = data;
        }
        // we want only stable rules
        if (stable)
        {
            fitnessMaxFinal += fitnessMax;
        }
    }
    // return two values - fitness and the best step
    return std::make_tuple(fitnessMaxFinal, best_step);
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    int rules[rules_length];
    int offsprings[lambda][rules_length];

    int parent_fit;
    int offsprings_fits[lambda];

    // create random rule
    for (int j = 0; j < rules_length; j++)
        rules[j] = rand() % 2;

    // cout << "Rules: ";
    // printRow(rules, rules_length);

    // init new simulator
    CAsim sim(cellular_length, neighborhood, steps);

    int br; // integer pro ukladani kroku, kdy se naslo nejlepsi reseni; casto jej nepotrebujeme
    std::tie(parent_fit, br) = calculate_fitness(sim, rules, steps);

    cout << "Initial:  fitness " << parent_fit << endl;

    int gen = 0;
    for (gen = 0; gen < generation; gen++)
    {
        if (parent_fit == cellular_length * number_of_configurations)
        {
            cout << "Solution found!" << endl;
            break;
        }

        int next_parent = -1;

        for (int l = 0; l < lambda; l++)
        {
            // copy of parent
            memcpy(offsprings[l], rules, sizeof(int) * rules_length);

            // provedeni mutaci
            for (int m = 0; m < mutations; m++)
            {
                offsprings[l][rand() % (rules_length)] ^= 0x01; // nahodne zmutujeme nektera cisla
            }

            std::tie(offsprings_fits[l], br) = calculate_fitness(sim, offsprings[l], steps); // provedeni simulace

            if (offsprings_fits[l] >= parent_fit && (next_parent < 0 || offsprings_fits[l] > offsprings_fits[next_parent]))
            {
                // lepsi nebo stejna fitness jako rodic a nez nejlepsi z generace
                next_parent = l;
            }
        }

        // kopirovani nejlepsiho
        if (next_parent >= 0)
        {
            assert((offsprings_fits[next_parent] >= parent_fit)); // musi dojit ke zlepseni fitness nebo aspon stejna (neutralni mutace)
            if (offsprings_fits[next_parent] > parent_fit)        // doslo
                cout << "Gen # " << gen << " fitness " << offsprings_fits[next_parent] << endl;

            memcpy(rules, offsprings[next_parent], sizeof(int) * rules_length);
#if 1
            // vyuzijeme fitness, kterou mame vypocitanou
            parent_fit = offsprings_fits[next_parent];
#else
            // pro jistotu znovu vypocitame fitness (pouzivano pro ladeni)
            std::tie(parent_fit, br) = calculate_fitness(sim, parent, steps, expectedValue);
            assert(parent_fit == offsprings_fits[next_parent]);
#endif
        }
    }

    cout << "Search ended" << endl;
    cout << "Best fitness " << parent_fit << " in " << gen << " generations " << endl;

    printRules(cout, rules, rules_length);
    cout << "Rules: ";
    printRow(rules, rules_length);
    int bf;
    std::tie(bf, br) = calculate_fitness(sim, rules, steps);
    cout << "Best fitness " << bf << "/" << (cellular_length * number_of_configurations) << " in step " << br << endl;
}
