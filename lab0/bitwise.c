#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BIT(n) (1<<(n))

char *byte2bin(uint8_t n, char *binstr) 
{
    // one element per bit (0/1)
    uint8_t binary[8];

    int i = 0;
    for(i = 0; i < 8; i++) {
        binary[i] = n % 2;
        n = n / 2;
    }

    // printing binary array in reverse order
    for (int j = 7, k= 0; j >= 0; j--, k++)
        sprintf(binstr + k, "%d", binary[j]);
        
	return binstr;
}

void print_usage(char *name) {
	printf("Usage: %s <action> <byte> <bit no>\n", name);
	printf("\tWhere: <action> one of 'h'|'l'|'r'|'s'|'t'\n"
		"\t\t <byte> 8-bit integer in hexadecimal\n"
		"\t\t <bit no> of bit to operate on\n");
}

int main(int argc, char *argv[])
{
	char* a;	// action: 'h', 'l', 'r', 's', 't'
	unsigned long n; // value to convert must be smaller than 256
	int bit; // bit to operate on: must be between 0 an 7
    char binary[9]; // array for binary representation of n,
					//  remember that in C strings are terminated with a 0
 
	// Validate command line arguments

    if(argc != 4){
        printf("Incorrect number of arguments\n");
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "h")==0 || strcmp(argv[1], "l")==0 || strcmp(argv[1], "t")==0 || strcmp(argv[1], "r")==0 || strcmp(argv[1], "s")==0){
        a = argv[1];
    }
    else{
        printf("Incorrect mode\n");
        print_usage(argv[0]);
        return 1;
    }


    if (atoi(argv[2]) >= 256 || atoi(argv[2]) < 0){
        printf("Value to convert is out of range\n");
        print_usage(argv[0]);
        return 1;
    }
    else n = atoi(argv[2]);

	if(atoi(argv[3]) > 7 || atoi(argv[3]) < 0){
        printf("Bit to test/change is out of range\n");
        print_usage(argv[0]);
        return 1;
    }
    else bit =  atoi(argv[3]);


	// Print to stdout the binary representation of n
    
    byte2bin((uint8_t)n, binary);
    printf("Inserted value: %s\n", binary);
    
	// Do what the user asked and print the result

    uint8_t bin_tool = BIT(bit);
    uint8_t result = 0;

    if (strcmp(a, "h")==0){ 
        result = n & bin_tool;
    }
    else if (strcmp(a, "l")==0){
        result = !(n & bin_tool);
    }
    else if(strcmp(a, "t")==0){
        result = n ^ bin_tool;
    }
    else if(strcmp(a, "s")==0){
        result = n | bin_tool;
    }
    else if (strcmp(a, "r")==0){
        result = n & ~bin_tool;
    }    
    char strres[9];
    byte2bin(result, strres);

    printf("Result post-operation: %s\n", strres);
    
    return 0;
}

