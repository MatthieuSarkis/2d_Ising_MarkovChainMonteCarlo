#ifndef SIM_H_
#define SIM_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdint>

#include "random_class.h"

struct Stat
{
	int E, M;
};

inline std::ostream& operator << (std::ostream& os, const Stat& c)
{
	os << c.E << '\t' << c.M;
	return os;
}

class Sim
{
protected:

	int lx, ly, lxy;
	std::vector<int8_t> spin;	
  Rand rnd;

	double norm_size;
	int E, M;
	float T;
  
  inline int CalculateEnergyPBC() const;
  inline int CalculateMagnet() const;


public:

  Sim();
  Sim (int lx, int ly) ;

  virtual void ResetTemperature (float T);
  virtual void ResetSize (int lx, int ly);
  inline void Copy(std::vector<bool>& vec);
  inline void Assign(const std::vector<bool>& vec);
  inline void SetRndSeed(int seed=12345);
  virtual void RunOneStep ();
  inline virtual void ResetSpins ();
  inline void SaveSpinConfig( std::ofstream& file, bool binary=false, bool raw_wise=true) const;
  inline void SaveStatistics(std::ofstream& file, bool binary=false) const;
  inline double GetEnergyPerSpin() const;
  inline double GetMagnetPerSpin() const;
  virtual inline int GetEenergy() const;
  virtual inline int GetMagnet() const;
  void ReCalculateEnergyMagnet ();
  inline int GetLatticeSize();

};



Sim::Sim()
{}

Sim::Sim (int lx, int ly)
{
  ResetSize(lx, ly);
}

virtual void Sim::ResetTemperature (float T)
{
  this->T = T;
}

virtual void Sim::ResetSize (int lx, int ly)
{
  this->lx = lx;
  this->ly = ly;
  this->lxy = lx * ly;
  spin.resize(lxy);
  norm_size = 1.0 / lxy;
}

inline void Sim::Copy(std::vector<bool>& vec)
{
  for (int i = 0; i < lxy; i++)
    vec[i] = spin[i] > 0 ? true : false;
}
inline void Sim::Assign(const std::vector<bool>& vec)
{
  for (int i = 0; i < lxy; i++)
    spin[i] = vec[i] ? 1 : -1;
}

inline void Sim::SetRndSeed(int seed=12345)
{
  rnd.SetSeed(seed);
}


virtual void Sim::RunOneStep ()
{}

inline virtual void Sim::ResetSpins ()
{
  for (auto& s : spin)
    s = (rnd.NextDouble() < 0.5) ? 1 : -1;
}


inline void Sim::SaveSpinConfig( std::ofstream& file, bool binary=false, bool raw_wise=true) const
{
  if (binary) {
    file.write((char*)spin.data(), spin.size()*sizeof(spin[0]));
  }
      
  else {
          if (raw_wise) {
              for (auto s : spin) file << (s+1)/2;
              file << '\n';
          }
          
    else {
              for (int i = 0; i < lx; i++) {
                  for (int j = 0; j < ly; j++) file << (spin[i*ly+j]+1) / 2;
                  file << '\n';
              }
              file << "#\n";
          }
  }
}

inline void Sim::SaveStatistics(std::ofstream& file, bool binary=false) const
{
  Stat stat_out = {GetEenergy(), GetMagnet()};

  if (binary)
    file.write((char*)&stat_out, sizeof(stat_out));
  else
    file << stat_out << '\n';
}

inline double Sim::GetEnergyPerSpin() const
{ return GetEenergy() * norm_size; }

inline double Sim::GetMagnetPerSpin() const
{ return GetMagnet() * norm_size; }

virtual inline int Sim::GetEenergy() const
{ return E; }

virtual inline int Sim::GetMagnet() const
{ return M; }

void Sim::ReCalculateEnergyMagnet ()
{
  E = CalculateEnergyPBC();
  M = CalculateMagnet();
}

inline int Sim::GetLatticeSize()
{return lxy;}

inline int Sim::CalculateEnergyPBC() const
{
  int e = 0;
  for (int i = 0; i < lxy; i++)
  {
    int8_t s0 = spin[i];
    int x = i / ly;
    int y = i - x * ly;
    int i2 = (x < lx - 1) ? i + ly : i + ly - lxy;    // i2: periodic along x axes
    int j2 = (y < ly - 1) ? i + 1  : i + 1 - ly;
    e += -s0 * (spin[i2] + spin[j2]);
  }
  return e;
}

inline int Sim::CalculateMagnet() const
{
  int m = std::accumulate(spin.begin(), spin.end(), 0);
  return m;
}



#endif
