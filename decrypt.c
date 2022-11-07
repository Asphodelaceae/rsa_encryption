#include <stdio.h>
#include <limits.h>
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
#define OPTIONS "i:o:n:vh"


int main(int argc, char **argv){
	//same as encrypt.c, init default pointers for files
	FILE *inp_file = stdin;
	FILE *out_file = stdout;
	char *pub_key = "rsa.priv";
	bool verbose = false;
	int operator;
	while ((operator = getopt(argc, argv, OPTIONS)) != -1){
		// initializes temp vars for argument parsing
		switch(operator){
			case 'i':
				// change input file pointer if -i called
				inp_file = fopen(optarg, "r");
				break;
			case 'o':
				// change output file pointer if -o is called
				out_file = fopen(optarg, "w");
				break;
			case 'n':
				// string for the private key file
				pub_key = optarg;
				break;
			case 'v':
				verbose = true;
				break;
			case 'h':
				// help msg
				fprintf(stderr, "Usage: ./decrypt [options]\n");
                                fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default:
				fprintf(stderr, "Usage: ./decrypt [options]\n");
                                fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				// help msg here too if invalid option is provided
				return 1;
		}
	}
		// initialize file pointer for the private key file
                FILE *priva_file;
                priva_file = fopen(pub_key, "r");
		// checks that the input and private key files exist and can be opened
                if(priva_file == NULL || inp_file == NULL){
                        fprintf(stderr, "File opening failure\n");
                        return 1;
                }
		// init temp vals for n and d
		mpz_t n, d;
		mpz_inits(n,d,NULL);
		// reads n and d from priv key file
		rsa_read_priv(n, d, priva_file);
		fclose(priva_file);
		if(verbose){
			// same as other executables, provides n and d values and their bitcounts
			uint64_t n_size = mpz_sizeinbase(n, 2);
			uint64_t d_size = mpz_sizeinbase(d, 2);
			gmp_fprintf(stderr, "n - modulus (%lu bits): %Zu\n", n_size, n);
			gmp_fprintf(stderr, "e - public exponent (%lu bits): %Zu\n", d_size, d);
		}
		// decrypts the provided file and writes it to the output file
		rsa_decrypt_file(inp_file, out_file, n, d);
		// clear temp vars
		fclose(inp_file);
		fclose(out_file);
                mpz_clears(n,d,NULL);
                return 0;

}


