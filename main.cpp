#include "CascadeOfGroups.h"
#include "TOG.h"
#include "AcceptanceRejection.h"
#include <fstream>
#include <iostream>
#include <vector>

const int maxRepetitions = 1e4;
const bool exponential = true;

const bool doNested = true;
const bool doSSAR = true;
const bool doSSACR = false;
bool doTree = true; //it will stop doing tree when it's not useful anymore
const bool doAccuracy = true;

const double R = 2.5;

const bool debugRate = false;

const int minlogN = .5;
const int maxlogN = 7;
const double deltalogN = .06;
const int logSwitch = 8;

const double minlogratio = -5;
const double maxlogratio = -.1;
const double deltalogratio = .05;

//extract output
std::ofstream nestedExtract;
std::ofstream ssarExtract;
std::ofstream ssacrExtract;
std::ofstream treeExtract;

//update output
std::ofstream nestedUpdate;
std::ofstream ssarUpdate;
std::ofstream ssacrUpdate;
std::ofstream treeUpdate;

//arbitrary update output
std::ofstream nestedArbUpdate;
std::ofstream ssarArbUpdate;
std::ofstream ssacrArbUpdate;
std::ofstream treeArbUpdate;

//accuracy output
std::ofstream nestedAccuracy;
std::ofstream ssarAccuracy;
std::ofstream ssacrAccuracy;
std::ofstream treeAccuracy;

//randNumbers
std::ofstream nestedRandNumbers;
std::ofstream ssarRandNumbers;
std::ofstream ssacrRandNumbers;

//ratios
std::ofstream ratioExtract;
std::ofstream ratioUpdate;
std::ofstream ratioArbUpdate;
std::ofstream ratioAccuracy;
std::ofstream ratioRandNumbers;

//iterations output
//std::ofstream nestedIterations("nestedIterations.txt", std::ofstream::out | std::ofstream::trunc);

//deep output
//std::ofstream deepIterations("deepIterations.txt", std::ofstream::out | std::ofstream::trunc);

