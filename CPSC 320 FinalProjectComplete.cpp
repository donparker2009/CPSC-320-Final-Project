//CPSC 320 Final Project
//Dylan Lafontsee, Ahmad Naqshbandi, Don Parker

#include<iostream>
#include<random>
#include<cmath>
#include<limits>
#include<math.h>
#include<queue>
using namespace std;

//Vendor struct
struct Vendor {
	int line;
	double nextDeparture;
	int totalServed;
	double serviceTime;
	queue<double> ArrivalQ;
};

uniform_int_distribution<> r_dist(0,RAND_MAX);
mt19937 r_entry{};

// Seed the random number generator
void my_srandom(int s){
	
	r_entry.seed(s);
	
}

// Return a random integer between 0 and RAND_MAX
int my_random(){
	
	return r_dist(r_entry);
	
}

// Returns a Unifrom Random Variable in range [raneStart, rangeEnd]
double uniform(double rangeStart, double rangeEnd){
	
	return rangeStart+(my_random()/((double)RAND_MAX))*(abs(rangeStart-rangeEnd));
	
}

// Returns an exponential random variable with specified rate
double exponential(double rate){
	
	double data=0;
	while (data==0) data=uniform(0,1);

		
	return (-1)*log(data)/rate;
	
}

// Returns a gamma random variable with specified n and rate
double gamma(int n, double lambda)
{
	double x = 1;
	double u;
	
	for(int i = 0; i < n; i++)
	{
		u = 0;
		while (u == 0)
		{
			u = uniform(0,1);
		}
		
		x = x * u;
	}

	x = -1/lambda * log(x);
	
	return x;	
		
}

//Chooses a Vendor based on an initial Vendor chosen and the two closest vendors.
int chooseVendor(Vendor vendor[], int vendorLen){
	
		int index = (int) uniform(0,vendorLen);
		
		while(index == vendorLen)
		{
		index = (int) uniform(0,vendorLen);
		}
		
		double distancePoints = (3770.0/vendorLen)/396.0;
		double minPoints = vendor[index].line;
		int minIndex = index;

		if (index==0){
			
			if (minPoints>distancePoints+vendor[vendorLen-1].line){
				minPoints=distancePoints+vendor[vendorLen-1].line;
				minIndex=vendorLen-1;
			}
		
			if (minPoints>distancePoints+vendor[1].line){
				minPoints=distancePoints+vendor[1].line;
				minIndex=1;
			}
			
		}
		
		else {
			
			if (minPoints>distancePoints+vendor[index-1].line){
				minPoints=distancePoints+vendor[index-1].line;
				minIndex=index-1;
			}
		
			if (minPoints>distancePoints+vendor[(index+1)%vendorLen].line){
				minPoints=distancePoints+vendor[(index+1)%vendorLen].line;
				minIndex=(index+1)%vendorLen;
			}
			
		}
		
		
		return minIndex;
		
}

void convertTime(double hours)
{
	
	int totalSeconds;
	int intHours;
	int minutes;
	int seconds;
	 
	totalSeconds = hours * 3600;
	intHours = hours;
	minutes = (totalSeconds - intHours * 3600)/60;
	seconds = totalSeconds - intHours * 3600 - minutes * 60; 
	cout << intHours << " Hours " << minutes << " Minutes " << seconds << " Seconds "; 
	
}


