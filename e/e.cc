#include <fstream>
#include <math.h>
#include <ctype.h>
#include <iostream>
using namespace std;

bool isPrime(unsigned long long);

int main(void) {
	ifstream eFile("e.digits");

	char eString[20000];
	int i;
	for(i = 0; eFile.good();)
	{
		char t = eFile.get();
		if(t == 48 ||
		   t == 49 ||
		   t == 50 ||
		   t == 51 ||
		   t == 52 ||
		   t == 53 ||
		   t == 54 ||
		   t == 55 ||
		   t == 56 ||
		   t == 57 ||
		   t == 58  )
		{
			eString[i] = t;
			++i;
		}
		else
			continue;
	}
	eString[i+1] = '\0';
	int len = i + 1;

	char currentTenDigits[] = "0000000000"; // ten digits
	int percentFinished  = 0;

	for(int i = 0; i < len - 10; ++i)
	{
		for(int z = 0; z < 10; ++z)
			currentTenDigits[z] = eString[z+i];
		/*
		int digitsAdded = 
		currentTenDigits[0] +
		currentTenDigits[1] +
		currentTenDigits[2] +
		currentTenDigits[3] +
		currentTenDigits[4] +
		currentTenDigits[5] +
		currentTenDigits[6] +
		currentTenDigits[7] +
		currentTenDigits[8] +
		currentTenDigits[9] - 480;
		if(digitsAdded == 49)
			printf("%s adds up to 49 at i = %d\n", currentTenDigits, i);
		*/

		unsigned long long amIPrime = 0;
		sscanf(currentTenDigits, "%lld10", &amIPrime);
		if(isPrime(amIPrime))
			printf("%lld is prime at i = %d.\n", amIPrime, i);
	}
}

bool isPrime(unsigned long long candidate) {
	if(candidate == 0 || candidate == 1 || candidate == 2) return false;
	for(unsigned long long denominator = 2; denominator < candidate / 2; ++denominator)
		if(candidate % denominator == 0)
			return false;
	return true;
}
