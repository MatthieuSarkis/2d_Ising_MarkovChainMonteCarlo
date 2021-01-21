#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <cmath>
#include <vector>


struct CountValue
{
	unsigned int count;
	double value;
};


inline void AddIncrease(CountValue& cv, int c, double v)
{
	cv.count += c;
	cv.value += v;
}

inline bool IsEqual(float d1, float d2)
{
	return std::fabs(d1 - d2) < 1e-6;
}

inline int Pow(int x, int y)
{
    int result = 1;
    for (int i = 0; i < y; i++)
        result *= x;
    return result;
}

template <typename T>
inline void Round(std::vector<T>& vec, int precision)
{
	int p10 = Pow(10, precision);
	for (T& v : vec)
	{
		v = round(v * p10) / p10;
	}
}


#endif
