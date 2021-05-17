//
// The class AcceptanceRejection implements the Acceptance rejection method from the paper.
// 
//

#ifndef ACCEPTANCE_REJECTION
#define ACCEPTANCE_REJECTION

#include <random>

//each event in the structure is represented by an eventAR
struct eventAR {
	unsigned long payload;		//unique event identifier               
	unsigned long position;		//position in the event vector, useful for updates          
	double rate;                //rate of the event


	//generates an event with given payload, position and rate
	eventAR(int pl, int ps, double r) : payload(pl), position(ps), rate(r){}
};

class AcceptanceRejection {                                
	
	double maxR = 0.;           //largest rate encountered by the data structure
	double R = 0.;		//sum of all rate inside the structure
	std::vector<eventAR*> items;       //where events are stored
	
	//random number generator + uniform real distribution between 0 and 1
	std::uniform_real_distribution<double> randExt;
	std::mt19937 rng;
	
public:

	//init random number generator
	AcceptanceRejection(){
		rng.seed(time(nullptr));
	};
	
	//cleans up the events before deleting the structure
	~AcceptanceRejection(){
		for (auto & item : items) delete item;
	}
	//adds an event to the data structure with given rate and payload
	eventAR* addEvent(unsigned long payload, double rate);
	
	//adds a pre-created event                        
	eventAR* addEvent(eventAR * it);
	
	//returns an event according to its rate
	eventAR * sampleEvent();                                  
	
	//updates the rate of event i to rate newRate														
	void updateEvent(eventAR* i, double newRate);                  

	//deletes event 
	void deleteEvent(eventAR* i);                          

};


#endif //ACCEPTANCE_REJECTION
