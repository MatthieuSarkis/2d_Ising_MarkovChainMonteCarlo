#ifndef METROPOLIS_H
#define METROPOLIS_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <random>
#include <vector>
#include <cstdint>

using namespace std;

unsigned seed = static_cast<unsigned int>(chrono::steady_clock::now().time_since_epoch().count());
default_random_engine eng(seed);

/**
 Generates an L*L spin lattice.
 */
int8_t** create_lattice(int L)
{
    int8_t** lattice = 0;
    lattice = new int8_t*[L];
    for (int i = 0; i < L; i++)
    {
        lattice[i] = new int8_t[L];
    }
    
    return lattice;
}

/**
 Sets the spins of an L*L lattice to uniform +1 and -1 values.
 */
void set_spins(int8_t** lattice, int L)
{
    uniform_int_distribution<int> distr(0, 1);
    
    for (int i = 0; i < L; i++)
    {
        for (int j = 0; j < L; j++)
        {
            lattice[i][j] = 2 * distr(eng) - 1;
        }
    }
}

/**
 Computes the energy of a spin configuration.
 In the computation we consider only two of the 4 neighbours not to float count links.
 We use Periodic boundary conditions.
 */
int energy(int8_t** lattice, int L)
{
    int E = 0;
    for (int i = 0; i < L; i++)
    {
        for (int j = 0; j < L; j++)
        {
            E -= lattice[i][j] * (lattice[(i+1) % L][j] + lattice[i][(j+1) % L]);
        }
    }
    return E;
}

/**
 Performs one step of the metropolis algorithm. One step corresponds to the flipping of L*L spins.
 We keep track of the change in energy.
 */
int metropolis_one_step(int8_t** lattice, int L, int E, float T)
{
    int e = E;
    
    uniform_int_distribution<int> distr(0, L-1);
    uniform_real_distribution<float> distrR(0, 1);
    
    for (int k = 0; k < L * L; k++)
    {
        int i = distr(eng);
        int j = distr(eng);
        
        int total = 0;
        int nbr[4][2] = {{(i-1+L) % L, j}, {(i+1) % L, j}, {i, (j-1+L) % L}, {i, (j+1) % L}};
        
        for (int l=0; l<4; l++) total += lattice[nbr[l][0]][nbr[l][1]];
        
        int dE = 2 * lattice[i][j] * total;
        
        if (dE <= 0 || exp(-dE / T) > distrR(eng))
        {
            e += dE;
            lattice[i][j] *= -1;
        }
    }
    
    return e;
}

/**
 Performs n_steps steps of the metropolis algorithm.
 We keep track of the enery to plot it and monitor whether the system has converged or not.
 */
void metropolis(int8_t** lattice, int L, float T, int n_steps, vector<int>& E_list)
{
    int e = energy(lattice, L);
    E_list.push_back(e);
    
    for (int i = 0; i < n_steps; i++)
    {
        e = metropolis_one_step(lattice, L, e, T);
        E_list.push_back(e);
    }
}

/*
void metropolis2(int8_t** lattice, int L, float T, float epsilon, int period, vector<int>& E_list, vector<int>& E_ma)
{
    int e = energy(lattice, L);
    E_list.push_back(e);
    float sum = 0;   // to compute the moving average
    int count = 1;
    float delta_E = 5 * epsilon;
    
    while (delta_E > epsilon)
    {
        sum += e;
        if (count >= period)
        {
            E_ma.push_back((float)sum / period);
            sum -= E_list[count - period];
        }
        e = metropolis_one_step(lattice, L, e, T);
        if (count > period) delta_E = abs(E_ma[count-1] - E_ma[count - 2]) / E_ma[count - 2];
        count++;
    }
    
    cout << "Number of iterations: " << count << endl;
}
 */

#endif /* METROPOLIS_H */
