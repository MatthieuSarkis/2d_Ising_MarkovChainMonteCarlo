#ifndef METROPOLIS_H_
#define METROPOLIS_H_

#include "sim.h"


class MetropolisSim : public Sim
{
protected:

	enum {DE_MAX = 8}; 		// de_max = 2*4, de_min = -2*4
	double W_ratio[2 * DE_MAX + 1];	// the weight ratio array

  inline double GetWeightRatio(int de);
  inline int DeltaEnergyFlippedSite(int i);
  
public:

  MetropolisSim() : Sim();
  MetropolisSim(int lx, int ly) : Sim(lx, ly);
  void ResetTemperature (float T) override;
  void RunOneStep() override;

};

  
  
MetropolisSim::MetropolisSim() : Sim()
{}
  
MetropolisSim::MetropolisSim(int lx, int ly) : Sim(lx, ly)
{}

void MetropolisSim::ResetTemperature (float T) override
{
  this->T = T;
  for(int de = -DE_MAX; de <= DE_MAX; de++)
    W_ratio[de + DE_MAX] = exp(-de/T);
  //ReCalculateEnergyMagnet();
}

void MetropolisSim::RunOneStep() override
{
  for (int trial = 0; trial < lxy; trial++)
  {
    int i = rnd.NextInt(lxy);
    int de = DeltaEnergyFlippedSite(i);
     
    if (de <= 0 || rnd.NextDouble() < GetWeightRatio(de))
    { // the fliping is accepted
      spin[i] = -spin[i];
      M += 2 * spin[i];
      E += de;
    }
  } // end for trial
}

inline double MetropolisSim::GetWeightRatio(int de)
{
  return W_ratio[de + DE_MAX];
}

inline int MetropolisSim::DeltaEnergyFlippedSite(int i)
{
  int8_t s = spin[i];
  int x = i / ly;
  int y = i - x * ly;

  int i1 = (x > 0) ? i - ly : i - ly + lxy;
  int i2 = (y > 0) ? i - 1  : i - 1 + ly;
  int i3 = (x < lx - 1) ? i + ly : i + ly - lxy;
  int i4 = (y < ly - 1) ? i + 1  : i + 1 - ly;

  int de = 2 * s * (spin[i1] + spin[i2] + spin[i3] + spin[i4]);
  return de;
}


#endif
