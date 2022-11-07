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
//comment for re-push

int main(int argc, char **argv){
// initialize the default pointers for files, starting at stdin and stdout
FILE *inp_file = stdin;
FILE *out_file = stdout;
// init default public key file
char *pub_key = "rsa.pub";
bool verbose = false;
int operator;
	while ((operator = getopt(argc, argv, OPTIONS)) != -1){
		// initializes temp vars for argument parsing
		switch(operator){
			case 'i':
				// change the pointer of inp_file to the filename of whatever user-supplied filename is given.
				// Will only change the pointer if -i is called
				inp_file = fopen(optarg, "r");
				break;
			case 'o':
				// same as -i but for stdout pointer redirection instead
				out_file = fopen(optarg, "w");
				break;
			case 'n':
				// just sets file name string to the new public key file
				pub_key = optarg;
				break;
			case 'v':
				// flag for verbose out
				verbose = true;
				break;
			case 'h':
				// help msg
				fprintf(stderr, "Usage: ./encrypt [options]\n");
                                fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default:
				fprintf(stderr, "Usage: ./encrypt [options]\n");                                                                                                                                    fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");                                                                                       fprintf(stderr, "  writing the result to the specified output file.\n");                                                                                                            fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");                                                                                          fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");                                                                                        fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");                                                                                               fprintf(stderr, "    -v          : Enable verbose output.\n");                                                                                                                      fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				// help msg here too for if incorrect option is provided
				return 1;
		}
	}
		// initialize the public key file pointer
                FILE *public_file;
		// open it using the provided file name
                public_file = fopen(pub_key, "r");
		// check that both the input file and output files were succesfully opened and they are non-NULL
                if(public_file == NULL || inp_file == NULL){
                        fprintf(stderr, "File opening failure\n");
                        return 1;
                }
		// temps for calculation
		mpz_t n, e, s, temp_str;
		// defines the LOGIN_NAME_MAX global var, which is the largest size a username can be in linux, so that username is guarnateed
		// to be able to fit the username of a linux user
		LOGIN_NAME_MAX;
		// init the username string with the size of the max name size
		char username[LOGIN_NAME_MAX];
		mpz_inits(n,e,s,temp_str,NULL);
		// read the public key info
		rsa_read_pub(n, e, s, username, public_file);
		// close pubkey file
		fclose(public_file);
		if(verbose){
			//same as keygen, prints vars with their bitcount
			uint64_t sig_size = mpz_sizeinbase(s, 2);
			uint64_t n_size = mpz_sizeinbase(n, 2);
			uint64_t e_size = mpz_sizeinbase(e, 2);
			fprintf(stderr, "username: %s\n", username);
			gmp_fprintf(stderr, "user signature (%lu bits): %Zu\n", sig_size, s);
			gmp_fprintf(stderr, "n - modulus (%lu bits): %Zu\n", n_size, n);
			gmp_fprintf(stderr, "e - public exponent (%lu bits): %Zu\n", e_size, e);
		}
		// checks username and sets it to an mpz_t, then verifies it using rsa_verify
		mpz_set_str(temp_str, username, 62);
		bool ver = rsa_verify(temp_str, s, e, n);
		if(ver == false){
			fprintf(stderr, "Signature not verified.\n");
			return 1;
		}
		// if verification is cofirmed, encrypt the input file and write it to the output file
		rsa_encrypt_file(inp_file, out_file, n, e);
		// close the files
		fclose(inp_file);
		fclose(out_file);
		// clear temp vars
                mpz_clears(n,e,s, temp_str,NULL);
                return 0;

}



