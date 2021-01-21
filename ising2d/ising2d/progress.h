

#ifndef PROGRESS_H_
#define PROGRESS_H_

#include <algorithm>
#include <ctime>
#include <iostream>
#include <string>
#include <utility>

class Progress {
	
public:
	typedef int int_t;
	typedef double float_t;
	typedef long long int long_t;

private:
	float_t inverse_total_iterations;
	int_t total_iterations;
	int_t curr;
	int_t prev;
	long_t start_time;
	
public:

    Progress(){}
	
	Progress(const int_t totalin): 
			inverse_total_iterations(1 / (float_t)totalin),
			total_iterations(totalin),
			curr(0),
			prev(-1),
			start_time(clock())
	{}
    
    void Assign(const int_t totalin)
    {
        Reset(totalin);
    }

	void ReAssign(const int_t totalin)
	{
		inverse_total_iterations = 1 / (float_t)(totalin);
		total_iterations = totalin;
		//int_t idx = prev / (inverse_total_iterations * 100.0) - 1;
		//prev = (int_t)((float_t)(idx + 1) * inverse_total_iterations * 100);
	}
	
	
	void Reset()
	{
		curr = 0;
        prev = -1;
		start_time = clock();
	}
	
	void Reset(const int_t totalin)
	{
		inverse_total_iterations = 1 / (float_t)(totalin);	
		total_iterations = totalin;	
		curr = 0;
        prev = -1;
		start_time = clock();
	}
	
	void Next(const int_t idx, const char left_space[]="", std::ostream& stm = std::cout)
	{
		curr = (int_t)((float_t)(idx + 1) * inverse_total_iterations * 100);
		if (curr != prev) 
		{
			PrintTimePercentage(idx, left_space, stm);
			if (curr >= 100) 
				stm << std::endl;
		}
	}
	void PrintTimePercentage(const int_t smp, const char left_space[], std::ostream& stm)
	{
		stm << "\r" << left_space;

		// percentage done
		update_and_print_percentage_complete(stm);
		stm <<  ". ";
		// time elapsed
		get_and_print_elapsed_time(stm);
		stm <<  ". ";
		// estimated time to completion
		estimate_and_print_time_to_complete(smp, stm);
		stm <<  ". ";
		// estimated total time
		estimate_and_print_total_time(smp, stm);
		stm <<  ". ";
		// estimated completion time in local time
		//estimate_and_print_completion_local_time(smp, stm);
		stm << "        ";
		stm.flush();
	}

	void update_and_print_percentage_complete(std::ostream& stm)
	{
		stm << curr << " %";
		prev = curr;
	}

	void get_and_print_elapsed_time(std::ostream& stm)
	{
		stm << "done" <<  ": ";
		print_estimated_time(clock() - start_time, stm);
	}

	void estimate_and_print_time_to_complete(const int_t smp, std::ostream& stm)
	{
		stm << "todo" <<  ": ";
		print_estimated_time(((float_t)(total_iterations - smp - 1) / (float_t)(smp + 1)) * (float_t)(clock() - start_time), stm);
	}

	void estimate_and_print_total_time(const int_t smp, std::ostream& stm)
	{
		stm << "total" <<  ": ";
		print_estimated_time(((float_t)total_iterations / (float_t)(smp + 1)) * (float_t)(clock() - start_time), stm);
	}

	void estimate_and_print_completion_local_time(const int_t smp, std::ostream& stm)
	{
		stm << "ends" <<  ": ";
		time_t timer = time(NULL);
		timer += (((float_t)(total_iterations - smp - 1) / (float_t)(smp + 1)) * (float_t)(clock() - start_time)) / CLOCKS_PER_SEC;
		std::string tmp(ctime(&timer));
		tmp.erase(std::remove(tmp.begin(), tmp.end(), '\n'), tmp.end());
		stm << tmp;
	}

	void print_estimated_time(const long_t inp, std::ostream& stm)
	{
		long_t tm = inp;
		long_t days = tm / ((long_t)CLOCKS_PER_SEC * (long_t)60 * (long_t)60 * (long_t)24);
		tm %= ((long_t)CLOCKS_PER_SEC * (long_t)60 * (long_t)60 * (long_t)24);
		long_t hours = tm / ((long_t)CLOCKS_PER_SEC * (long_t)60 * (long_t)60);
		tm %= ((long_t)CLOCKS_PER_SEC * (long_t)60 * (long_t)60);
		long_t minutes = tm / ((long_t)CLOCKS_PER_SEC * (long_t)60);
		tm %= ((long_t)CLOCKS_PER_SEC * (long_t)60);
		long_t seconds = tm / ((long_t)CLOCKS_PER_SEC);
		if (days) {
			stm << days << " d ";
		}
		if (hours) {
			stm << hours << " h ";
		}
		if (minutes) {
			stm << minutes << " m ";
		}
		if (seconds) {
			stm << seconds << " s";
		}
	}

};

#endif // #ifndef PROGRESS_HPP_KJS
