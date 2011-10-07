#include "Random.h"
#include <ctime>
#include <cstdlib>

Random::Random() : seed_((unsigned)time(0)^((unsigned)time(0)<<16))
{
   srand(seed_);
}

Random::~Random()
{
}

int Random::get(int lo, int hi)
{
   if(hi<lo) { int x = hi; hi = lo; lo = x; }
   return lo + rand()*(hi-lo)/RAND_MAX;
}

