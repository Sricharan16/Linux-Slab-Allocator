#include "libmymem.hpp"
#include<bits/stdc++.h>
#include<bitset>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
using namespace std;
#define SZ (1024*64)

std::mutex rdx[12];
struct object				//struct object which points to starting address of the slab
{
    struct slab* slabparent;
};
struct bucket				//struct bucket which cointains bucket size
{							//and pointer to the first slab of that bucket
	long long objsz;
	struct slab* firstslab;
};
struct slab 				//this struct cointains the information of the slab
{
	struct bucket* parent;
	struct slab* nextslab;
	long long freeobjects;
	long long totalobjects;
	std::bitset <6000> bitmap;

};							//initilizing the bucket
struct bucket List[12]={{4,NULL},{8,NULL},{16,NULL},{32,NULL},{64,NULL},{128,NULL},{256,NULL},{512,NULL},{1024,NULL},{2048,NULL},{4096,NULL},{8192,NULL}};
int numberofobjects(int size)		//function to know the number of objects in the slab
{
    int temp=0;
    temp=64*1024-sizeof(struct slab);
    temp=temp/(size+sizeof(struct slab*));
    return temp;
}
void* mymalloc(int size)			//mymalloc function which allocats memory
{
    int i=0;
    for(;i<12;i++)					//checks from which bucket should the memory be allocated
    {
        if(size<=List[i].objsz)
            break;
    }
    if(List[i].firstslab==NULL)		//if there are no slabs for a bucket the adding the slab
    {
        struct slab* buf;
        if ((buf = (struct slab*)mmap(NULL, SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0)) == MAP_FAILED) 
        {							//getting memory by mmap 
            perror("mmap");
            exit(1);
        }
        List[i].firstslab=buf;
        buf->totalobjects=numberofobjects(size); // calculating the total number of objects in bucket
        buf->freeobjects=numberofobjects(size);
        buf->parent= (&List[i]);
        buf->nextslab=NULL;
    }
    struct slab* temp;					
    struct slab* prev;
    temp=prev=List[i].firstslab;
    for(;temp!=NULL;)					// this loop gives the slab form which we can allocate 
    {
        if(temp->freeobjects==0)
        {
            prev=temp;
            temp=temp->nextslab;
        }
        else
            break;
    }
    if(temp==NULL)				//there are slabs to the bucket and non of them are free
    {							// then creating the new slab and allocating in that slab
        struct slab* bufferi;
        if ((bufferi = (struct slab*)mmap(NULL, SZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0)) == MAP_FAILED) 
        {
            perror("mmap");
            exit(1);
        }
        prev->nextslab=bufferi;
        bufferi->totalobjects=numberofobjects(size);
        bufferi->freeobjects=numberofobjects(size);
        bufferi->parent= &List[i];
        bufferi->nextslab=NULL;		//forms the link with that new slab
        prev=bufferi;

    }
    else
        prev=temp;					//finally this gives the pointer to the slab that we are 
    for(int j=0;j<numberofobjects(i);j++) //allocating in
    {
        if(prev->bitmap[j]==0)
        {
            prev->freeobjects--;
            prev->bitmap[j]=1;
            int offset=sizeof(struct slab)+j*(sizeof(struct slab*)+List[i].objsz);
            unsigned char* bytePtr = reinterpret_cast<unsigned char*>(prev);
            bytePtr += offset;
            unsigned char* peta=bytePtr;
            struct object* tempoo=(struct object*)peta;
            tempoo->slabparent =prev;
            bytePtr+=sizeof(struct slab*);
            return (void*)(bytePtr);
        }
    }
    int x=0;
     void* ptr=&x;
    return ptr;
}
void myfree(void *ptr)		// free function
{
    unsigned char* temp= (unsigned char*)ptr;
    struct object* tempoo=(struct object*)(temp-sizeof(struct slab*));
    struct slab* initial=tempoo->slabparent;
    unsigned char* pla=(unsigned char*)initial;
    if(initial->freeobjects!=initial->totalobjects-1)	//if the total number of objects in 
    { 													//the slab are greater than 1
        int k=(temp-(pla+sizeof(slab)))/(sizeof(object)+initial->parent->objsz);
        initial->bitmap[k]=0;
        initial->freeobjects++;
    }
    else{										//if the slab contains only 1 object
        struct slab* init= initial->parent->firstslab;//then deleting that slab
        if(init==initial)
        {
            initial->parent->firstslab=initial->nextslab;
        }
        else
        {
            while(init->nextslab!=initial)
            {
                init=init->nextslab;
            }
            init->nextslab=initial->nextslab;
        }
        if (munmap(initial, SZ) == -1) 			// unmapping the slab memory
        {
            perror("Error un-mmapping the file");
        }
    }
}