#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include "numtheory.h"
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>


void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters){
	//manually sets range for random() bitcounts and calcs p and q bitcounts
	uint64_t p_bits = ((random() % ((2 * nbits) / 4)) + (nbits/4));
	uint64_t q_bits = nbits - p_bits;
	//initializes the million temp values (I love GMP!!!)
	mpz_t random_num;
	mpz_init(random_num);
	mpz_t p_val;
	mpz_init(p_val);
	mpz_t q_val;
	mpz_init(q_val);
	mpz_t sumpq;
	mpz_init(sumpq);
	mpz_t denom_gcd;
	mpz_init(denom_gcd);
	mpz_t psub1;
	mpz_t qsub1;
	mpz_inits(psub1, qsub1, NULL);
	//calculates constants for p-1 and q-1
	mpz_sub_ui(psub1, p, 1);
	mpz_sub_ui(qsub1, q, 1);
	mpz_t final;
	mpz_init(final);
	// p prime creation
	make_prime(p_val, p_bits, iters);
	mpz_set_ui(p, 0);
	// q prime creation
	make_prime(q_val, q_bits, iters);
	//reset p var
	mpz_set_ui(p, 0);
	//set p and q equal to their actual values
	mpz_set(p, p_val);
	mpz_set(q, q_val);
	// calc n 
	mpz_mul(n, p, q);
	//calc carmichaels(n) using formula from
	//design doc
	mpz_add(sumpq, p, q);
	mpz_sub(sumpq, n, sumpq);
	mpz_add_ui(sumpq, sumpq, 1);
	mpz_abs(sumpq, sumpq);
	gcd(denom_gcd, psub1, qsub1);
	mpz_fdiv_q(final, sumpq, denom_gcd);
	//tempval for random num
	mpz_t rand_gcd;
	mpz_init(rand_gcd);
	mpz_t temp_car;
	mpz_init(temp_car);
	while(mpz_sizeinbase(e,2) < nbits || mpz_cmp_ui(rand_gcd, 1) != 0){ // generates rand nums until its nbits long and 
		     // gcd(carmichael(n), random_num) == 1
			mpz_urandomb(random_num, state, nbits);
			//continuall sets e to the new rand_num so it'll be correct when it breaks
			mpz_set(e, random_num);
			mpz_set(temp_car, final);
			gcd(rand_gcd, random_num, temp_car);
	}
	//clear temps
	mpz_clears(random_num, p_val, q_val, sumpq, denom_gcd, psub1, qsub1, final, rand_gcd, temp_car, NULL);
	
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	// just prints all 4 variables to the specified file w/ trailing newlines
	gmp_fprintf(pbfile, "%Zx\n\n%Zx\n\n%Zx\n\n%s\n\n", n, e, s, username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	// reads all 4 vars from the specified file and sets the mpz_t vars equal to them
	// formatting is the same as done in write_pub so it can read properly
	gmp_fscanf(pbfile, "%Zx\n\n%Zx\n\n%Zx\n\n%s\n\n", n, e, s, username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q){
	//initialize all the temp vars for various calculations
	mpz_t n;
	mpz_t sumpq;
	mpz_t denom_gcd;
	mpz_t final;
	mpz_t psub1;
	mpz_t qsub1;
	mpz_inits(n, sumpq, denom_gcd, final, psub1, qsub1, NULL);
	// p-1 and q-1 temps
	mpz_sub_ui(psub1, p, 1);
	mpz_sub_ui(qsub1, q, 1);
	//calc n
	mpz_mul(n, p, q);
	//final = carmichaels(n), copy pasted from make_pub
	mpz_add(sumpq, p, q);
        mpz_sub(sumpq, n, sumpq);
        mpz_add_ui(sumpq, sumpq, 1);
        mpz_abs(sumpq, sumpq);
        gcd(denom_gcd, psub1, qsub1);
        mpz_fdiv_q(final, sumpq, denom_gcd);
	//calculate d by taking inverse mod of d and e
	mod_inverse(d, e, final);
	mpz_clears(n,sumpq,denom_gcd,final,psub1,qsub1, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile){
	//same thing as write_pub, jsut writes n and d to specified file with trailing
	//newlines after each
	gmp_fprintf(pvfile, "%Zx\n\n%Zx\n\n", n, d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile){
	//same thing as read_pub, just reads n and d from specified file with formatting
	//identical to write_priv
	gmp_fscanf(pvfile, "%Zx\n\n%Zx\n\n", n, d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n){
	//just our pow_mod function of (c, m, e, n). provided in lab doc
	pow_mod(c, m, e, n);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n){
	//just pow_mod of (m,c,d,n), provided in lab doc
	pow_mod(m, c, d, n);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n){
	//once again basic pow mod, formula provided in lab doc
	mpz_t sign_temp;
	mpz_init(sign_temp);
	pow_mod(sign_temp, m, d, n);
	mpz_set(s, sign_temp);
	mpz_clear(sign_temp);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n){
	//basically just computes what the signature should be from the provided message
	//and checks it against the provided signature. If they're the same, the message
	//signature is verified and returns true. If it isn't, theres a problem so return
	//false.
	mpz_t t;
	mpz_init(t);
	pow_mod(t, s, e, n);
	if(mpz_cmp(t, m) == 0){
		mpz_clear(t);
		return true;
	}
	else{
		mpz_clear(t);
		return false;
	}
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e){
	//first, calculate the block size, which is ((log_2(n) - 1) / 8)
	uint64_t k = mpz_sizeinbase(n, 2);
	k -= 1;
	k /= 8;
	//temp var initialization
	uint64_t j;
	mpz_t temp_hex;
	mpz_init(temp_hex);
	mpz_t c;
	mpz_init(c);
	//pointer for memory block made inside the while loop
	uint8_t *block;
	while(feof(infile) == 0){ // this jsut checks if the stream pointer has reached the end of the file, and breaks if so
		//allocates the block
		block = (uint8_t*)calloc(k, sizeof(uint8_t));
		// prepend 0xFF onto array as first element
		block[0] = 0xFF;
		//read k-1 elements from the infile and store them in block starting at index 1 by incremeting the pointer
		//by 1. This prevents 0xFF from being overwritten. j is num of bytes read as returned by fread
		j = fread(block+1, sizeof(uint8_t), k-1, infile);
		//imports the array (block) and converts the values into an mpz_t var. Read j+1 bytes to account for prepended 0xFF.
		mpz_import(temp_hex, j+1, 1, sizeof(uint8_t), 1, 0, block);
		//encrypt the imported mpz_t message temp_hex
		rsa_encrypt(c, temp_hex, e, n);
		//prints the encrypted message block to the outfile
		gmp_fprintf(outfile,"%Zx\n\n", c);
		free(block);
	}
	//clear temps and done
	mpz_clears(temp_hex, c, NULL);

}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d){
	//same block calculation as encrypt_file
	uint64_t k = mpz_sizeinbase(n, 2);
	k-= 1;
	k /= 8;
	//temp vars
	uint64_t j;
	mpz_t temp;
	mpz_init(temp);
	mpz_t c;
	mpz_init(c);
	mpz_t m;
	mpz_init(m);
	uint8_t *block;
	while(feof(infile) == 0){// same loop conditional as encrypt file
		block = (uint8_t*)calloc(k, sizeof(uint8_t));
		//scans in the hexstring from the encrypted file and sets it to c
		gmp_fscanf(infile, "%Zx\n\n", c);
		//decrypts that hexstring and sets it to m
		rsa_decrypt(m, c, d, n);
		//exports m back into byte array format so it can be written to the outfile
		mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);
		//writes each byte of the array to the outfile piece by piece
		fwrite(block+1, sizeof(uint8_t), j - 1, outfile);
		//free the block so it can be reallocated in the next loop
		free(block);
	}
	//clear temps and done
	mpz_clears(temp, c, m, NULL);
}
