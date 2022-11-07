#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "randstate.h"
#include "rsa.h"


void gcd(mpz_t d, mpz_t a, mpz_t b){
	//finds the gcd of a and b and sets d equal to it.
	mpz_t t;
	mpz_init(t);
	mpz_t temp_a, temp_b;
	mpz_inits(temp_a, temp_b, NULL);
	mpz_set(temp_a, a);
	mpz_set(temp_b, b);
	// set and initialize t
	while(mpz_cmp_si(temp_b, 0) != 0){
		// loop until b = 0
		// fake parallel assignment with a temp variable
		// and perform the necessary calc with modulus
		mpz_set(t, temp_b);
		mpz_mod(temp_b, temp_a, temp_b);
		mpz_set(temp_a, t);
	}
	// after while loop breaks you have gcd. clear the temp var
	// and set d to a (the gcd).
	mpz_set(d, temp_a);
	mpz_clears(temp_a, temp_b, t, NULL);
	return;
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus){
	mpz_t v;
	mpz_init_set_ui(v, 1);
	mpz_t p;
	mpz_t temp_ex;
	mpz_init_set(temp_ex, exponent);
	mpz_init_set(p, base);
	// init and set v to 1 and p to the value of base arg
	while(mpz_cmp_si(temp_ex, 0) > 0){
		// loop until the exponent var is < 0.
		if(mpz_odd_p(temp_ex) != 0){
			// if the exponent var is odd, set
			// v to (v*p) mod (modulus) 
			mpz_mul(v, v, p);
			mpz_mod(v, v, modulus);
		}
		mpz_mul(p, p, p);
		mpz_mod(p, p, modulus);
		// set p to (p*p) mod (modulus)
		mpz_fdiv_q_ui(temp_ex, temp_ex, 2);
		//floor div exponent by 2
	}
	mpz_set(out, v);
	// set out to v and clear temps p and v
	mpz_clear(p);
	mpz_clear(v);
	mpz_clear(temp_ex);
}