int main()
{
	//INITIAL VARIABLES
	double closingTime = 3.0; 
	int peopleTotal = gamma(200,1/225.0); //(Mean 45,000) (SD approx. 3,000) Lining up with known statistics
	int peopleServicing = .72 * peopleTotal;
	int numVendors;
	double percentServed;
	
	//TRACKING VARIABLES
	double currentTime = 0.0;
	double nextArrival = 0;
	//double nextArrival = exponential(peopleServicing/closingTime);
	
	//OUTPUT VARIABLES
	int globalTotalServed = 0;
	int totalProductsSold = 0;
	double avgServiceTime = 0;
	
	//Get simulation information
	cout << "Enter number of Vendors for simulation: ";
	cin >> numVendors;
	
	//Vendor array
	Vendor vendorRoster[numVendors];
	
	//Initialize Vendors
	for (int i = 0; i < numVendors; i++)
	{
		vendorRoster[i].line = 0;
		vendorRoster[i].nextDeparture = INFINITY;
		vendorRoster[i].totalServed = 0;
		vendorRoster[i].serviceTime = 0;
	}
	
	//Start Simulation Time Loop
	while(currentTime <= closingTime)
	{
		//Checking Departures
		for (int i = 0; i < numVendors; i++)
		{	
			while(vendorRoster[i].nextDeparture <= currentTime)
			{
				vendorRoster[i].serviceTime += vendorRoster[i].nextDeparture;
				vendorRoster[i].serviceTime -= vendorRoster[i].ArrivalQ.front();
				vendorRoster[i].ArrivalQ.pop();
				vendorRoster[i].totalServed++;
				vendorRoster[i].line--; 
				
				if(vendorRoster[i].line > 0)
				{
				vendorRoster[i].nextDeparture += gamma(10,1/.0025); //Based on average service time of 90 seconds with SD about 30 seconds.
				}
				else
				{
				vendorRoster[i].nextDeparture = INFINITY;
				}
					
			}
		}
		
		// Arrival
		nextArrival = currentTime + exponential(peopleServicing/closingTime);
		
		int index = chooseVendor(vendorRoster, numVendors);
		
		vendorRoster[index].line++;
		
		vendorRoster[index].ArrivalQ.push(nextArrival);
		
		currentTime = nextArrival;
		
		if(vendorRoster[index].line == 1)
		{
			vendorRoster[index].nextDeparture = currentTime + gamma(10,1/.0025);
		}
		
	} //End Simulation Time Loop
	
	//Checking Departures again
	for (int i = 0; i < numVendors; i++)
	{
		while(vendorRoster[i].nextDeparture <= closingTime)
		{
			vendorRoster[i].serviceTime += vendorRoster[i].nextDeparture;
			vendorRoster[i].serviceTime -= vendorRoster[i].ArrivalQ.front();
			vendorRoster[i].ArrivalQ.pop();
			vendorRoster[i].totalServed++;
			vendorRoster[i].line--; 
			
			if(vendorRoster[i].line > 0)
			{
			vendorRoster[i].nextDeparture += gamma(10,1/.0025); //Based on average service time of 90 seconds with SD about 30 seconds.
			}
			else
			{
			vendorRoster[i].nextDeparture = INFINITY;
			}
				
		}
	}
		
	//OUTPUTS
	for(int i = 0; i < numVendors; i++)
	{
		cout << "Vendor: " << i + 1 << endl;
		cout << "Total Served: " << vendorRoster[i].totalServed << endl;
		cout << "Total Time in System: "; //Including Line time.
		convertTime(vendorRoster[i].serviceTime);
		cout << endl;
		cout << "AVG Wait: ";
		convertTime(vendorRoster[i].serviceTime / vendorRoster[i].totalServed);
		cout << endl;
		cout << endl;
	}
	
	for(int i = 0; i < numVendors; i++)
	{
		globalTotalServed += vendorRoster[i].totalServed;
		avgServiceTime += vendorRoster[i].serviceTime;
	}
	
	avgServiceTime = avgServiceTime/globalTotalServed;
	avgServiceTime = avgServiceTime;
	
	//Total of people who were served divided by those who ideally wanted to be.
	percentServed = double (globalTotalServed)/peopleServicing;
	
	cout << endl;
	cout << "Number of Vendors: " << numVendors << endl;
	cout << "Total Attendees: " << peopleTotal << endl;
	cout << "Global Total Served: " << globalTotalServed << endl;
	cout << "Global AVG Time in System: ";
	convertTime(avgServiceTime);
	cout << endl;
	cout << "Vendor Efficiency Score: " << globalTotalServed / numVendors * percentServed <<" (Higher is better)" <<endl;
	cout << endl;
	
	
	return 0;
}
