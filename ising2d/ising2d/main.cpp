#include <sys/stat.h>
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

#include "macros.h"
#include "utilities.h"
#include "progress.h"
#include "sim.h"
#include "metropolis.h"
#include "path.h"
#include "generator.h"

using namespace std;

////////////////////////////////////////////////////////////////////
void Intro      (int argc, char* argv[]);
void InitWorld  (int argc, char* argv[]);
string GetFilePath  (const string& odir, const string& bname, int L, float T,
                        const char format[]);
void Run (  int re, int start_step, int end_step, float T,
            const bool print_energy_per_step, 
            const bool calc_stat, 
            const bool export_config,
            const bool show_progressbar);

//////////////////////////////////////////////////import some params
int max_n_realizations = 2;
int L = 3;
float T_down = 3, T_up = 3, dT = 0.05;
bool Tc_is_considered = true;
bool use_last_config_per_temperature = true;
int equilib_steps_1st = 1000;
int equilib_steps_2nd = 100;
int steps_per_sample = 10;
int n_samples_per_realization = 2;

const bool print_energy_per_step = true;
const bool export_spin_config = true;
const bool export_stat = true;
const bool export_stat_as_binary = true;
const bool export_spin_config_as_binary = true;

const float Tc = 2.26918531421F; 
////////////////////////////////////////////////////////////////////
int stedaystate_steps;
vector<float> T_array;
Sim* sim;
vector<bool> last_spin_config;
string osuffix = "";
string init_dir = "";
string dir_data = "stat-files";
string dir_config = "config-files";
string dir_energy = "energy-files";
const string dashedline_separator (62, '-');
const string equalline_separator (72, '=');
////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{ 
    InitWorld(argc, argv);

    Intro(argc, argv);

    for (size_t i = 0; i < T_array.size(); i++)
    {
        clock_t start_time = clock();

        auto T = T_array[i];
        cout << "# T=" << T << endl;

        sim->ResetTemperature(T);        
               
        for (int re = 1; re <= max_n_realizations; re++)
        {
            cout << " # Realization " << re << "/" << max_n_realizations << ": " << endl;

            // bring the last spin config
            if (use_last_config_per_temperature && i > 0)
                sim->Assign(last_spin_config);
            else
                sim->ResetSpins();

            // we should calculate energy it again because of changing spin set
            sim->ReCalculateEnergyMagnet();


            // find the appropriate steps  
            int equilib_steps = equilib_steps_1st;
            if (use_last_config_per_temperature && i > 0 && !IsEqual(T, Tc))
                equilib_steps = equilib_steps_2nd;


            cout << " --> Equilibrating (" << equilib_steps <<  " steps) ... "   << endl;
            Run (re, 1, equilib_steps, T, 
                    print_energy_per_step, false, false, false);

            cout << " --> GetSamples (" << stedaystate_steps <<  " steps) ... "   << endl;
            Run (re, equilib_steps + 1, equilib_steps + stedaystate_steps, T, 
                    print_energy_per_step, export_stat, export_spin_config, true);

        }  // end for re
        
        // we copy the last config to use later
        if (use_last_config_per_temperature)
            sim->Copy(last_spin_config);

       
        cout << "# elapsed_time=" 
             << (clock() - start_time) / CLOCKS_PER_SEC  << " secs."<< endl;
        cout << dashedline_separator << endl;
    } // end for T
    
    return 0;
}



void Run (  int re, int start_step, int end_step, float T, 
            const bool print_energy_per_step,
            const bool calc_stat,
            const bool export_config,
            const bool show_progressbar
        )
{
    vector<int> desired_steps;
    ofstream file_energy, file_stat, file_config;
    Progress prgress;


    if (print_energy_per_step)
    {
        GenerateLogSpace(desired_steps, start_step, end_step, 1.1, true, true);
        string fname = GetFilePath(dir_energy, "EM", L, T, ".dat");

        auto fmode = ios::out;
        if (re > 1 || start_step > 1) fmode |= ios::app;
        file_energy.open(fname, fmode);
    }

    if (calc_stat)
    {
        const char* fmt = (export_stat_as_binary) ? ".bin" : ".dat";
        string fname = GetFilePath(dir_data, "stat", L, T, fmt);
        auto fmode = ios::out;
        if (re > 1) fmode |= ios::app;
        if (export_stat_as_binary) fmode |= ios::binary;
        file_stat.open(fname, fmode);
    }

    if (export_config)
    {
        const char* fmt = (export_spin_config_as_binary) ? ".bin" : ".dat";
        string fname = GetFilePath(dir_config, "spin", L, T, fmt);
        auto fmode = ios::out;
        if (re > 1) fmode |= ios::app;
        if (export_spin_config_as_binary) fmode |= ios::binary;
        file_config.open(fname, fmode);
    }
    

    if (show_progressbar)
    {
        int total_steps = end_step - start_step + 1;
        prgress.Assign(total_steps);
    }
       

    for(int step = start_step, t = 1, ie = 0; step <= end_step; step++, t++)
    {
        sim->RunOneStep();

        bool can_calc_energy = print_energy_per_step && step == desired_steps[ie];
        bool can_calc_stat = calc_stat && (t % steps_per_sample == 0 || step == end_step);

        if (can_calc_energy)
        {
            double E = sim->GetEnergyPerSpin();
            double M = sim->GetMagnetPerSpin();
            file_energy << step << '\t' << E << '\t' << M << '\n';
            ie++;
        }

        if (can_calc_stat)
        {
            sim->SaveStatistics(file_stat, export_stat_as_binary);
            if (export_config)
                sim->SaveSpinConfig(file_config, export_spin_config_as_binary);
        }

        if (show_progressbar)
            prgress.Next(t - 1, "      ");
        
    }

    file_energy.close();
    file_stat.close();
    file_config.close();
}




