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

const int rows = 8, cols = 8;
const int neighborhood = 1; // number of cells from left and right of the main cell that we will compute
const int cellular_length = 9;

// ocekavany vstup
// const int expected[rows * cols] = {
//     0, 0, 0, 0, 0, 0, 0, 0,
//     0, 0, 0, 0, 0, 0, 0, 0,
//     0, 0, 0, 0, 0, 0, 0, 0,
//     0, 0, 0, 1, 1, 1, 0, 0,
//     0, 0, 0, 0, 0, 0, 0, 0,
//     0, 0, 0, 0, 0, 0, 0, 0,
//     0, 0, 0, 0, 0, 0, 0, 0,
//     0, 0, 0, 0, 0, 0, 0, 0};

void print_configuration(ostream &out, int *dataset)
{

    int *ds = dataset;
    out << rows << " " << cols << endl;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            out << *(dataset++) << " ";
        }
        out << endl;
    }

    // JSON format too
    dataset = ds;

    out << "JSON data: " << endl
        << "{";
    for (int r = 0; r < rows; r++)
    {
        if (r)
            out << ",";
        out << "\"" << r << "\":[";

        int wasout = 0;
        for (int c = 0; c < cols; c++)
        {
            if (*(dataset++))
            {
                if (wasout)
                    out << ",";
                out << c;
                wasout = 1;
            }
        }
        out << "]";
    }

    out << "}" << endl
        << endl;
    ;
}

/** 
 * Vypocet fitness pro kandidatni reseni
 * sim - simulator (nutne nastavit a spustit)
 * candidate - 1D pole kandidatniho reseni
 * steps - pocet kroku, po ktery se bude provadet simulace
 * vraci dvojici integeru fitness a best_step
 *    fitness je v rozsahu 0 az rows * cols, kdy rows * cols je nejlepsi (funkcni reseni)
 *    best_step je cislo kroku, kdy se dosahlo nejvyssi hodnoty fitness, je 0 az steps
 */
std::tuple<int, int> calculate_fitness(CAsim &sim, int *candidateRule, int steps, int expectedValue)
{
    // nastavit candidate jako pocatecni stav v simulatori (fce sim.set_init)
    sim.set_init(candidateRule);

    // spustit simulator (fce sim.run_sim)
    sim.run_sim(steps);

    int fitness = 0;
    int fitnessMax = 0;
    int best_step = 0; // v kterem kroku simulace byla nejlepsi fintess nalezena

    // projit vsechny korky, pomoci funkce sim.get_state ziskat stav
    int *data = nullptr;
    for (int j = 1; j < steps; j++)
    {
        data = sim.get_states(j);
        fitness = 0;
        for (int i = 0; i < cellular_length; i++)
        {
            if (data[i] == expectedValue)
            {
                fitness++;
            }
        }
        if (fitness > fitnessMax)
        {
            fitnessMax = fitness;
            best_step = j;
        }
    }
    // navrat dvou hodnot
    return std::make_tuple(fitnessMax, best_step);
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

int main(int argc, char **argv)
{
    const int generation = 200;
    const int steps = 20;
    const int lambda = 5;
    const int mutations = 0;

    const int rules_length = pow(2, 2 * neighborhood + 1); // number of rules (compination of zeros and ones in neighborhood))
    cout << rules_length;
    srand(time(NULL));

    // geneticky algoritmus mu+lambda: vezme se mi nejlepsi,
    // z toho se vygeneruje dalsich lambda konfiguraci. Vybere
    // se zase mu nejlepsich

    int parent[cellular_length];
    int rules[rules_length];
    int offsprings[lambda][rules_length];

    int parent_fit;
    int offsprings_fits[lambda];

    // vytvoreni nahodne prechodove funkce
    for (int j = 0; j < rules_length; j++)
        rules[j] = rand() % 2;

    for (int j = 0; j < cellular_length; j++)
        parent[j] = rand() % 2;

    cout << "config: ";
    printRow(parent, cellular_length);
    cout << "Rules: ";
    printRow(rules, rules_length);

    int expectedValue = computeMajorValue(parent);

    cout << "Expected is: " << expectedValue << endl;

    // novy simulator 4x4
    CAsim sim(cellular_length, rules_length, parent, neighborhood, steps);

    int br; // integer pro ukladani kroku, kdy se naslo nejlepsi reseni; casto jej nepotrebujeme
    std::tie(parent_fit, br) = calculate_fitness(sim, rules, steps, expectedValue);

    cout << "Initial:  fitness " << parent_fit << endl;
    // return 0;

    int gen = 0;
    for (gen = 0; gen < generation; gen++)
    {
        if (parent_fit == cellular_length)
        {
            cout << "solution found!" << endl;
            break;
        }

        int next_parent = -1;

        for (int l = 0; l < lambda; l++)
        {
            // kopie rodice
            memcpy(offsprings[l], rules, sizeof(int) * rules_length);

            // provedeni mutaci
            for (int m = 0; m < mutations; m++)
            {
                offsprings[l][rand() % (rules_length)] ^= 0x01; // nahodne zmutujeme nektera cisla
            }

            std::tie(offsprings_fits[l], br) = calculate_fitness(sim, offsprings[l], steps, expectedValue); // provedeni simulace

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

    // print_configuration(cout, parent);
    cout << "config: ";
    printRow(parent, cellular_length);
    cout << "Rules: ";
    printRow(rules, rules_length);
    int bf;
    std::tie(bf, br) = calculate_fitness(sim, rules, steps, expectedValue);
    cout << "Best fitness " << bf << "/" << (cellular_length) << " in step " << br << endl;
}
