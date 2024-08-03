Compilation Instructions:

Compile with the words "make" in the command line when all necessary files shown below
are present in the current directory. It will generate 3 executables, keygen, encrypt,
and decrypt. Keygen will create files containing public and private keys. Encrypt will
take files, encrypt them, and output their ciphertext to a specified output file.
Decrypt will take a specified infile and decrypt its contents and write them to an
outfile."make clean" will remove all derived files from the current file directory
 "make keygen", "make encrypt", and "make decrypt" will make only keygen, encrypt, or
decrypt, respectively. "Make cleankeys" will remove existing public and private key
files.

Executable Instructions:

Usage: ./keygen [options]  
 ./keygen generates a public / private key pair, placing the keys into the public and private                                                                                        key files as specified below. The keys have a modulus (n) whose length is specified in\
 the program options.\
    -s <seed>   : Use <seed> as the random number seed. Default: time()\
    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\
    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\
    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\
    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\
    -v          : Enable verbose output.\
    -h          : Display program synopsis and usage.\

Usage: ./encrypt [options]\
 ./encrypt encrypts an input file using the specified public key file,\
 writing the result to the specified output file.\
    -i <infile> : Read input from <infile>. Default: standard input.\
    -b <bits>   : Write output to <outfile>. Default: standard input.\
    -i <iters>  : Public key is in <keyfile>. Default: rsa.pub.\
    -v          : Enable verbose output.\
    -h          : Display program synopsis and usage.\

Usage: ./decrypt [options]\
 ./decrypt decrypts an input file using the specified private key file,\
 writing the result to the specified output file.\
    -i <infile> : Read input from <infile>. Default: standard input.\
    -b <bits>   : Write output to <outfile>. Default: standard input.\
    -i <iters>  : Private key is in <keyfile>. Default: rsa.pub.\
    -v          : Enable verbose output.\
    -h          : Display program synopsis and usage.\


Deliverables:

Decrypt.c - Contains implementation of decryption of files in main()\
Encrypt.c - Contains implementation of encryption of files in main()\
Keygen.c - Contains implementation of public and private key generation in main()\
Numtheory.c - Contains the implementation of number theory functions\
Numtheory.h - Specifies the interface for the number theory functions.\
Randstate.c - Contains implementation of the random state interface for the RSA\
library and number theory functions\
Randstate.h - Specifies the interface for initializing and clearing the random state\
Rsa.c - This contains the implementation of the RSA library.\
Rsa.h - Specifies the interface for the RSA library\
Makefile - Compilation file that specifies the building and executable creation process\
of all files contained in this directory.\
README.md - This file, contains compilation and runtime instructions\
DESIGN.pdf - Description of design and design process for my program.\
WRITEUP.pdf - Description of testing methodology for the program and my overall\
takeaways from it.
