#include <stdio.h>
#include <gmp.h>
#include <stdint.h>
#include <stdlib.h>
#include "numtheory.h"
#include "rsa.h"
extern gmp_randstate_t state;
gmp_randstate_t state;

void randstate_init(uint64_t seed){
	// start random num gen
	gmp_randinit_mt(state);
	gmp_randseed_ui(state, seed);
	srandom(seed);
}

void randstate_clear(void){
	gmp_randclear(state);
}

