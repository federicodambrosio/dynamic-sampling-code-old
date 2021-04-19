#include <iostream>
#include <cmath>
#include <fstream>
#include <random>
#include <array>

int main(){
	std::ofstream out("out.txt");

	const double min = 0.01;
	const double max = 1.;
	const double dr = 0.01;
	const unsigned long runs = 10000000;
	
	std::array<double,100> freqU,freqX,freq1,freq2,freq3;
	freqU.fill(0.);
	freqX.fill(0.);
	freq1.fill(0.);
	freq2.fill(0.);
	freq3.fill(0.);

	
	std::mt19937_64 rng(time(nullptr));
	std::uniform_real_distribution<double> uniform(0,1);

	
	for (int i = 0; i < runs; i++){
		//uniform
		double rU = uniform(rng)*(max-min)+min;
		freqU[(unsigned long)((rU-min)/dr)] += 1.;
		
		//x
		const double m = max*max-min*min;
		const double q = min*min;
		double rX = std::sqrt(uniform(rng)*m+q);
		freqX[(unsigned long)((rX-min)/dr)] += 1.;
		
		//beta = 1
		double logMin = std::log(min);
		double logMax = std::log(max);
		double exp = uniform(rng)*(logMax-logMin)+logMin;
		double r1 = std::exp(exp);
		freq1[(unsigned long)((r1-min)/dr)] += 1.;
		
		//beta = 2
		double rand2 = uniform(rng);
		double r2 = (max*min)/(max-max*rand2+min*rand2);
		freq2[(unsigned long)((r2-min)/dr)] += 1.;
		
		//beta = 3
		double rand3 = uniform(rng);
		double r3 = max*min/std::sqrt(max*max - max*max*rand3 + min*min*rand3);
		freq3[(unsigned long)((r3-min)/dr)] += 1.;
	}


	for (int i = 0; i < 100; i++){
		freqU[i] /= (double)runs;
		freqX[i] /= (double)runs;
		freq1[i] /= (double)runs;
		freq2[i] /= (double)runs;
		freq3[i] /= (double)runs;	
		//std::cout << min << "\t" << dr << std::endl;
		
		out << (double)i * dr + min << "\t" << freqU[i] << "\t" << freqX[i] << "\t";
		out << freq1[i] <<"\t" << freq2[i] << "\t" << freq3[i] << std::endl;
	}
	out.close();

	return 0;
}