#ifndef __Random_h
#define __Random_h

#include "Singletons.h"

class Random
{
   friend void Singletons::initialize();
   friend void Singletons::finalize();
   //
   Random();
   ~Random();

public:
   int get(int lo, int hi);

private:
   int seed_;
};

#endif
