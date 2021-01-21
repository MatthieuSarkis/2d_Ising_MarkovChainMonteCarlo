#ifndef RANDOMCLASS_H_
#define RANDOMCLASS_H_

#include <iostream>
#include <random>
#include <ctime>

class Rand
{
protected:
	std::mt19937 rgen_;

public:

	Rand();
	Rand(unsigned int seed);

	void SetSeed(unsigned int seed);
	void SetSeed();
	double NextDouble();
  double NextNormal(double = 0, double = 1);
	int NextInt(int first, int last);
	int NextInt(int last);

	std::mt19937& GetRgen();
};



Rand::Rand()
{
	SetSeed();
}

Rand::Rand(unsigned int seed)
{
	SetSeed(seed);
}

inline void Rand::SetSeed(unsigned int seed)
{
	rgen_.seed(seed);
}

inline void Rand::SetSeed()
{
#ifdef _WIN32
	rgen_.seed((unsigned int)time(nullptr));
#else
	rgen_.seed(std::random_device{}());
#endif
}

inline double Rand::NextDouble()
{
	return std::uniform_real_distribution<>(0, 1.0)(rgen_);
}

inline double Rand::NextNormal(double mean, double stddev)
{
	return std::normal_distribution<>(mean, stddev)(rgen_);
}

inline int Rand::NextInt(int first, int last)
{
	return std::uniform_int_distribution<>(first, last-1)(rgen_);
}

inline int Rand::NextInt(int last)
{
	return  std::uniform_int_distribution<>(0, last-1)(rgen_);
}

inline std::mt19937& Rand::GetRgen()
{
	return rgen_;
}

#endif
