//
// Test code for the TOG Class
//

#include "../TOG.h"
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include "../RunningStats.hpp"

int main(){
	//TOG Test
	const unsigned long maxNExp = 5;				//Nmax = 10^maxNExp
	const unsigned long minRExp = 50;				//smallest value of min = 10^minRExp
	const unsigned long deltaR = 1;					//increases in RExp
	const double deltaN = 1;						//increases in N
	const double max = 1.;							//max is constant
	const unsigned long runs = 100;					//how many iterations per distro 
	const unsigned long samples = 10000;			//how many samples per iter
	const unsigned long updates = 100;				//how many updates per iter

	const std::string outx = "tog-x.txt";
	const std::string outU = "tog-uni.txt";
	const std::string outb1 = "tog-b1.txt";
	const std::string outb2 = "tog-b2.txt";
	const std::string outb3 = "tog-b3.txt";
	
	const bool doX = true;
	const bool doUni = true;
	const bool dob1 = true;
	const bool dob2 = true;
	const bool dob3 = true;

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

			double totRate = 0.;

			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<elementTOG*> element;
				TOG tog(1.,min,2.);
				for (unsigned long n = 0; n < N-1; n++){
					//x
					const double m = max*max-min*min;
					const double q = min*min;
					double r = std::sqrt(uniform(rng)*m+q);
					element.emplace_back(tog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}

				tog.update();
				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tog.sampleElement()->payload;
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
					elementTOG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					tog.updateElement(randomPointer,r);
					tog.update();
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
			double totRate = 0.;

			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<elementTOG*> element;
				TOG tog(1.,min,2.);
				for (unsigned long n = 0; n < N-1; n++){
					//uniform
					double r = uniform(rng)*(max-min)+min;
					element.emplace_back(tog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}



				tog.update();
				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tog.sampleElement()->payload;
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
					elementTOG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					tog.updateElement(randomPointer,r);
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
			const double logMin = std::log(min);
			const double logMax = std::log(max);
			//std::pow(10.,RExp) * samples;

			double totRate = 0.;

			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<elementTOG*> element;
				TOG tog(1.,min,2.);
				for (unsigned long n = 0; n < N-1; n++){
					//beta = 1
					double rExp = uniform(rng)*(logMax-logMin)+logMin;
					double r = std::exp(rExp);
					element.emplace_back(tog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}



				tog.update();
				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tog.sampleElement()->payload;
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
					elementTOG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					tog.updateElement(randomPointer,r);
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
			const double fact1 = max*min;

			//std::pow(10.,RExp) * samples;

			double totRate = 0.;

			for (unsigned long i = 0; i < runs; i++){
				std::vector<double> generated,sampled;
				std::vector<elementTOG*> element;
				TOG tog(1.,min,2.);
				for (unsigned long n = 0; n < N-1; n++){
					double r = 1e99;
					while (r < min or r > max){
						double rand = uniform(rng);
						r = fact1/(max-max*rand+min*r);
					}
					
					element.emplace_back(tog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}



				tog.update();
				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tog.sampleElement()->payload;
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
					elementTOG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					tog.updateElement(randomPointer,r);
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
				std::vector<elementTOG*> element;
				TOG tog(1.,min,2.);
				for (unsigned long n = 0; n < N-1; n++){
					double r = 1e99;
					while (r>max or r<min){
						double rand = uniform(rng); 
						r = std::sqrt(factor1/(-max2*rand+min2*rand+max2));
					}
					element.emplace_back(tog.addElement(n,r));
					generated.emplace_back(r);
					sampled.emplace_back(0.);
					totRate += r;
				}



				tog.update();
				for (unsigned long s = 0; s < sampleHere; s++){
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					unsigned long res = tog.sampleElement()->payload;
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
					elementTOG * randomPointer = element[randomId];
					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					tog.updateElement(randomPointer,r);
					std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(
							t2 - t1);
					updateTime.Push(time_span.count());
				}

				dof += 1;
			}

			dof -= 1;

			resultsb3 << N << "\t" << min << "\t" << sampleTime.Mean() << "\t" << sampleTime.StandardDeviation()/std::sqrt(dof)
			          <<"\t"<< updateTime.Mean()<<"\t"<<updateTime.StandardDeviation()/std::sqrt(dof) << "\t"<< chiSquared << "\t"<<dof<< "\t"<<(chiSquared-dof)/std::sqrt(2*dof)<< std::endl;

			std::cout<<"! ";

		}



	}

	resultsb3.close();
	}

	return 0;
}