int main() {

	//k
	if (doSSAR && doTree && doNested) {
		ratioExtract.open("threeway/ratioExtract.txt", std::ofstream::out | std::ofstream::trunc);
		ratioUpdate.open("threeway/ratioUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		ratioArbUpdate.open("threeway/ratioArbUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		if (doAccuracy) ratioAccuracy.open("threeway/ratioAccuracy.txt", std::ofstream::out | std::ofstream::trunc);
		ratioRandNumbers.open("threeway/ratioRandNumbers.txt", std::ofstream::out | std::ofstream::trunc);
	} else if (doSSAR && doNested && !doTree) {
		ratioExtract.open("twoway/ratioExtract.txt", std::ofstream::out | std::ofstream::trunc);
		ratioUpdate.open("twoway/ratioUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		ratioArbUpdate.open("twoway/ratioArbUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		if (doAccuracy) ratioAccuracy.open("twoway/ratioAccuracy.txt", std::ofstream::out | std::ofstream::trunc);
		ratioRandNumbers.open("twoway/ratioRandNumbers.txt", std::ofstream::out | std::ofstream::trunc);
	} else {
		ratioExtract.open("else/ratioExtract.txt", std::ofstream::out | std::ofstream::trunc);
		ratioUpdate.open("else/ratioUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		ratioArbUpdate.open("else/ratioArbUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		if (doAccuracy) ratioAccuracy.open("else/ratioAccuracy.txt", std::ofstream::out | std::ofstream::trunc);
		ratioRandNumbers.open("else/ratioRandNumbers.txt", std::ofstream::out | std::ofstream::trunc);
	}

	std::mt19937 rng(std::time(0));
	std::uniform_real_distribution<double> *random;

	if (doNested) {
		nestedExtract.open("nestedExtract.txt", std::ofstream::out | std::ofstream::trunc);
		nestedUpdate.open("nestedUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		nestedArbUpdate.open("nestedArbUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		if (doAccuracy) nestedAccuracy.open("nestedAccuracy.txt", std::ofstream::out | std::ofstream::trunc);
		nestedRandNumbers.open("nestedRandNumbers.txt", std::ofstream::out | std::ofstream::trunc);
	}

	if (doSSAR) {
		ssarExtract.open("ssarExtract.txt", std::ofstream::out | std::ofstream::trunc);
		ssarUpdate.open("ssarUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		ssarArbUpdate.open("ssarArbUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		if (doAccuracy) ssarAccuracy.open("ssarAccuracy.txt", std::ofstream::out | std::ofstream::trunc);
		ssarRandNumbers.open("ssarRandNumbers.txt", std::ofstream::out | std::ofstream::trunc);
	}

	if (doSSACR) {
		ssacrExtract.open("ssacrExtract.txt", std::ofstream::out | std::ofstream::trunc);
		ssacrUpdate.open("ssacrUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		ssacrArbUpdate.open("ssacrArbUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		if (doAccuracy) ssacrAccuracy.open("ssacrAccuracy.txt", std::ofstream::out | std::ofstream::trunc);
		ssacrRandNumbers.open("ssacrRandNumbers.txt", std::ofstream::out | std::ofstream::trunc);
	}

	if (doTree) {
		treeExtract.open("treeExtract.txt", std::ofstream::out | std::ofstream::trunc);
		treeUpdate.open("treeUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		treeArbUpdate.open("treeArbUpdate.txt", std::ofstream::out | std::ofstream::trunc);
		if (doAccuracy) treeAccuracy.open("treeAccuracy.txt", std::ofstream::out | std::ofstream::trunc);
	}

	for (double logN = minlogN; logN < maxlogN; logN += deltalogN) {
		bool large = false;
		//if (logN > logSwitch) large = true;

		long long int N = (long long int) std::floor(pow(10, logN));
		//if (N > 316227) large = true;


		std::cout << "N = " << N;
		if (large) std::cout << " (LARGE)" << std::endl;
		else std::cout << std::endl;

		if (!large) {
			for (double logratio = minlogratio; logratio < maxlogratio; logratio += deltalogratio) {
				double ratio = pow(10, logratio);

				if (debugRate) std::cout << "Ratio: " << ratio << std::endl;

				if (exponential) {
					double logMinRate = 0;
					double logMaxRate = -log10(ratio);
					random = new std::uniform_real_distribution<double>(logMinRate, logMaxRate);
				}

				if (!exponential) random = new std::uniform_real_distribution<double>(ratio, 1);
				std::uniform_real_distribution<double> randExt(0, 1);

				std::vector<double> rates;
				double sum = 0;

				rates.push_back(pow(10, minlogratio));
				rates.push_back(1);

				for (int i = 0; i < N - 2; i++) {
					double rand;
					if (exponential) rand = exp(-random->operator()(rng));
					if (!exponential) rand = random->operator()(rng);
					sum += rand;
					rates.push_back(rand);
				}

				std::vector<item *> nestedItems;
				std::vector<itemSSAR *> SSARItems;
				std::vector<elementTOG *> SSACRItems;
				std::vector<CBTNode *> treeItems;

				//CascadeOfGroups init
				CascadeOfGroups nest(pow(10, minlogratio), 1, N, R);
				if (doNested) for (int i = 0; i < N; i++) nestedItems.push_back(nest.addItem(i, rates.at(i)));

				//AcceptanceRejection init
				AcceptanceRejection ssar;
				if (doSSAR) for (int i = 0; i < N; i++) SSARItems.push_back(ssar.addMove(rates.at(i)));

				//TOG init
				TOG ssacr(1, pow(10, minlogratio), R);
				if (doSSACR) for (int i = 0; i < N; i++) SSACRItems.push_back(ssacr.addMove(i, rates.at(i)));
				if (doSSACR) ssacr.update();

				//Tree init
				CompleteBinaryTree tree;
				if (doTree) {
					for (int i = 0; i < N; i++) {
						treeItems.push_back(tree.addLeaf(i, rates.at(i)));
					}

				}


				std::vector<double> frequencyNested;
				std::vector<double> frequencySSACR;
				std::vector<double> frequencySSAR;
				std::vector<double> frequencyTree;

				for (int i = 0; i < N; i++) {
					frequencyNested.push_back(0);
					frequencySSACR.push_back(0);
					frequencySSAR.push_back(0);
					frequencyTree.push_back(0);
				}

				double extractions = 0;

				RunningStats extractionNested;
				RunningStats updateNested;
				RunningStats randNested;
				RunningStats arbupdateNested;



				//RunningStats iterationNested;
				//RunningStats deepNested;

				RunningStats extractionSSACR;
				RunningStats updateSSACR;
				RunningStats randSSACR;
				RunningStats arbupdateSSACR;

				RunningStats extractionSSAR;
				RunningStats updateSSAR;
				RunningStats randSSAR;
				RunningStats arbupdateSSAR;

				RunningStats extractionTree;
				RunningStats updateTree;
				RunningStats arbupdateTree;

				if (debugRate) tree.print();

				for (int t = 0; t < maxRepetitions; t++) {
					item *it;
					//nest.extractItem(random, deepNested, iterationNested);

					int countExtract = 0;
					int countRand = 0;

					if (doNested) {
						it = nest.extractItem(countExtract, countRand);
						extractionNested.Push(countExtract);
						randNested.Push(countRand);
						frequencyNested[it->payload]++;
					}

					if (doSSAR) {
						itemSSAR *iit;
						countExtract = 0;
						countRand = 0;
						iit = ssar.getMove(countExtract, countRand);
						extractionSSAR.Push(countExtract);
						randSSAR.Push(countRand);
						frequencySSAR[iit->payload]++;
					}

					if (doSSACR) {
						elementTOG *m;
						countExtract = 0;
						countRand = 0;
						m = ssacr.sampleElement(countExtract, countRand);
						extractionSSACR.Push(countExtract);
						randSSACR.Push(countRand);
						frequencySSACR[m->payload]++;
					}

					if (doTree) {
						CBTNode *node;
						countExtract = 0;
						node = tree.sampleLeaf(countExtract);
						frequencyTree[node->payload]++;
						extractionTree.Push(countExtract);
					}

					extractions++;
				}

				double chiSquaredNested = 0;
				double chiSquaredSSACR = 0;
				double chiSquaredSSAR = 0;
				double chiSquaredTree = 0;

				if (doAccuracy) {
					for (int i = 0; i < N; i++) {
						double realFreq = rates[i] / sum;

						if (doNested) {
							double measuredFreqNested = frequencyNested[i] / extractions;
							if (realFreq > 0) chiSquaredNested += pow(measuredFreqNested - realFreq, 2) / realFreq;
							else chiSquaredNested += 0;
						}

						if (doSSAR) {
							double measuredFreqSSAR = frequencySSAR[i] / extractions;
							if (realFreq > 0) chiSquaredSSAR += pow(measuredFreqSSAR - realFreq, 2) / realFreq;
							else chiSquaredSSAR += 0;
						}

						if (doSSACR) {
							double measuredFreqSSACR = frequencySSACR[i] / extractions;
							if (realFreq > 0) chiSquaredSSACR += pow(measuredFreqSSACR - realFreq, 2) / realFreq;
							else chiSquaredSSACR += 0;
						}

						if (doTree) {
							double measuredFreqTree = frequencyTree[i] / extractions;
							if (realFreq > 0) chiSquaredTree += pow(measuredFreqTree - realFreq, 2) / realFreq;
							else chiSquaredTree += 0;
						}
					}
				}



				for (int t = 0; t < maxRepetitions; t++) {
					item *it;
					double randomRate;
					if (exponential) randomRate = exp(-random->operator()(rng));
					if (!exponential) randomRate = random->operator()(rng);

					int countUpdate = 0;

					if (doNested) {
						it = nest.extractItem();
						nest.updateItem(it, randomRate, countUpdate);
						updateNested.Push(countUpdate);
					}
					if (doSSAR) {
						itemSSAR *iit;
						iit = ssar.getMove();
						countUpdate = 0;
						ssar.updateMove(iit, randomRate, countUpdate);
						updateSSAR.Push(countUpdate);
					}
					if (doSSACR) {
						elementTOG *m;
						m = ssacr.sampleElement();
						countUpdate = 0;
						ssacr.updateMove(m, randomRate, countUpdate);
						ssacr.update(countUpdate);
						updateSSACR.Push(countUpdate);
					}
					if (doTree) {
						CBTNode *itt = tree.sampleLeaf();
						countUpdate = 0;
						tree.updateLeaf(itt, randomRate, countUpdate);
						tree.updateTree(countUpdate);
						updateTree.Push(countUpdate);
					}


				}

				//arbitrary update
				for (int t = 0; t < maxRepetitions; t++) {
					double randomRate;
					if (exponential) randomRate = exp(-random->operator()(rng));
					if (!exponential) randomRate = random->operator()(rng);

					int randIndex = (int) std::floor(randExt(rng) * (double) N);

					int countUpdate = 0;

					if (doNested) {
						nest.updateItem(nestedItems[randIndex], randomRate, countUpdate);
						arbupdateNested.Push(countUpdate);
					}
					if (doSSAR) {
						countUpdate = 0;
						ssar.updateMove(SSARItems[randIndex], randomRate, countUpdate);
						arbupdateSSAR.Push(countUpdate);
					}
					if (doSSACR) {
						countUpdate = 0;
						ssacr.updateMove(SSACRItems[randIndex], randomRate, countUpdate);
						ssacr.update(countUpdate);
						arbupdateSSACR.Push(countUpdate);
					}
					if (doTree) {
						countUpdate = 0;
						tree.updateLeaf(treeItems[randIndex], randomRate, countUpdate);
						tree.updateTree(countUpdate);
						arbupdateTree.Push(countUpdate);
					}

				}

				if (doTree) {
					if (doAccuracy) treeAccuracy << N << "\t" << ratio << "\t" << chiSquaredTree << "\t" << std::endl;
					treeExtract << N << "\t" << ratio << "\t" << extractionTree.Mean() << "\t"
					            << extractionTree.StandardDeviation() / sqrt(extractionTree.NumDataValues()) << "\t"
					            << std::endl;

					treeUpdate << N << "\t" << ratio << "\t" << updateTree.Mean() << "\t"
					           << updateTree.StandardDeviation() / sqrt(updateTree.NumDataValues()) << "\t"
					           << std::endl;

					treeArbUpdate << N << "\t" << ratio << "\t" << arbupdateTree.Mean() << "\t"
					              << arbupdateTree.StandardDeviation() / sqrt(arbupdateTree.NumDataValues())
					              << "\t"
					              << std::endl;
				}

				if (doNested) {
					if (doAccuracy)
						nestedAccuracy << N << "\t" << ratio << "\t" << chiSquaredNested << "\t" << std::endl;

					nestedExtract << N << "\t" << ratio << "\t" << extractionNested.Mean() << "\t"
					              << extractionNested.StandardDeviation() / sqrt(extractionNested.NumDataValues())
					              << "\t"
					              << std::endl;

					nestedUpdate << N << "\t" << ratio << "\t" << updateNested.Mean() << "\t"
					             << updateNested.StandardDeviation() / sqrt(updateNested.NumDataValues()) << "\t"
					             << std::endl;

					nestedArbUpdate << N << "\t" << ratio << "\t" << arbupdateNested.Mean() << "\t"
					                << arbupdateNested.StandardDeviation() / sqrt(arbupdateNested.NumDataValues())
					                << "\t"
					                << std::endl;
					nestedRandNumbers << N << "\t" << ratio << "\t" << randNested.Mean() << "\t"
					                  << randNested.StandardDeviation() / sqrt(randNested.NumDataValues()) << "\t"
					                  << std::endl;
				}

				if (doSSAR) {
					if (doAccuracy) ssarAccuracy << N << "\t" << ratio << "\t" << chiSquaredSSAR << "\t" << std::endl;;

					ssarExtract << N << "\t" << ratio << "\t" << extractionSSAR.Mean() << "\t"
					            << extractionSSAR.StandardDeviation() / sqrt(extractionSSAR.NumDataValues()) << "\t"
					            << std::endl;

					ssarUpdate << N << "\t" << ratio << "\t" << updateSSAR.Mean() << "\t"
					           << updateSSAR.StandardDeviation() / sqrt(updateSSAR.NumDataValues()) << "\t"
					           << std::endl;

					ssarArbUpdate << N << "\t" << ratio << "\t" << arbupdateSSAR.Mean() << "\t"
					              << arbupdateSSAR.StandardDeviation() / sqrt(arbupdateSSAR.NumDataValues()) << "\t"
					              << std::endl;
					ssarRandNumbers << N << "\t" << ratio << "\t" << randSSAR.Mean() << "\t"
					                << randSSAR.StandardDeviation() / sqrt(randSSAR.NumDataValues()) << "\t"
					                << std::endl;
				}

				if (doSSACR) {
					if (doAccuracy) ssacrAccuracy << N << "\t" << ratio << "\t" << chiSquaredSSACR << "\t" << std::endl;

					ssacrExtract << N << "\t" << ratio << "\t" << extractionSSACR.Mean() << "\t"
					             << extractionSSACR.StandardDeviation() / sqrt(extractionSSACR.NumDataValues()) << "\t"
					             << std::endl;
					ssacrUpdate << N << "\t" << ratio << "\t" << updateSSACR.Mean() << "\t"
					            << updateSSACR.StandardDeviation() / sqrt(updateSSACR.NumDataValues()) << "\t"
					            << std::endl;

					ssacrArbUpdate << N << "\t" << ratio << "\t" << arbupdateSSACR.Mean() << "\t"
					               << arbupdateSSACR.StandardDeviation() / sqrt(arbupdateSSACR.NumDataValues()) << "\t"
					               << std::endl;
					ssacrRandNumbers << N << "\t" << ratio << "\t" << randSSACR.Mean() << "\t"
					                 << randSSACR.StandardDeviation() / sqrt(randSSACR.NumDataValues()) << "\t"
					                 << std::endl;
				}

				if (doSSAR * doNested && !doTree) {
					ratioExtract << N << "\t" << ratio << "\t";
					if (extractionNested.Mean() > extractionSSAR.Mean()) ratioExtract << 1;
					else ratioExtract << 2;

					ratioExtract << "\t" << extractionNested.Mean() / extractionSSAR.Mean()
					             << "\t"
					             << std::sqrt(std::pow(
							             extractionNested.StandardDeviation() / sqrt(extractionNested.NumDataValues()),
							             2) +
					                          std::pow(extractionNested.StandardDeviation() /
					                                   sqrt(extractionNested.NumDataValues()), 2)) << "\t"
					             << std::endl;

					ratioUpdate << N << "\t" << ratio << "\t";
					if (updateNested.Mean() > updateSSAR.Mean()) ratioUpdate << 1;
					else ratioUpdate << 2;

					ratioUpdate << "\t" << updateNested.Mean() / updateSSAR.Mean() << "\t"
					            << std::sqrt(
							            std::pow(updateNested.StandardDeviation() / sqrt(updateNested.NumDataValues()),
							                     2) +
							            std::pow(updateNested.StandardDeviation() / sqrt(updateNested.NumDataValues()),
							                     2))
					            << "\t"
					            << std::endl;

					ratioArbUpdate << N << "\t" << ratio << "\t";
					if (arbupdateNested.Mean() > arbupdateSSAR.Mean()) ratioArbUpdate << 1;
					else ratioArbUpdate << 2;

					ratioArbUpdate << "\t" << arbupdateNested.Mean() / arbupdateSSAR.Mean()
					               << "\t"
					               << std::sqrt(std::pow(
							               arbupdateNested.StandardDeviation() / sqrt(arbupdateNested.NumDataValues()),
							               2) +
					                            std::pow(arbupdateNested.StandardDeviation() /
					                                     sqrt(arbupdateNested.NumDataValues()), 2)) << "\t"
					               << std::endl;

					ratioRandNumbers << N << "\t" << ratio << "\t";
					if (randNested.Mean() > randSSAR.Mean()) ratioRandNumbers << 1;
					else ratioRandNumbers << 2;

					ratioRandNumbers << "\t" << randNested.Mean() / randSSAR.Mean() << "\t"
					                 << std::sqrt(
							                 std::pow(randNested.StandardDeviation() / sqrt(randNested.NumDataValues()),
							                          2) +
							                 std::pow(randNested.StandardDeviation() / sqrt(randNested.NumDataValues()),
							                          2))
					                 << "\t"
					                 << std::endl;

					if (doAccuracy) {
						ratioAccuracy << N << "\t" << ratio << "\t";
						if (chiSquaredNested > chiSquaredSSAR) ratioAccuracy << 1;
						else ratioAccuracy << 2;
						ratioAccuracy << "\t" << chiSquaredNested / chiSquaredSSAR << std::endl;
					}

				}

				if (doSSAR * doNested * doTree) {

					bool isTreeEverFaster = false;

					// r - minR - which is faster - k Nested/AcceptanceRejection - k AcceptanceRejection/Tree - k Nested/Tree

					ratioExtract << N << "\t" << ratio << "\t";
					if (extractionNested.Mean() < extractionTree.Mean() &&
					    extractionNested.Mean() < extractionSSAR.Mean())
						ratioExtract << 2 << "\t";
					if (extractionNested.Mean() > extractionTree.Mean() &&
					    extractionTree.Mean() < extractionSSAR.Mean()) {
						ratioExtract << 0 << "\t";
						isTreeEverFaster = true;
					}

					if (extractionSSAR.Mean() < extractionTree.Mean() &&
					    extractionNested.Mean() > extractionSSAR.Mean())
						ratioExtract << 1 << "\t";
					ratioExtract << extractionNested.Mean() / extractionSSAR.Mean() << "\t"
					             << extractionSSAR.Mean() / extractionTree.Mean() << "\t"
					             << extractionNested.Mean() / extractionTree.Mean();
					ratioExtract << std::endl;

					ratioUpdate << N << "\t" << ratio << "\t";
					if (updateNested.Mean() < updateTree.Mean() && updateNested.Mean() < updateSSAR.Mean())
						ratioUpdate << 2 << "\t";
					if (updateNested.Mean() > updateTree.Mean() && updateTree.Mean() < updateSSAR.Mean()) {
						ratioUpdate << 0 << "\t";
						isTreeEverFaster = true;
					}

					if (updateSSAR.Mean() < updateTree.Mean() && updateNested.Mean() > updateSSAR.Mean())
						ratioUpdate << 1 << "\t";
					ratioUpdate << updateNested.Mean() / updateSSAR.Mean() << "\t"
					            << updateSSAR.Mean() / updateTree.Mean()
					            << "\t" << updateNested.Mean() / updateTree.Mean();
					ratioUpdate << std::endl;

					ratioArbUpdate << N << "\t" << ratio << "\t";
					if (arbupdateNested.Mean() < arbupdateTree.Mean() && arbupdateNested.Mean() < arbupdateSSAR.Mean())
						ratioArbUpdate << 2 << "\t";
					if (arbupdateNested.Mean() > arbupdateTree.Mean() && arbupdateTree.Mean() < arbupdateSSAR.Mean()) {
						ratioArbUpdate << 0 << "\t";
						isTreeEverFaster = true;
					}

					if (arbupdateSSAR.Mean() < arbupdateTree.Mean() && arbupdateNested.Mean() > arbupdateSSAR.Mean())
						ratioArbUpdate << 1 << "\t";
					ratioArbUpdate << arbupdateNested.Mean() / arbupdateSSAR.Mean() << "\t"
					               << arbupdateSSAR.Mean() / arbupdateTree.Mean() << "\t"
					               << arbupdateNested.Mean() / arbupdateTree.Mean();
					ratioArbUpdate << std::endl;

					ratioRandNumbers << N << "\t" << ratio << "\t";
					if (randNested.Mean() < 1. && randNested.Mean() < randSSAR.Mean()) ratioRandNumbers << 2 << "\t";
					if (randNested.Mean() > 1. && 1. < randSSAR.Mean()) ratioRandNumbers << 0 << "\t";
					if (randSSAR.Mean() < 1. && randNested.Mean() > randSSAR.Mean()) ratioRandNumbers << 1 << "\t";
					ratioRandNumbers << randNested.Mean() / randSSAR.Mean() << "\t" << randSSAR.Mean() / 1. << "\t"
					                 << randNested.Mean() / 1.;
					ratioRandNumbers << std::endl;

					if (doAccuracy) {
						ratioAccuracy << N << "\t" << ratio << "\t";
						if (chiSquaredNested > chiSquaredSSAR && chiSquaredTree > chiSquaredSSAR) ratioAccuracy << 1;
						else if (chiSquaredNested < chiSquaredSSAR && chiSquaredTree > chiSquaredNested)
							ratioAccuracy << 2;
						else ratioAccuracy << 0;
						ratioAccuracy << "\t" << chiSquaredNested / chiSquaredSSAR << "\t"
						              << chiSquaredSSAR / chiSquaredTree << "\t" << chiSquaredNested / chiSquaredTree
						              << std::endl;

					}

					/*if (isTreeEverFaster == false) {
						doTree = false;
						std::cout << "Stop doing tree" << std::endl;
					}*/
				}

			}
		}

		if (large) {
			for (double logratio = minlogratio; logratio < maxlogratio; logratio += deltalogratio) {
				double ratio = pow(10, logratio);

				if (debugRate) std::cout << "Ratio: " << ratio << std::endl;


				if (exponential) {
					double logMinRate = 0;
					double logMaxRate = -log10(ratio);
					random = new std::uniform_real_distribution<double>(logMinRate, logMaxRate);
				}

				if (!exponential) random = new std::uniform_real_distribution<double>(ratio, 1);
				std::uniform_real_distribution<double> randExt(0, 1);

				std::vector<double> rates;
				double sum = 0;

				rates.push_back(pow(10, minlogratio));
				rates.push_back(1);

				for (int i = 0; i < N - 2; i++) {
					double rand;
					if (exponential) rand = exp(-random->operator()(rng));
					if (!exponential) rand = random->operator()(rng);
					sum += rand;
					rates.push_back(rand);
				}

				std::vector<item *> nestedItems;
				std::vector<itemSSAR *> SSARLargeItems;
				std::vector<elementTOG *> SSACRLargeItems;
				std::vector<CBTNode *> treeItems;

				//NestedGroupsLarge init
				NestedGroupsLarge nest(pow(10, minlogratio), 1, N, R);
				if (doNested) for (int i = 0; i < N; i++) nestedItems.push_back(nest.addItem(i, rates.at(i)));


				//SSACRLarge init
				SSACRLarge ssacr(1, pow(10, minlogratio), R);
				if (doSSACR) for (int i = 0; i < N; i++) SSACRLargeItems.push_back(ssacr.addMove(i, rates.at(i)));
				if (doSSACR) ssacr.update();

				//Tree init
				DynamicFlatTreeLarge tree;
				if (doTree) {
					for (int i = 0; i < N; i++) treeItems.push_back(tree.addLeaf(i, rates.at(i)));
					tree.updateTree();
				}

				std::vector<double> frequencyNested;
				std::vector<double> frequencySSACRLarge;
				std::vector<double> frequencySSARLarge;
				std::vector<double> frequencyTree;

				for (int i = 0; i < N; i++) {
					frequencyNested.push_back(0);
					frequencySSACRLarge.push_back(0);
					frequencySSARLarge.push_back(0);
					frequencyTree.push_back(0);
				}

				double extractions = 0;

				RunningStats extractionNested;
				RunningStats updateNested;
				RunningStats randNested;
				RunningStats arbupdateNested;

				//RunningStats iterationNested;
				//RunningStats deepNested;

				RunningStats extractionSSACRLarge;
				RunningStats updateSSACRLarge;
				RunningStats randSSACRLarge;
				RunningStats arbupdateSSACRLarge;

				RunningStats extractionSSARLarge;
				RunningStats updateSSARLarge;
				RunningStats randSSARLarge;
				RunningStats arbupdateSSARLarge;

				RunningStats extractionTree;
				RunningStats updateTree;
				RunningStats arbupdateTree;


				for (int t = 0; t < maxRepetitions; t++) {
					item *it;
					//nest.extractItem(random, deepNested, iterationNested);

					int countExtract = 0;
					int countRand = 0;

					if (doNested) {
						it = nest.extractItem(countExtract, countRand);
						extractionNested.Push(countExtract);
						randNested.Push(countRand);
						frequencyNested[it->payload]++;
					}

					if (doSSAR) {
						itemSSAR *iit;
						countExtract = 0;
						countRand = 0;
						iit = ssar.sampleElement(countExtract, countRand);
						extractionSSARLarge.Push(countExtract);
						randSSARLarge.Push(countRand);
						frequencySSARLarge[iit->payload]++;
					}

					if (doSSACR) {
						elementTOG *m;
						countExtract = 0;
						countRand = 0;
						m = ssacr.getMove(countExtract, countRand);
						extractionSSACRLarge.Push(countExtract);
						randSSACRLarge.Push(countRand);
						frequencySSACRLarge[m->payload]++;
					}

					if (doTree) {
						CBTNode *node;
						countExtract = 0;
						node = tree.sampleLeaf(countExtract);
						frequencyTree[node->payload]++;
						extractionTree.Push(countExtract);
					}

					extractions++;
				}

				double chiSquaredNested = 0;
				double chiSquaredSSACRLarge = 0;
				double chiSquaredSSARLarge = 0;
				double chiSquaredTree = 0;

				if (doAccuracy) {
					for (int i = 0; i < N; i++) {
						double realFreq = rates[i] / sum;

						if (doNested) {
							double measuredFreqNested = frequencyNested[i] / extractions;
							if (realFreq > 0) chiSquaredNested += pow(measuredFreqNested - realFreq, 2) / realFreq;
							else chiSquaredNested += 0;
						}

						if (doSSAR) {
							double measuredFreqSSARLarge = frequencySSARLarge[i] / extractions;
							if (realFreq > 0)
								chiSquaredSSARLarge += pow(measuredFreqSSARLarge - realFreq, 2) / realFreq;
							else chiSquaredSSARLarge += 0;
						}

						if (doSSACR) {
							double measuredFreqSSACRLarge = frequencySSACRLarge[i] / extractions;
							if (realFreq > 0)
								chiSquaredSSACRLarge += pow(measuredFreqSSACRLarge - realFreq, 2) / realFreq;
							else chiSquaredSSACRLarge += 0;
						}

						if (doTree) {
							double measuredFreqTree = frequencyTree[i] / extractions;
							if (realFreq > 0) chiSquaredTree += pow(measuredFreqTree - realFreq, 2) / realFreq;
							else chiSquaredTree += 0;
						}
					}

				}

				for (int t = 0; t < maxRepetitions; t++) {
					item *it;
					double randomRate;
					if (exponential) randomRate = exp(-random->operator()(rng));
					if (!exponential) randomRate = random->operator()(rng);

					int countUpdate = 0;

					if (doNested) {
						it = nest.extractItem();
						nest.updateItem(it, randomRate, countUpdate);
						updateNested.Push(countUpdate);
					}
					if (doSSAR) {
						itemSSAR *iit;
						iit = ssar.sampleElement();
						countUpdate = 0;
						ssar.updateMove(iit, randomRate, countUpdate);
						updateSSARLarge.Push(countUpdate);
					}
					if (doSSACR) {
						elementTOG *m;
						m = ssacr.getMove();
						countUpdate = 0;
						ssacr.updateMove(m, randomRate, countUpdate);
						ssacr.update(countUpdate);
						updateSSACRLarge.Push(countUpdate);
					}
					if (doTree) {
						CBTNode *itt = tree.sampleLeaf();
						countUpdate = 0;
						tree.updateLeaf(itt, randomRate, countUpdate);
						tree.updateTree(countUpdate);
						updateTree.Push(countUpdate);
					}


				}

				//arbitrary update
				for (int t = 0; t < maxRepetitions; t++) {
					double randomRate;
					if (exponential) randomRate = exp(-random->operator()(rng));
					if (!exponential) randomRate = random->operator()(rng);

					int randIndex = (int) std::floor(randExt(rng) * (double) N);

					int countUpdate = 0;

					if (doNested) {
						nest.updateItem(nestedItems[randIndex], randomRate, countUpdate);
						arbupdateNested.Push(countUpdate);
					}
					if (doSSAR) {
						countUpdate = 0;
						ssar.updateMove(SSARLargeItems[randIndex], randomRate, countUpdate);
						arbupdateSSARLarge.Push(countUpdate);
					}
					if (doSSACR) {
						countUpdate = 0;
						ssacr.updateMove(SSACRLargeItems[randIndex], randomRate, countUpdate);
						ssacr.update(countUpdate);
						arbupdateSSACRLarge.Push(countUpdate);
					}
					if (doTree) {
						countUpdate = 0;
						tree.updateLeaf(treeItems[randIndex], randomRate, countUpdate);
						tree.updateTree(countUpdate);
						arbupdateTree.Push(countUpdate);
					}

				}

				if (doTree) {
					if (doAccuracy) treeAccuracy << N << "\t" << ratio << "\t" << chiSquaredTree << "\t" << std::endl;
					treeExtract << N << "\t" << ratio << "\t" << extractionTree.Mean() << "\t"
					            << extractionTree.StandardDeviation() / sqrt(extractionTree.NumDataValues()) << "\t"
					            << std::endl;

					treeUpdate << N << "\t" << ratio << "\t" << updateTree.Mean() << "\t"
					           << updateTree.StandardDeviation() / sqrt(updateTree.NumDataValues()) << "\t"
					           << std::endl;

					treeArbUpdate << N << "\t" << ratio << "\t" << arbupdateTree.Mean() << "\t"
					              << arbupdateTree.StandardDeviation() / sqrt(arbupdateTree.NumDataValues())
					              << "\t"
					              << std::endl;
				}

				if (doNested) {
					if (doAccuracy)
						nestedAccuracy << N << "\t" << ratio << "\t" << chiSquaredNested << "\t" << std::endl;

					nestedExtract << N << "\t" << ratio << "\t" << extractionNested.Mean() << "\t"
					              << extractionNested.StandardDeviation() / sqrt(extractionNested.NumDataValues())
					              << "\t"
					              << std::endl;

					nestedUpdate << N << "\t" << ratio << "\t" << updateNested.Mean() << "\t"
					             << updateNested.StandardDeviation() / sqrt(updateNested.NumDataValues()) << "\t"
					             << std::endl;

					nestedArbUpdate << N << "\t" << ratio << "\t" << arbupdateNested.Mean() << "\t"
					                << arbupdateNested.StandardDeviation() / sqrt(arbupdateNested.NumDataValues())
					                << "\t"
					                << std::endl;
					nestedRandNumbers << N << "\t" << ratio << "\t" << randNested.Mean() << "\t"
					                  << randNested.StandardDeviation() / sqrt(randNested.NumDataValues()) << "\t"
					                  << std::endl;
				}

				if (doSSAR) {
					if (doAccuracy)
						ssarAccuracy << N << "\t" << ratio << "\t" << chiSquaredSSARLarge << "\t" << std::endl;;

					ssarExtract << N << "\t" << ratio << "\t" << extractionSSARLarge.Mean() << "\t"
					            << extractionSSARLarge.StandardDeviation() / sqrt(extractionSSARLarge.NumDataValues())
					            << "\t"
					            << std::endl;

					ssarUpdate << N << "\t" << ratio << "\t" << updateSSARLarge.Mean() << "\t"
					           << updateSSARLarge.StandardDeviation() / sqrt(updateSSARLarge.NumDataValues()) << "\t"
					           << std::endl;

					ssarArbUpdate << N << "\t" << ratio << "\t" << arbupdateSSARLarge.Mean() << "\t"
					              << arbupdateSSARLarge.StandardDeviation() / sqrt(arbupdateSSARLarge.NumDataValues())
					              << "\t"
					              << std::endl;
					ssarRandNumbers << N << "\t" << ratio << "\t" << randSSARLarge.Mean() << "\t"
					                << randSSARLarge.StandardDeviation() / sqrt(randSSARLarge.NumDataValues()) << "\t"
					                << std::endl;
				}

				if (doSSACR) {
					if (doAccuracy)
						ssacrAccuracy << N << "\t" << ratio << "\t" << chiSquaredSSACRLarge << "\t" << std::endl;

					ssacrExtract << N << "\t" << ratio << "\t" << extractionSSACRLarge.Mean() << "\t"
					             << extractionSSACRLarge.StandardDeviation() /
					                sqrt(extractionSSACRLarge.NumDataValues()) << "\t"
					             << std::endl;
					ssacrUpdate << N << "\t" << ratio << "\t" << updateSSACRLarge.Mean() << "\t"
					            << updateSSACRLarge.StandardDeviation() / sqrt(updateSSACRLarge.NumDataValues()) << "\t"
					            << std::endl;

					ssacrArbUpdate << N << "\t" << ratio << "\t" << arbupdateSSACRLarge.Mean() << "\t"
					               << arbupdateSSACRLarge.StandardDeviation() /
					                  sqrt(arbupdateSSACRLarge.NumDataValues()) << "\t"
					               << std::endl;
					ssacrRandNumbers << N << "\t" << ratio << "\t" << randSSACRLarge.Mean() << "\t"
					                 << randSSACRLarge.StandardDeviation() / sqrt(randSSACRLarge.NumDataValues())
					                 << "\t"
					                 << std::endl;
				}

				if (doSSAR * doNested && !doTree) {
					ratioExtract << N << "\t" << ratio << "\t";
					if (extractionNested.Mean() > extractionSSARLarge.Mean()) ratioExtract << 1;
					else ratioExtract << 2;

					ratioExtract << "\t" << extractionNested.Mean() / extractionSSARLarge.Mean()
					             << "\t"
					             << std::sqrt(std::pow(
							             extractionNested.StandardDeviation() / sqrt(extractionNested.NumDataValues()),
							             2) +
					                          std::pow(extractionNested.StandardDeviation() /
					                                   sqrt(extractionNested.NumDataValues()), 2)) << "\t"
					             << std::endl;

					ratioUpdate << N << "\t" << ratio << "\t";
					if (updateNested.Mean() > updateSSARLarge.Mean()) ratioUpdate << 1;
					else ratioUpdate << 2;

					ratioUpdate << "\t" << updateNested.Mean() / updateSSARLarge.Mean() << "\t"
					            << std::sqrt(
							            std::pow(updateNested.StandardDeviation() / sqrt(updateNested.NumDataValues()),
							                     2) +
							            std::pow(updateNested.StandardDeviation() / sqrt(updateNested.NumDataValues()),
							                     2))
					            << "\t"
					            << std::endl;

					ratioArbUpdate << N << "\t" << ratio << "\t";
					if (arbupdateNested.Mean() > arbupdateSSARLarge.Mean()) ratioArbUpdate << 1;
					else ratioArbUpdate << 2;

					ratioArbUpdate << "\t" << arbupdateNested.Mean() / arbupdateSSARLarge.Mean()
					               << "\t"
					               << std::sqrt(std::pow(
							               arbupdateNested.StandardDeviation() / sqrt(arbupdateNested.NumDataValues()),
							               2) +
					                            std::pow(arbupdateNested.StandardDeviation() /
					                                     sqrt(arbupdateNested.NumDataValues()), 2)) << "\t"
					               << std::endl;

					ratioRandNumbers << N << "\t" << ratio << "\t";
					if (randNested.Mean() > randSSARLarge.Mean()) ratioRandNumbers << 1;
					else ratioRandNumbers << 2;

					ratioRandNumbers << "\t" << randNested.Mean() / randSSARLarge.Mean() << "\t"
					                 << std::sqrt(
							                 std::pow(randNested.StandardDeviation() / sqrt(randNested.NumDataValues()),
							                          2) +
							                 std::pow(randNested.StandardDeviation() / sqrt(randNested.NumDataValues()),
							                          2))
					                 << "\t"
					                 << std::endl;

					if (doAccuracy) {
						ratioAccuracy << N << "\t" << ratio << "\t";
						if (chiSquaredNested > chiSquaredSSARLarge) ratioAccuracy << 1;
						else ratioAccuracy << 2;
						ratioAccuracy << "\t" << chiSquaredNested / chiSquaredSSARLarge << std::endl;
					}

				}

				if (doSSAR * doNested * doTree) {

					bool isTreeEverFaster = false;

					// r - minR - which is faster - k Nested/AcceptanceRejection - k AcceptanceRejection/Tree - k Nested/Tree

					ratioExtract << N << "\t" << ratio << "\t";
					if (extractionNested.Mean() < extractionTree.Mean() &&
					    extractionNested.Mean() < extractionSSARLarge.Mean())
						ratioExtract << 2 << "\t";
					if (extractionNested.Mean() > extractionTree.Mean() &&
					    extractionTree.Mean() < extractionSSARLarge.Mean()) {
						ratioExtract << 0 << "\t";
						isTreeEverFaster = true;
					}

					if (extractionSSARLarge.Mean() < extractionTree.Mean() &&
					    extractionNested.Mean() > extractionSSARLarge.Mean())
						ratioExtract << 1 << "\t";
					ratioExtract << extractionNested.Mean() / extractionSSARLarge.Mean() << "\t"
					             << extractionSSARLarge.Mean() / extractionTree.Mean() << "\t"
					             << extractionNested.Mean() / extractionTree.Mean();
					ratioExtract << std::endl;

					ratioUpdate << N << "\t" << ratio << "\t";
					if (updateNested.Mean() < updateTree.Mean() && updateNested.Mean() < updateSSARLarge.Mean())
						ratioUpdate << 2 << "\t";
					if (updateNested.Mean() > updateTree.Mean() && updateTree.Mean() < updateSSARLarge.Mean()) {
						ratioUpdate << 0 << "\t";
						isTreeEverFaster = true;
					}

					if (updateSSARLarge.Mean() < updateTree.Mean() && updateNested.Mean() > updateSSARLarge.Mean())
						ratioUpdate << 1 << "\t";
					ratioUpdate << updateNested.Mean() / updateSSARLarge.Mean() << "\t"
					            << updateSSARLarge.Mean() / updateTree.Mean()
					            << "\t" << updateNested.Mean() / updateTree.Mean();
					ratioUpdate << std::endl;

					ratioArbUpdate << N << "\t" << ratio << "\t";
					if (arbupdateNested.Mean() < arbupdateTree.Mean() &&
					    arbupdateNested.Mean() < arbupdateSSARLarge.Mean())
						ratioArbUpdate << 2 << "\t";
					if (arbupdateNested.Mean() > arbupdateTree.Mean() &&
					    arbupdateTree.Mean() < arbupdateSSARLarge.Mean()) {
						ratioArbUpdate << 0 << "\t";
						isTreeEverFaster = true;
					}

					if (arbupdateSSARLarge.Mean() < arbupdateTree.Mean() &&
					    arbupdateNested.Mean() > arbupdateSSARLarge.Mean())
						ratioArbUpdate << 1 << "\t";
					ratioArbUpdate << arbupdateNested.Mean() / arbupdateSSARLarge.Mean() << "\t"
					               << arbupdateSSARLarge.Mean() / arbupdateTree.Mean() << "\t"
					               << arbupdateNested.Mean() / arbupdateTree.Mean();
					ratioArbUpdate << std::endl;

					ratioRandNumbers << N << "\t" << ratio << "\t";
					if (randNested.Mean() < 1. && randNested.Mean() < randSSARLarge.Mean())
						ratioRandNumbers << 2 << "\t";
					if (randNested.Mean() > 1. && 1. < randSSARLarge.Mean()) ratioRandNumbers << 0 << "\t";
					if (randSSARLarge.Mean() < 1. && randNested.Mean() > randSSARLarge.Mean())
						ratioRandNumbers << 1 << "\t";
					ratioRandNumbers << randNested.Mean() / randSSARLarge.Mean() << "\t" << randSSARLarge.Mean() / 1.
					                 << "\t"
					                 << randNested.Mean() / 1.;
					ratioRandNumbers << std::endl;

					if (doAccuracy) {
						ratioAccuracy << N << "\t" << ratio << "\t";
						if (chiSquaredNested > chiSquaredSSARLarge && chiSquaredTree > chiSquaredSSARLarge)
							ratioAccuracy << 1;
						else if (chiSquaredNested < chiSquaredSSARLarge && chiSquaredTree > chiSquaredNested)
							ratioAccuracy << 2;
						else ratioAccuracy << 0;
						ratioAccuracy << "\t" << chiSquaredNested / chiSquaredSSARLarge << "\t"
						              << chiSquaredSSARLarge / chiSquaredTree << "\t"
						              << chiSquaredNested / chiSquaredTree << std::endl;

					}

					if (isTreeEverFaster == false) {
						doTree = false;
						std::cout << "Stop doing tree" << std::endl;
					}
				}

			}
		}
	}

	std::cout << "\a" << std::endl;
	return 0;
}