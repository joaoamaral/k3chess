#include "Random.h"
#include <ctime>
#include <cstdlib>

Random::Random() : seed_((unsigned)time(0))
{
   srand(seed_);
}

Random::~Random()
{
}

int Random::get(int lo, int hi)
{
   if(hi<lo) { int x = hi; hi = lo; lo = x; }
   int r0 = rand();
   int r1 = r0 % (hi-lo+1) + lo;
   return r1;
}

