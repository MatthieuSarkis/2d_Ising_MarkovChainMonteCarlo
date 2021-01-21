#ifndef GENERATOR_H_
#define GENERATOR_H_

//#include <iostream>
#include <cmath>


template <typename T>
class Generator
{
protected:
    static constexpr double epsilon = 1e-9;

    T start_, stop_;
    bool catch_endpoint_;
    T current_;

public:

    Generator(){}

    Generator (T start, T stop, bool catch_endpoint) : start_(start), 
                        stop_(stop), catch_endpoint_(catch_endpoint), current_(start)
    {}


    void Assign(T start, T stop, bool catch_endpoint)
    {
        start_ = start;
        stop_ = stop;
        catch_endpoint_ = catch_endpoint;
        current_ = start;
    }
    

    virtual inline T Next() = 0;

    inline void SetStop(T stop)
    {
        if (stop > stop_)
        {
            stop_ = stop;
        }
        else if (stop < stop_)
        {
            stop_ = stop;
            AdjustCurrent();
        }
        
    }
    
    inline void AdjustCurrent()
    {
        if (current_ > stop_ && catch_endpoint_) 
            current_ = stop_;
    }

    inline T Current() const
    {
        return current_;
    }

    inline T Stop() const
    {
        return stop_;
    }

    inline T Start() const
    {
        return start_;
    }

    inline bool IsBegin()
    {
        return ( current_ == start_ );
    }
    inline bool IsEnd()
    {
        return ( current_ >= stop_ );
    }
    
};


class LogSpace : public Generator<double>
{
private:
    double scale_;
    bool rounding_;
    int  increment_;
public: 
    LogSpace () : Generator()
    {}
    LogSpace (  double start, double stop, bool catch_endpoint, 
                double scale, bool rounding=true, int increment=1) : 
                    Generator(start, stop, catch_endpoint), scale_(scale), 
                    rounding_(rounding), increment_(increment)
    {}

    void Assign(double start, double stop, bool catch_endpoint, double scale, bool rounding=true, int increment=1)
    {
        Generator::Assign(start, stop, catch_endpoint);
        scale_ = scale;
        rounding_ = rounding;
        increment_ = increment;
    }

    inline double Next()
    {   
        if (rounding_)
        {
            double next_value = round(current_ * scale_);
            current_ = fabs(next_value - current_) < epsilon ? 
                                         current_ + increment_: next_value; 
        }
        else
            current_ *= scale_;

        AdjustCurrent();
        return current_;
    }

};


template <typename T>
class LinearSpace : public Generator<T>
{
private:
    T delta_;
public:
    LinearSpace(T start, T stop, bool catch_endpoint, T delta) : 
        Generator<T>(start, stop, catch_endpoint), delta_(delta)
    {}

    inline T Next()
    {
        this->current_ += delta_ ;
        this->AdjustCurrent();
        return this->current_;
    } 

};


template <typename T>
inline void GenerateLogSpace(	std::vector<T>& vec,
								double start, double stop, double scale,
								bool catch_endpoint, 
								bool rounding=true
								) 
{
	LogSpace generator(start, stop, catch_endpoint, scale, rounding);
    T val = generator.Current();
	vec.push_back(val);

	while(!generator.IsEnd()) 
	{
		val = generator.Next();
		vec.push_back(val);	
	} 
}


template <typename T>
inline void GenerateLinearSpaceUsingGenerator (	
                    std::vector<T>& vec,
                    T start, T stop, T delta,
					bool catch_endpoint
                    ) 
{
	LinearSpace<T> generator(start, stop, catch_endpoint, delta);
    T val = generator.Current();
	vec.push_back(val);

	while(!generator.IsEnd()) 
	{
		val = generator.Next();
		vec.push_back(val);	
	} 
}


template <typename T>
inline void GenerateLinearSpace ( 
                    std::vector<T>& vec,
                    T start, T stop, T delta) 
{
    int n = ((stop - start) / delta) + 1;
    for (int i = 0; i < n; i++)
        vec.push_back(i * delta + start);
}

#endif
