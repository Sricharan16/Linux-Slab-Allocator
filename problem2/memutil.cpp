#include<bits/stdc++.h>
#include<unistd.h>
#include<mutex>
using namespace std;
#include "libmymem.hpp"
mutex rdx[12];                //12 mutex locks for 12 buckets
void threadMain(int threadId, int niterations) {
    int objList[12];
    
    for(int kl=0; kl<12; kl++) objList[kl] = pow(2, kl + 2);

    srand(time(NULL));
    for(int kl=0; kl<niterations; kl++) {
        int k= rand()%12;          //random number generation 
        int size = objList[k];
        rdx[k].lock();              //locking for bucket k
        void*p=mymalloc(size);      //calling mymalloc function
        rdx[k].unlock();            //unlocking for bucket k 
        usleep(1000);               //allowing the thread to sleep
        rdx[k].lock();
            myfree(p);              //calling myfree function
        rdx[k].unlock();
    }

}

int main(int argc, char const *argv[])
{
    int nthreads, niterations;              //variable declaration number of threads and iterations
    nthreads=atoi(argv[4]); niterations=atoi(argv[2]);
    thread th[nthreads];                    //declaring threads
    for(int i=0; i<nthreads; i++) {
        th[i] = std::thread(threadMain,i, niterations);
    }

    for(int i=0; i<nthreads; i++) {
        th[i].join();                       //waiting for the threads to join
    }
    return 0;
}