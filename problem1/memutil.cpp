#include<iostream>
#include "libmymem.hpp"
#include<bits/stdc++.h>
#include <time.h>
#include <unistd.h>
using namespace std;
int main(int arg,char* argv[]){
	long long iterations;
	iterations=atoi(argv[2]);
	long long min=1;
	long long max=8192;
	long long sleepmin=10;
	long long sleepmax=15;
	long long random,sleeping;
	for(int i=0; i<iterations; i++)
{
     random = rand()%(max-min+1) + min;				//random number generation
     sleeping= rand()%(sleepmax-sleepmin+1)+sleepmin; 
     int* a= (int*)mymalloc(random);
	 usleep(sleeping*10);							//sleeping
	 myfree(a);										
    //cout<<random<<endl;
}
	return 0;
}