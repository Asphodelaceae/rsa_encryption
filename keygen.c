#include <stdio.h>
#include <gmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"
#define OPTIONS "b:i:n:d:s:vh"


int main(int argc, char **argv){
// initialize defaults for num of bits, Miller-Rabin iters, the seed, and the public and private key file names
uint64_t nbits = 1024;
uint64_t iters = 50;
uint64_t seed = time(NULL);
char *pub_file = "rsa.pub";
char *priv_file = "rsa.priv";
bool verbose = false;
int operator;
	while ((operator = getopt(argc, argv, OPTIONS)) != -1){
		// initializes temp vars for argument parsing
		uint64_t temp_bits;
		uint64_t temp_iters;
		uint64_t temp_seed;
		switch(operator){
			case 'b':
				temp_bits = atoi(optarg);
				if((temp_bits < 50) || (temp_bits > 4096) || optarg[0] == '-'){ // checks the provided bit val is 50-4096 and non-negative
					// prints help msg if incorrect and returns nonzero
					fprintf(stderr, "./keygen-dist: Number of bits must be 50-4096, not %s.\n", optarg);
					fprintf(stderr, "Usage: ./keygen [options]\n");
                                	fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                        	        fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                	                fprintf(stderr, "  the program options.\n");
        	                        fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
	                                fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                	fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                        	        fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                	                fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
        	                        fprintf(stderr, "    -v          : Enable verbose output.\n");
	                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
				}
				nbits = temp_bits;
				break;
			case 'i':
				temp_iters = atoi(optarg);
				if((temp_iters < 1) || (temp_iters > 500) || optarg[0] == '-'){ // checks the provided iters val is 1-500 and non-negative
					// print help msg if out of range
					fprintf(stderr, "./keygen-dist: Number of iterations must be 1-500, not %s.\n", optarg);
					fprintf(stderr, "Usage: ./keygen [options]\n");
                                	fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                        	        fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                	                fprintf(stderr, "  the program options.\n");
        	                        fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
	                                fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                	fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                        	        fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                	                fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
        	                        fprintf(stderr, "    -v          : Enable verbose output.\n");
	                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
				}
				iters = temp_iters;
				break;
			case 'n':
				if(strlen(optarg) == 0){ // just in case an empty file name is provided. Don't know if its necessary or not
					fprintf(stderr, "Empty pbfile name\n");
					fprintf(stderr, "Usage: ./keygen [options]\n");
	                                fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
        	                        fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                	                fprintf(stderr, "  the program options.\n");
                        	        fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                	fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
	                                fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
        	                        fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                	                fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                        	        fprintf(stderr, "    -v          : Enable verbose output.\n");
                                	fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
				}
				//sets file name equal to optarg string
				pub_file = optarg;
				break;
			case 'd':
				// same as case n
				if(strlen(optarg) == 0){
                                        fprintf(stderr, "Empty pvfile name\n");
					fprintf(stderr, "Usage: ./keygen [options]\n");
                                	fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                                	fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                	fprintf(stderr, "  the program options.\n");
	                                fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
         	                        fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                	                fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                        	        fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                	fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                               		fprintf(stderr, "    -v          : Enable verbose output.\n");
                                	fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
					return 1;
                                }
				priv_file = optarg;
				break;
			case 's':
				// dont really care is str is negative or not, strtol will convert it anyways and mersenne twister is fine with it
				temp_seed = strtol(optarg, NULL, 10);
				seed = temp_seed;
				break;
			case 'v':
				verbose = true;
				break;
			case 'h':
				// help msg
				fprintf(stderr, "Usage: ./keygen [options]\n");
				fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
				fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
				fprintf(stderr, "  the program options.\n");
				fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
				fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
				fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
				fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
				fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
				fprintf(stderr, "    -v          : Enable verbose output.\n");
				fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default:
				// print a help message if an invalid option is provided
				fprintf(stderr, "Usage: ./keygen [options]\n");
                                fprintf(stderr, "  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                                fprintf(stderr, "  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                fprintf(stderr, "  the program options.\n");
                                fprintf(stderr, "    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                fprintf(stderr, "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                fprintf(stderr, "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                fprintf(stderr, "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                fprintf(stderr, "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				// help msg here too
				return 1;
		}
	}
		// initialize pointers for the public and private key files and open them using the provided file names
		// in write only mode
                FILE *public_file;
                FILE *private_file;
                public_file = fopen(pub_file, "w");
                private_file = fopen(priv_file, "w");
		// check that they opened successfully
                if((public_file == NULL) || (private_file == NULL)){
                        fprintf(stderr, "File opening failure\n");
                        return 1;
                }
                int perm_out;
                int chmod_out;
		// get pointer for the permissions of the priv key file
                perm_out = fileno(private_file);
                if(perm_out == -1){
                        fprintf(stderr, "fileno error\n");
                }
		// set permissions to 0600 using the union of IRUSR (user read-only) and IWUSR (user write-only)
                chmod_out = fchmod(perm_out, S_IRUSR|S_IWUSR);
		// makes sure fchmod ran properly
                if(chmod_out == -1){
                        fprintf(stderr, "chmod error\n");
		}
		// initialize randstate and necessary variables for the key generation
                randstate_init(seed);
                mpz_t p, q, n, e, s, d;
                mpz_inits(p, q, n, e, s, d, NULL);
		//make pub and priv keys
                rsa_make_pub(p, q, n, e, nbits, iters);
                rsa_make_priv(d, e, p, q);
                //get the username string by checking USER environment variable
                char *username;
                username = getenv("USER");
		// convert username to mpz_t
                mpz_t temp_user;
                mpz_init(temp_user);
                mpz_set_str(temp_user, username, 62);
		// compute sign of the mpz_t version of username
                rsa_sign(s, temp_user, d, n);
		// write the final key info to files
                rsa_write_pub(n, e, s, username, public_file);
                rsa_write_priv(n, d, private_file);
                //close files
                fclose(public_file);
                fclose(private_file);
		if(verbose){
			// finds info of the bit size of each variable and prints each var with corresponding bit count
			uint64_t sig_size = mpz_sizeinbase(s, 2);
			uint64_t p_size = mpz_sizeinbase(p, 2);
			uint64_t q_size = mpz_sizeinbase(q, 2);
			uint64_t n_size = mpz_sizeinbase(n, 2);
			uint64_t e_size = mpz_sizeinbase(e, 2);
			uint64_t d_size = mpz_sizeinbase(d, 2);
			fprintf(stderr, "username: %s\n", username);
			gmp_fprintf(stderr, "user signature (%lu bits): %Zu\n", sig_size, s);
			gmp_fprintf(stderr, "p (%lu bits): %Zu\n", p_size, p);
			gmp_fprintf(stderr, "q (%lu bits): %Zu\n", q_size, q);
			gmp_fprintf(stderr, "n - modulus (%lu bits): %Zu\n", n_size, n);
			gmp_fprintf(stderr, "e - public exponent (%lu bits): %Zu\n", e_size, e);
			gmp_fprintf(stderr, "d - private exponent (%lu bits): %Zu\n", d_size, d);
		}
		//clear randstate and temp variables
                randstate_clear();
                mpz_clears(p,q,n,e,s,d,temp_user, NULL);
                return 0;

}
