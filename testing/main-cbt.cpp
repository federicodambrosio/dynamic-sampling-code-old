//
// Test code for the Complete Binary Tree Class
//
#include "../CompleteBinaryTree.h"
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>
#include <fstream>
#include "../RunningStats.hpp"

int main(){
	const unsigned long maxNExp = 7;				//Nmax = 10^maxNExp
	const unsigned long minRExp = 5;				//smallest value of min = 10^minRExp
	const unsigned long deltaR = 1;					//increases in RExp
	const double deltaN = 1;						//increases in N
	const double max = 1.;							//max is constant
	const unsigned long runs = 100;					//how many iterations per distro 
	const unsigned long samples = 10000;			//how many samples per iter
	const unsigned long updates = 100;				//how many updates per iter

	std::mt19937_64 rng(time(nullptr));
	std::uniform_real_distribution<double> uniform(0,1);

	const std::string outx = "cbt-x.txt";
		
	//Since distribution doesn't matter, we do only X
	std::ofstream resultsX(outx);

	for (double NExp = 1; NExp <= maxNExp; NExp+=deltaN){
		const unsigned long N = std::pow(10,NExp);
		std::cout << "\nN = "<<N<<":\t"<<std::flush;

		for (unsigned long RExp = 0; RExp <= minRExp; RExp+=deltaR){
			const double min = 1./std::pow(10.,RExp);
			std::cout << min<<std::flush;
			RunningStats sampleTime,updateTime;
			double chiSquared = 0;
			unsigned long dof = 0;
			const unsigned long sampleHere = samples;
			//std::pow(10.,RExp) * samples;
			
			double totRate = 0.;
			
			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<CBTNode*> element;
				CompleteBinaryTree tree;
				for (unsigned long n = 0; n < N-1; n++){
					//x
					const double m = max*max-min*min;
					const double q = min*min;
					double r = std::sqrt(uniform(rng)*m+q);
					element.emplace_back(tree.addLeaf(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}
				tree.updateTree();
			

				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tree.sampleLeaf()->payload;
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
					sampleTime.Push(time_span.count());
					sampled[res] += 1.;
				}

				for (unsigned long s = 0; s < sampled.size(); s++) {
					chiSquared += std::pow(sampled[s]/(double)(sampleHere*N) - generated[s]/totRate,2)/(generated[s]/totRate);
				}

				for (unsigned long s = 0; s < updates; s++) {
					//uniform
					double r = uniform(rng) * (max - min) + min;
					unsigned long randomId = std::floor(uniform(rng)*element.size());
					CBTNode * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tree.sampleLeaf()->payload;
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
							t2 - t1);
					updateTime.Push(time_span.count());
				}

				dof += 1;
			}
			
			dof -= 1;
			
			resultsX << N << "\t" << min << "\t" << sampleTime.Mean() << "\t" << sampleTime.StandardDeviation()/std::sqrt(dof)
			        <<"\t"<< updateTime.Mean()<<"\t"<<updateTime.StandardDeviation()/std::sqrt(dof) << "\t"<< chiSquared << "\t"<<dof<< "\t"<<(chiSquared-dof)/std::sqrt(2*dof)<< std::endl;

			std::cout<<"! ";

			//std::cout << N << "\t" << min << std::endl;

		}



	}
	
	resultsX.close();


	return 0;
}
