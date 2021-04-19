//
// Created by Federico D'Ambrosio on 06/04/2021.
//

#include "CompleteBinaryTree.h"
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>
#include <fstream>
#include "RunningStats.hpp"

int main(){
	//CBT Test
	const unsigned long maxNExp = 20;
	const unsigned long minRExp = 10;
	const double max = 1.;
	const unsigned long runs = 10;
	const unsigned long samples = 1000;// x N
	const unsigned long updates = 1000;//x N

	std::mt19937_64 rng(time(nullptr));
	std::uniform_real_distribution<double> uniform(0,1);

	std::ofstream results("cbt-uniform.txt");

	for (unsigned long NExp = 1; NExp < maxNExp; NExp++){
		const unsigned long N = std::pow(10,NExp);
		std::cout << "\nN = "<<N<<":\t"<<std::flush;

		for (unsigned long RExp = 0; RExp < minRExp; RExp++){
			const double min = 1./std::pow(10.,RExp);
			std::cout << min<<std::flush;
			RunningStats sampleTime,updateTime;
			double chiSquared = 0;
			unsigned long dof = 0;

			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<CBTNode*> nodes;
				CompleteBinaryTree tree;
				for (unsigned long n = 0; n < N; n++){
					//uniform
					double r = uniform(rng)*(max-min)+min;
					//x
					//const double m = max*max-min*min;
					//const double q = min*min;
					//double r = std::sqrt(uniform(rng)*m+q);
					//beta = 1
//					double logMin = std::log(min);
//					double logMax = std::log(max);
//					double exp = uniform(rng)*(logMax-logMin)+logMin;
//					double r = std::exp(exp);
					//beta = 2
//					double rand = uniform(rng);
//					double r = (max*min)/(max-max*rand+min*rand);
					//beta = 3
//					double r = max*min/std::sqrt(max*max - max*max*rand + min*min*rand);
					nodes.emplace_back(tree.addLeaf(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
				}

				tree.updateTree();

				for (unsigned long s = 0; s < samples * N; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tree.sampleLeaf()->payload;
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
					sampleTime.Push(time_span.count());
					sampled[res] += 1.;
				}

				for (unsigned long s = 0; s < sampled.size(); s++) {
					chiSquared += std::pow(sampled[s]/(double)(samples*N) - generated[s],2)/generated[s];
				}
				dof += sampled.size();

				for (unsigned long s = 0; s < updates * N; s++) {
					//uniform
					double r = uniform(rng) * (max - min) + min;
					unsigned long randomId = std::floor(uniform(rng)*nodes.size());
					CBTNode * randomPointer = nodes[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					tree.updateLeaf(randomPointer,r);
					tree.updateTree();
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
							t2 - t1);
					updateTime.Push(time_span.count());
				}


			}
			results << N << "\t" << min << "\t" << sampleTime.Mean() << "\t" << sampleTime.StandardDeviation()/std::sqrt(sampleTime.NumDataValues())
			        <<"\t"<< updateTime.Mean()<<"\t"<<updateTime.StandardDeviation()/std::sqrt(updateTime.NumDataValues())
			        << chiSquared << "\t"<<dof<< std::endl;

			std::cout<<"! ";

			//std::cout << N << "\t" << min << std::endl;

		}



	}

	return 0;
}