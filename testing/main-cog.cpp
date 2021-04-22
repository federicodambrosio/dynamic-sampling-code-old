//
// Test code for the CascadeOfGroups Class
//


#include "../CascadeOfGroups.h"
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include "../RunningStats.hpp"

int main(){
	const unsigned long maxNExp = 5;				//Nmax = 10^maxNExp
	const unsigned long minRExp = 50;				//smallest value of min = 10^minRExp
	const unsigned long deltaR = 1;					//increases in RExp
	const double deltaN = 1;						//increases in N
	const double max = 1.;							//max is constant
	const unsigned long runs = 100;					//how many iterations per distro 
	const unsigned long samples = 10000;			//how many samples per iter
	const unsigned long updates = 100;				//how many updates per iter
	
	const bool doX = true;
	const bool doUni = true;
	const bool dob1 = true;
	const bool dob2 = true;
	const bool dob3 = true;
	
	const std::string outx = "cog-x.txt";
	const std::string outU = "cog-uni.txt";
	const std::string outb1 = "cog-b1.txt";
	const std::string outb2 = "cog-b2.txt";
	const std::string outb3 = "cog-b3.txt";

	std::mt19937_64 rng(time(nullptr));
	std::uniform_real_distribution<double> uniform(0,1);

	//X
	if (doX){
		std::ofstream resultsX(outx);

		for (double NExp = 1; NExp <= maxNExp; NExp+=deltaN){
			const unsigned long N = std::pow(10,NExp);
			std::cout << "\nN = "<<N<<":\t"<<std::flush;

			for (unsigned long RExp = 1; RExp <= minRExp; RExp+=deltaR){
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
					std::vector<elementCoG*> element;
					CascadeOfGroups cog(false,min,1.,2);
					for (unsigned long n = 0; n < N-1; n++){
						//uniform
	// 					double r = uniform(rng)*(max-min)+min;
						//x
						const double m = max*max-min*min;
						const double q = min*min;
						double r = std::sqrt(uniform(rng)*m+q);
						//beta = 1
	// 				double logMin = std::log(min);
	// 				double logMax = std::log(max);
	// 				double exp = uniform(rng)*(logMax-logMin)+logMin;
	// 				double r = std::exp(exp);
						//beta = 2
	// 				double rand = uniform(rng);
	// // 				double r = (max*min)/(max-max*rand+min*rand);
	// 					//beta = 3
	// 				double r = max*min/std::sqrt(max*max - max*max*rand + min*min*rand);
						element.emplace_back(cog.addElement(n,r));
						generated.emplace_back(r);
						sampled.emplace_back(0.);
						totRate += r;
					}
					
				

					for (unsigned long s = 0; s < sampleHere; s++){
						std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
						unsigned long res = cog.sampleElement()->payload;
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
						elementCoG * randomPointer = element[randomId];
						std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
						cog.updateElement(randomPointer,r);
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
	}
	//U
	if (doUni){
	std::ofstream resultsU(outU);

	for (double NExp = 1; NExp <= maxNExp; NExp+=deltaN){
		const unsigned long N = std::pow(10,NExp);
		std::cout << "\nN = "<<N<<":\t"<<std::flush;

		for (unsigned long RExp = 1; RExp <= minRExp; RExp+=deltaR){
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
				std::vector<elementCoG*> element;
				CascadeOfGroups cog(false,min,1.,2);
				for (unsigned long n = 0; n < N-1; n++){
					//uniform
 					double r = uniform(rng)*(max-min)+min;
					element.emplace_back(cog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}
				
			

				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = cog.sampleElement()->payload;
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
					elementCoG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					cog.updateElement(randomPointer,r);
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
							t2 - t1);
					updateTime.Push(time_span.count());
				}

				dof += 1;
			}
			
			dof -= 1;
			
			resultsU << N << "\t" << min << "\t" << sampleTime.Mean() << "\t" << sampleTime.StandardDeviation()/std::sqrt(dof)
			        <<"\t"<< updateTime.Mean()<<"\t"<<updateTime.StandardDeviation()/std::sqrt(dof) << "\t"<< chiSquared << "\t"<<dof<< "\t"<<(chiSquared-dof)/std::sqrt(2*dof)<< std::endl;

			std::cout<<"! ";

			//std::cout << N << "\t" << min << std::endl;

		}



	}
	resultsU.close();
	}
	//b1
	if (dob1){
	std::ofstream resultsb1(outb1);

	for (double NExp = 1; NExp <= maxNExp; NExp+=deltaN){
		const unsigned long N = std::pow(10,NExp);
		std::cout << "\nN = "<<N<<":\t"<<std::flush;

		for (unsigned long RExp = 1; RExp <= minRExp; RExp+=deltaR){
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
				std::vector<elementCoG*> element;
				CascadeOfGroups cog(false,min,1.,2);
				for (unsigned long n = 0; n < N-1; n++){
					//beta = 1
					double logMin = std::log(min);
					double logMax = std::log(max);
					double exp = uniform(rng)*(logMax-logMin)+logMin;
					double r = std::exp(exp);
					element.emplace_back(cog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}
				
			

				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = cog.sampleElement()->payload;
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
					elementCoG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					cog.updateElement(randomPointer,r);
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
							t2 - t1);
					updateTime.Push(time_span.count());
				}

				dof += 1;
			}
			
			dof -= 1;
			
			resultsb1 << N << "\t" << min << "\t" << sampleTime.Mean() << "\t" << sampleTime.StandardDeviation()/std::sqrt(dof)
			        <<"\t"<< updateTime.Mean()<<"\t"<<updateTime.StandardDeviation()/std::sqrt(dof) << "\t"<< chiSquared << "\t"<<dof<< "\t"<<(chiSquared-dof)/std::sqrt(2*dof)<< std::endl;

			std::cout<<"! ";

			//std::cout << N << "\t" << min << std::endl;

		}



	}
	resultsb1.close();
	}
	
	if (dob2){
	std::ofstream resultsb2(outb2);

	for (double NExp = 1; NExp <= maxNExp; NExp+=deltaN){
		const unsigned long N = std::pow(10,NExp);
		std::cout << "\nN = "<<N<<":\t"<<std::flush;

		for (unsigned long RExp = 1; RExp <= minRExp; RExp+=deltaR){
			const double min = 1./std::pow(10.,RExp);
			std::cout << min<<std::flush;
			RunningStats sampleTime,updateTime;
			double chiSquared = 0;
			unsigned long dof = 0;
			const unsigned long sampleHere = samples;
			//std::pow(10.,RExp) * samples;
			
			double totRate = 0.;
			
			const double fact1 = max*min;
			
			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<elementCoG*> element;
				CascadeOfGroups cog(false,min,1.,2);
				for (unsigned long n = 0; n < N-1; n++){
					//beta = 2
					double r = 1e99;
					while (r < min or r > max){
						double rand = uniform(rng);
						r = fact1/(max-max*rand+min*r);
					}
					
					element.emplace_back(cog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}
				
			

				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = cog.sampleElement()->payload;
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
					elementCoG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					cog.updateElement(randomPointer,r);
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
							t2 - t1);
					updateTime.Push(time_span.count());
				}

				dof += 1;
			}
			
			dof -= 1;
			
			resultsb2 << N << "\t" << min << "\t" << sampleTime.Mean() << "\t" << sampleTime.StandardDeviation()/std::sqrt(dof)
			        <<"\t"<< updateTime.Mean()<<"\t"<<updateTime.StandardDeviation()/std::sqrt(dof) << "\t"<< chiSquared << "\t"<<dof<< "\t"<<(chiSquared-dof)/std::sqrt(2*dof)<< std::endl;

			std::cout<<"! ";

			//std::cout << N << "\t" << min << std::endl;

		}



	}
	
	resultsb2.close(); 
	}
	if (dob3){
	std::ofstream resultsb3(outb3);
	std::ofstream distro("distribuzione.txt");

	for (double NExp = 1; NExp <= maxNExp; NExp+=deltaN){
		const unsigned long N = std::pow(10,NExp);
		std::cout << "\nN = "<<N<<":\t"<<std::flush;

		for (unsigned long RExp = 1; RExp <= minRExp; RExp+=deltaR){
			const double min = 1./std::pow(10.,RExp);
			std::cout << min<<std::flush;
			RunningStats sampleTime,updateTime;
			double chiSquared = 0;
			unsigned long dof = 0;
			const unsigned long sampleHere = samples;
			//std::pow(10.,RExp) * samples;
			
			const double factor1 = std::pow(min,2) * std::pow(max,2);
			const double max2 = std::pow(max,2);
			const double min2 = std::pow(min,2);
			
			double totRate = 0.;
			
			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<elementCoG*> element;
				CascadeOfGroups cog(false,min,1.,2);
				for (unsigned long n = 0; n < N-1; n++){
					//beta = 3
					double rand = uniform(rng); 
					double rAlt = std::sqrt(factor1/(-max2*rand+min2*rand+max2));
					element.emplace_back(cog.addElement(n,rAlt));
					generated.emplace_back(rAlt);
					sampled.emplace_back(0.);
					totRate += rAlt;
				}
				
				auto ratesGroups = cog.rates();
				
				std::ofstream distro("distribuzione.txt");
				for (unsigned long g = 0; g < ratesGroups.size(); g++){
					distro << g << "\t" << ratesGroups[g] << "\n";
				}
				distro.close();

				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = cog.sampleElement()->payload;
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
					sampleTime.Push(time_span.count());
					sampled[res] += 1.;
				}
				if (i == 0 && NExp == 3 && RExp == 20) {
					std::ofstream samp("sampled.txt");
					for (unsigned long s = 0; s < sampled.size(); s++){
						samp << generated.at(s)/(double)totRate << "\t" << sampled.at(s)/(double)sampleHere << "\n";
					}
					samp.close();
				}

				
				for (unsigned long s = 0; s < sampled.size(); s++) {
					chiSquared += std::pow(sampled[s]/(double)(sampleHere*N) - generated[s]/totRate,2)/(generated[s]/totRate);
				}

				for (unsigned long s = 0; s < updates; s++) {
					//uniform
					double r = uniform(rng) * (max - min) + min;
					unsigned long randomId = std::floor(uniform(rng)*element.size());
					elementCoG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					cog.updateElement(randomPointer,r);
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
							t2 - t1);
					updateTime.Push(time_span.count());
				}

				dof += 1;
			}
			
			dof -= 1;
			
			dof = 1;
			
			resultsb3 << N << "\t" << min << "\t" << sampleTime.Mean() << "\t" << sampleTime.StandardDeviation()/std::sqrt(dof)
			        <<"\t"<< updateTime.Mean()<<"\t"<<updateTime.StandardDeviation()/std::sqrt(dof) << "\t"<< chiSquared << "\t"<<dof<< "\t"<<(chiSquared-dof)/std::sqrt(2*dof)<< std::endl;

			std::cout<<"! ";

			//std::cout << N << "\t" << min << std::endl;

		}



	}
	
	resultsb3.close();
	}
	return 0;
}