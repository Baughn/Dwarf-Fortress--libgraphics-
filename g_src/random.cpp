#include "platform.h"
#include <string.h>
#include <math.h>
#include <iosfwd>
#include <iostream>
#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <zlib.h>

#include "svector.h"
using std::string;

#include "endian.h"

#include "files.h"

#include "textlines.h"

#include "enabler.h"

#include "basics.h"

#include "random.h"

extern long basic_seed;
extern int mt_index[MT_BUFFER_NUM];
extern short mt_cur_buffer;
extern short mt_virtual_buffer;
extern unsigned long mt_buffer[MT_BUFFER_NUM][MT_LEN];

//public domain RNG stuff by Michael Brundage
	//with some modifications by me to handle more buffers

void mt_init()
{
	mt_cur_buffer=0;
	mt_virtual_buffer=0;

    int i;
    for(i=0;i<MT_LEN;i++)mt_buffer[0][i]=rand()*GetTickCount();
    mt_index[0]=MT_LEN*sizeof(unsigned long);

	long j;
	for(j=0;j<20;j++)trandom_twist();
}

#define MT_IA           397
#define MT_IB           (MT_LEN - MT_IA)
#define UPPER_MASK      0x80000000
#define LOWER_MASK      0x7FFFFFFF
#define MATRIX_A        0x9908B0DF
#define TWIST(b,i,j)    ((b)[i] & UPPER_MASK) | ((b)[j] & LOWER_MASK)
#define MAGIC(s)        (((s)&1)*MATRIX_A)

unsigned long mt_trandom()
{
    unsigned long * b = mt_buffer[mt_cur_buffer];
    int idx = mt_index[mt_cur_buffer];
    unsigned long s;
    int i;
	
    if (idx == MT_LEN*sizeof(unsigned long))
    {
        idx = 0;
        i = 0;
        for (; i < MT_IB; i++) {
            s = TWIST(b, i, i+1);
            b[i] = b[i + MT_IA] ^ (s >> 1) ^ MAGIC(s);
        }
        for (; i < MT_LEN-1; i++) {
            s = TWIST(b, i, i+1);
            b[i] = b[i - MT_IB] ^ (s >> 1) ^ MAGIC(s);
        }
        
        s = TWIST(b, MT_LEN-1, 0);
        b[MT_LEN-1] = b[MT_IA-1] ^ (s >> 1) ^ MAGIC(s);
    }
    mt_index[mt_cur_buffer] = idx + sizeof(unsigned long);
     return *(unsigned long *)((unsigned char *)b + idx);
}

void trandom_twist()
{
    unsigned long * b = mt_buffer[mt_cur_buffer];
    unsigned long s;
    int i;
	
    i = 0;
    for (; i < MT_IB; i++) {
        s = TWIST(b, i, i+1);
        b[i] = b[i + MT_IA] ^ (s >> 1) ^ MAGIC(s);
    }
    for (; i < MT_LEN-1; i++) {
        s = TWIST(b, i, i+1);
        b[i] = b[i - MT_IB] ^ (s >> 1) ^ MAGIC(s);
    }
    
    s = TWIST(b, MT_LEN-1, 0);
    b[MT_LEN-1] = b[MT_IA-1] ^ (s >> 1) ^ MAGIC(s);
}

//back to my crap - tarn

//picks a random number from 0 to max-1
long loadtrandom(unsigned long max)
{
	unsigned long seed=mt_trandom();
	seed=seed%max;

	return((long)seed);
}

long trandom(unsigned long max)
{
	if(max<=1)return 0;
	unsigned long seed=mt_trandom();
	seed=seed%2147483647LU;
	seed=seed/((2147483647LU/max)+1);

	return((long)seed);
}

void pop_trandom_uniform_seed()
{
	if(mt_virtual_buffer>0)mt_virtual_buffer--;
	mt_cur_buffer=mt_virtual_buffer;
	if(mt_cur_buffer>=MT_BUFFER_NUM)mt_cur_buffer=MT_BUFFER_NUM-1;
}

void push_trandom_uniform_seed(unsigned long newseed)
{
	mt_virtual_buffer++;
	mt_cur_buffer=mt_virtual_buffer;
	if(mt_cur_buffer>=MT_BUFFER_NUM)
		{
		mt_cur_buffer=MT_BUFFER_NUM-1;
		errorlog_string("Random Buffer Overload");
		}

    short i;

	unsigned long * b = mt_buffer[mt_cur_buffer];

	basic_seed=newseed;
    for(i=0;i<MT_LEN;i++)
		{
        b[i]=basic_seed;r_num();
		}
    mt_index[mt_cur_buffer]=MT_LEN*sizeof(unsigned long);

	long j;
	for(j=0;j<20;j++)trandom_twist();
}

//picks a random number from 0 to max-1
long basic_random(long max)
{
	r_num();

	return (long)((unsigned long)basic_seed/((1073741824UL/(unsigned long)max)+1UL));
}

//sets seed to a random number from 0 to 1 billion
void r_num()
{
	basic_seed=(long)(((unsigned long)basic_seed*907725UL+99979777UL)%1073741824UL);
}