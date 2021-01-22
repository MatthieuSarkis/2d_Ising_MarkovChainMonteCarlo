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



 

int main()
{
  float Tc = 2 / log(1 + sqrt(2));
  
  int L=128;
  unsigned seed = static_cast<unsigned int>(chrono::steady_clock::now().time_since_epoch().count());
  Metropolis model(L, Tc);
  model.set_seed(seed);
  model.initialize_spins();
  for (int i = 0; i < 100; i++)
  {
    model.one_step_evolution();
  }
  
  ofstream file("./test.txt", ios::out);
  
  model.save_spin_lattice(file, false, false);
  
  file.close();
  
  return 0;
  
  int n_steps_initial = 1000;
  int n_steps_generation = 10;
  int n_steps_thermalize = 100;
  //int L = 128;


  int n_data_per_temp = 10;
  float T_min = 1.8;
  float T_max = 3.0;
  float dT = 0.012;
  
  
  
  
  //unsigned seed = static_cast<unsigned int>(chrono::steady_clock::now().time_since_epoch().count());

  // We create the vector containing all the temperatures, including the critical temperature, and order it in decreasing order.
  vector<float> T;
  for (int i = 0; i <= (int)((T_max - T_min) / dT); i++)
  {
    T.push_back(T_min + i * dT);
  }
  T.push_back(Tc);
  sort(T.begin(), T.end(), greater<float>());
  
  
  
  
  

  /*
  for (int j = 0; j < T.size(); j++)
  {
    //To see where at the progress
    cout << "Temperature number: " << j+1 << "/" << T.size() << endl;
    
    // We create the name of the file for the temperature T
    string file_name;
    stringstream ss;
    ss.setf(ios::fixed);
    ss << setprecision(4);
    ss << "(L=" << L << ",T=" << T[j] << ')';
    file_name = ss.str();
    
    // we instanciate an ofstream object and open the binary file for the given temperature
    ofstream file(("./data/" + file_name + ".bin").c_str(), ios::out | ios::binary);
    
    
     
    
    
    
    
    
    
    //file.close();
    
  }
  */
  
  return 0;
}
