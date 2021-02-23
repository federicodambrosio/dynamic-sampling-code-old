//
// Created by federicodambrosio on 9-3-17.
//

#ifndef ACCEPTANCE_REJECTION
#define ACCEPTANCE_REJECTION

#include <random>

struct elementAR {
	int payload;                //unique event id
	int position;               //position in the event vector, useful for updates
	double rate;                //rate of the event

	elementAR() : position(-1), payload(-1), rate(0){}
	elementAR(int pl, int ps, double r) : payload(pl), position(ps), rate(r){}
	elementAR(int pl, int ps, double r, int & counter) : payload(pl), position(ps), rate(r){counter+=3;}
};

class AcceptanceRejection {                                 //Section 3.2
	std::mt19937 rng;
	double maxR=0;                                          //largest rate encountered by the data structure
	std::vector<elementAR*> items;                          //vector of events
	std::uniform_real_distribution<double> * randExt;

public:
	//init random number generation methods
	AcceptanceRejection(){
		rng.seed(time(nullptr));
		randExt = new std::uniform_real_distribution<double>(0,1);
	};
	//cleans up the events before deleting the structure
	~AcceptanceRejection(){
		for (auto & item : items) delete item;
	}
	//adds an event to the data structure
	elementAR* addMove(double rate);                        //payload = order of creation
	elementAR* addMove(elementAR * it);                     //adds a pre-created event
	elementAR* addMove(elementAR * it, int & counter);      //counter is increased by the number of operations required

	elementAR * getMove();                                  //returns an event according to rate
	elementAR * getMove(int & counter, int & randNumbers);  //increases counter by the number of operations and
															//randNumbers by the number of random numbers generated

	void updateMove(elementAR* i, double newRate);                  //updates the rate of an event
	void updateMove(elementAR* i, double newRate, int & counter);   //increases counter by the number of operations

};


#endif //ACCEPTANCE_REJECTION
