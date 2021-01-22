#ifndef params_h
#define params_h

#include <math.h>

float T_critical(int L)
{
    float Tc = 2 / log(1 + sqrt(2));
    return Tc / (1 + 5 / (4.0 * L));
}

int n_steps_initial = 1000;
int n_steps_generation = 10;
int n_steps_thermalize = 100;
int L = 128;

float Tc = T_critical(L);

int n_data_per_temp = 10;
float T_min = 1.8;
float T_max = 3.0;
float dT = 0.012;

#endif /* params_h */
