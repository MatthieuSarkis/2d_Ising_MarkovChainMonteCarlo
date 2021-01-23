//
//  main2.cpp
//  ising
//
//  Created by matthieu.sarkis on 2021/01/22.
//

#include "metropolis_class.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>
#include <ctime>
#include <cmath>

using namespace std;

bool isSame(float x, float y)
{
  return fabs(x - y) < 10e-6;
}

int main()
{
  float Tc = 2 / log(1 + sqrt(2));
  
  int L = 128;
  unsigned seed = static_cast<unsigned int>(chrono::steady_clock::now().time_since_epoch().count());
  
  int n_steps_initial = 1000;
  int n_steps_generation = 10;
  int n_steps_thermalize = 100;
  int n_data_per_temp = 1000;
  float T_min = 1.8;
  float T_max = 3.0;
  float dT = 0.012;
  
  vector<float> T;
  for (int i = 0; i <= (int)((T_max - T_min) / dT); i++)
  {
    T.push_back(T_min + i * dT);
  }
  T.push_back(Tc);
  sort(T.begin(), T.end(), greater<float>());
  
  Metropolis model(L, T_max);
  model.set_seed(seed);
  model.initialize_spins();
  int n_steps = 0;
  
  for (int i = 0; i < T.size(); i++)
  {
    
    string file_name;
    stringstream ss;
    ss.setf(ios::fixed);
    ss << setprecision(4);
    ss << "(L=" << L << ",T=" << T[i] << ')';
    file_name = ss.str();
    
    ofstream file(("/Users/matthieu.sarkis/git_repos/data_2d_Ising_MarkovChainMonteCarlo/" + file_name + ".bin").c_str(), ios::out | ios::binary);
    
    n_steps = (i == 0 || isSame(fabs(T[i] - Tc), dT)) ? n_steps_initial : n_steps_thermalize;
    
    for (int j = 0; j < n_steps; j++)
    {
      model.one_step_evolution();
    }
          
    for (int j = 0; j < n_data_per_temp; j++)
    {
      for (int k = 0; k < n_steps_generation; k++)
      {
        model.one_step_evolution();
        model.save_spin_lattice(file, false, true);
      }
    }
    
    file.close();
    
  }
  
  return 0;
}
