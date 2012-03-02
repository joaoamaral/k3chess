#ifndef __Random_h
#define __Random_h

#include "Singletons.h"

class RandomGen
{
   friend void Singletons::initialize();
   friend void Singletons::finalize();
   //
   RandomGen();
   ~RandomGen();

public:
   int get(int lo, int hi);

private:
   int seed_;
};

#endif