bool is_prime(mpz_t n, uint64_t iters){
	if((mpz_cmp_si(n, 1) == 0) || (mpz_cmp_si(n, 2) == 0) || (mpz_cmp_si(n, 3) == 0)){
		// just cheks if the input is 1, 2, or 3, since alg doesnt like
		// low numbers for the random number. Anything 4 and above works
		return true;
	}
	// all the inits for various constant values and temporaries,
	// such as a constant for (n - 1) and a temp_denom for division.
	mpz_t s;
	mpz_t one;
	mpz_t temp_denom;
	mpz_init_set_ui(one, 1);
	//clear later
	mpz_t temp_n;
	mpz_init(temp_n);
	//clear later
	mpz_init(temp_denom);
	// clear later
	mpz_sub_ui(temp_n, n, 1);
	mpz_init_set_ui(s, 0);
	//inits for the for loop variables and a couple more constants.
	mpz_t a;
	mpz_init(a);
	mpz_t temp_ra;
	mpz_t y;
	mpz_init(y);
	mpz_t j;
	mpz_init(j);
	mpz_t two;
	mpz_init_set_ui(two, 2);
	mpz_t r;
	mpz_init_set(r, temp_n);
	//for the s-1 calc
	mpz_t s_sub;
	mpz_init(s_sub);
	mpz_init(temp_ra);
	while(mpz_even_p(r)){
		mpz_fdiv_q(r, r, two);
		mpz_add_ui(s, s, 1);
	}// divides r by two repeatedly and increments s by 1 until r is odd.
	mpz_sub_ui(temp_ra, n, 3);
	mpz_sub_ui(s_sub, s, 1);
	// takes value of n-3 for changing the bounds of the random number
	// generation and also takes a constant value of s-1 for later use.
	for(uint64_t i = 1; i <= iters; i+=1){
		// loops for specified num of iters
		mpz_urandomm(a, state, temp_ra);
		mpz_add_ui(a, a, 2);
		//generates rand number between 2 and n-2 by adding two to
		//output and changing upper bound.
		pow_mod(y,a,r,n);
		if((mpz_cmp_si(y, 1) != 0) && (mpz_cmp(y, temp_n) != 0)){
			//checks if y != 1 and y != n-1
			mpz_set_si(j, 1);
			while((mpz_cmp(j, s_sub) <= 0) && (mpz_cmp(y, temp_n) != 0)){
				// loops while j <= s-1 and y != n-1
				pow_mod(y,y,two,n);
				if(mpz_cmp(y, one) == 0){
					mpz_clears(temp_n,a,temp_ra, s_sub, one, two, j, y, temp_denom, s, r, NULL);
					//clears temp vars and returns false
					return false;
				}
				mpz_add_ui(j, j, 1);
			}
			if(mpz_cmp(y, temp_n) != 0){
				mpz_clears(temp_n, a,temp_ra, s_sub, one, two, j, y, temp_denom, s, r, NULL);
				//clears temp vars and returns false if y!= n-1
				return false;
			}
		}
	}
	// if all conditions pass, clear vars and return true
	mpz_clears(temp_n,a,temp_ra, s_sub, one, two, j, y, temp_denom, s, r, NULL);
	return true;
	
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n){
	mpz_t r;
	mpz_t r_pri;
	mpz_t r_temp;
	mpz_t t;
	mpz_t t_pri;
	mpz_t t_temp;
	mpz_t q;
	mpz_init(q);
	mpz_init(r_temp);
	mpz_init(t_temp);
	mpz_init_set(r, n);
	mpz_init_set(r_pri, a);
	mpz_init_set_si(t, 0);
	mpz_init_set_si(t_pri, 1);
	// initializes 7 variables, two of them are specifically temp vars
	// for imitating parallel assignment and one is the quotient value.
	// Others are in normal pseudocode on lab doc.
	while((mpz_cmp_si(r_pri, 0) != 0)){ // loops until r = 0
		// floor div of r and r_pri
		mpz_fdiv_q(q, r, r_pri);
		//simultaneous assign for r, r_pri
		mpz_set(r_temp, r);
		mpz_set(r, r_pri);
		mpz_mul(r_pri, q, r_pri);
		mpz_sub(r_pri, r_temp, r_pri);
		//simuiltaneous assign for t, t_pri
                mpz_set(t_temp, t);
                mpz_set(t, t_pri);
                mpz_mul(t_pri, q, t_pri);
                mpz_sub(t_pri, t_temp, t_pri);
	}
	// checks if r > 1, returns 0 (no inverse) if it is
	if(mpz_cmp_ui(r, 1) > 0){
		mpz_set_si(i, 0);
		mpz_clears(r, r_pri, r_temp, t, t_pri, t_temp, q, NULL);
		return;
	}
	//if t< 0, increment t by n
	if(mpz_cmp_ui(t, 0) < 0){
		mpz_add(t, t, n);
	}
	// set final output var (i) to t and return after clearing vars
	mpz_set(i, t);
	mpz_clears(r, r_pri, r_temp, t, t_pri, t_temp, q, NULL);
	return;
	
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters){
	// remove the randseed stuff later, will conflict with other files
	// initializes the random number for testing 
	mpz_t rand_num;
	mpz_init(rand_num);
	// just loops until the number thats generated is the right number
	// of bits long and is a prime. Some overhead but should be fine.
	// Uses shortcircuiting to increase efficiency a bit.
	while((mpz_sizeinbase(rand_num, 2) < bits) || is_prime(rand_num, iters) == false){
		// generates random num and then checks if its prime
		mpz_urandomb(rand_num, state, bits);
		if(mpz_even_p(rand_num) != 0){
			// just checks if a number is even and just adds 1 to make it odd since even nums can never be prime
			mpz_add_ui(rand_num, rand_num, 1);
		}
	}
	// once loop breaks, have a valid prime number. Set the output p to it
	// and return after clearing rand_num.
	mpz_set(p, rand_num);
	mpz_clear(rand_num);
	return;
}