void Intro(int argc, char* argv[])
{
    cout    << equalline_separator                          << endl;
  
    cout    << "## 2D Ising with PBCs"          << " ##"    << endl;
	  cout	  << "# L="	                          << L        << endl
			      << "# n_realizaions="               << max_n_realizations           << endl
            << "# T_up=" << T_up << " T_down="  << T_down << " dT=" << dT 
            << " Tc="    << Tc 													<< endl   
            << "# Tc_is_considered=" << (Tc_is_considered ? "Yes":"No")  		<< endl
            << "# use_last_config_per_temperature=" << (use_last_config_per_temperature ? "Yes":"No")  		<< endl
            << "# equilib_steps_1st="           << equilib_steps_1st            << endl
            << "# equilib_steps_2nd="           << equilib_steps_2nd            << endl
            << "# stedaystate_steps="           << stedaystate_steps            << endl
            << "# steps_per_sample="            << steps_per_sample             << endl
            << "# n_samples_per_realization="   << n_samples_per_realization    << endl
            << "# out_suffix=\""    << osuffix  << "\""                         << endl
            << "# init_dir=\""      << init_dir << "\""                         << endl
            << "# print_energy_per_step="           << (print_energy_per_step ? "Yes" : "No")           << endl
            << "# export_spin_config="              << (export_spin_config ? "Yes" : "No")              << endl
            << "# export_stat="                     << (export_stat ? "Yes" : "No")                     << endl;
            
    cout    << equalline_separator          << endl;
}

void InitWorld(int argc, char* argv[])
{
    auto NotAcceptablePath = [](const string& st) 
    { return  (st == "") || (st[0] == '_') || (st[0]=='>') || (st[0]=='&') || (st[0]=='<') || (st[0]=='|'); };
    
    auto IsYes = [](const string& st) 
    { return (st[0]=='y') || (st[0]=='Y'); };
    
    auto IsSpecificChar = [](const string& st) 
    { return (st[0]=='>') || (st[0]=='&') || (st[0]=='<') || (st[0]=='|'); };
    
    cout << "** USAGE |./sim.x|L|max_n_realizations|T_up|T_down|dT|"        << endl
         << "         |Tc_is_considered(y/n)|"                              << endl  
         << "         |use_last_config_per_temperature(y/n)|"               << endl
         << "         |equilib_steps_1st|equilib_steps_2nd|"                << endl
         << "         |steps_per_sample|n_samples_per_realization|"         << endl
         << "         |out_suffix|init_dir|"                                << endl;

    if (argc > 1 && !IsSpecificChar(argv[1]))
    {
        int i = 1;

        L = atoi(argv[i++]);
        max_n_realizations = atoi(argv[i++]);
        
        T_up = atof(argv[i++]);
        T_down = atof(argv[i++]);
        dT = atof(argv[i++]);
        Tc_is_considered = IsYes(argv[i++]);
        use_last_config_per_temperature = IsYes(argv[i++]);
        
        equilib_steps_1st = atoi(argv[i++]);
        equilib_steps_2nd = atoi(argv[i++]);
        
        steps_per_sample = atoi(argv[i++]);
        n_samples_per_realization = atoi(argv[i++]);
        
        osuffix = argv[i++];
        init_dir = argv[i++];
    }
    
    if (IsSpecificChar(osuffix)) osuffix = "0";
    if (NotAcceptablePath(init_dir)) init_dir = "";

    ostringstream stL; stL << "L=" << L;
    dir_energy = Path::PathJoin(init_dir, dir_energy, stL.str());
    dir_data = Path::PathJoin(init_dir, dir_data, stL.str());
    dir_config = Path::PathJoin(init_dir, dir_config, stL.str());

    if (print_energy_per_step)
        Path::MakeDirRecursively(dir_energy);
    if (export_stat)
        Path::MakeDirRecursively(dir_data);
    if (export_spin_config)
        Path::MakeDirRecursively(dir_config);
  
    
    pair<float,float> mmT = std::minmax(T_up, T_down);
    T_down = mmT.first;
    T_up = mmT.second;
    
    GenerateLinearSpace(T_array, T_down, T_up, dT);
    if (Tc_is_considered)
        T_array.push_back(Tc);
    std::sort(T_array.begin(), T_array.end(), greater<float>());
    //T_array.erase( std::unique(T_array.begin(), T_array.end()), T_array.end() );
    
    
    stedaystate_steps = n_samples_per_realization * steps_per_sample;

    // part initialize sim object
    sim = new MetropolisSim(L, L);
    #ifdef INIT_RANDOM_SEED
    sim->SetRndSeed(INIT_RANDOM_SEED);
    #endif
   

    if (use_last_config_per_temperature)
        last_spin_config.resize(sim->GetLatticeSize());
}

string GetFilePath (const string& odir, const string& bname, int L, float T, 
                    const char format[])
{
    ostringstream st;
	st << "(L="	<< L << ",T=" << std::fixed << std::setprecision(4) 
        << T << ")" << osuffix << format;
    
    return Path::PathJoin(odir, bname + st.str());
}
