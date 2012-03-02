
// main.cpp

// includes

#include <cstdio>
#include <cstdlib>

#include "attack.h"
#include "book.h"
#include "hash.h"
#include "move_do.h"
#include "option.h"
#include "pawn.h"
#include "piece.h"
#include "protocol.h"
#include "toga2_random.h"
#include "square.h"
#include "trans.h"
#include "util.h"
#include "value.h"
#include "vector.h"

#include <string>
#include <list>

// functions

// main()

void toga2_main()
{

   // init

   util_init();
   my_random_init(); // for opening book

   // printf("Toga II 1.3.1 UCI based on Fruit 2.1 by Thomas Gaksch and Fabien Letouzey. Settings by Dieter Eberle\n");

   // early initialisation (the rest is done after UCI options are parsed in protocol.cpp)

   option_init();

   square_init();
   piece_init();
   pawn_init_bit();
   value_init();
   vector_init();
   attack_init();
   move_do_init();

   random_init();
   hash_init();

   trans_init(Trans);
   book_init();

   // loop

   loop();

}
