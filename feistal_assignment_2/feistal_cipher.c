#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ROUNDS 8

// Defined Number of Rounds as 8
uint16_t keys[ROUNDS] = { 0xDEAD, 0xBAAD, 0xFEED, 0xCAFE, 0xDEAD, 0xD15E, 0xDECE, 0xAC1D };

typedef struct _middle_text {
	uint32_t per_round_e_str;
	uint32_t per_round_d_str;
} middle_text;

static middle_text mt[ROUNDS] = {
	{.per_round_e_str = 0, .per_round_d_str = 0},
};

uint32_t f(uint16_t block, uint16_t key) {
	block = (block >> 1);
	return block ^ key;                                                     
}

uint8_t hammingDistance(uint32_t n1, uint32_t n2)
{
	uint8_t hamming_distance = 0;
	uint32_t x = n1 ^ n2;

	while (x > 0) {
		hamming_distance += x & 1;
		x = (x >> 1);
	}

	return hamming_distance;
}

uint32_t decrypt(uint16_t left, uint16_t right)
{
        uint16_t i;
	uint16_t left1;
	uint16_t right1;

	for(i = 0; i < ROUNDS; i++)
	{
		left1 = f(left, keys[ROUNDS-i-1]) ^ right;
		right1 = left;

		if(i == (ROUNDS-1))
		{
			left = right1;
			right = left1;
			printf("Round = %u str = 0x%X\n", i,
					((uint32_t)left<<16 | right));
			mt[i].per_round_d_str = ((uint32_t)left<<16 | right);
		}
		else
		{
			left = left1;
			right = right1;
			printf("Round = %u str = 0x%X\n", i,
					((uint32_t)left<<16 | right));
			mt[i].per_round_d_str = ((uint32_t)left<<16 | right);
		}
	}
	return (uint32_t)left<<16 | right;
}

uint32_t encrypt(uint16_t left, uint16_t right)
{
	uint16_t i; 
	uint16_t left1;
	uint16_t right1;

	for(i = 0; i < ROUNDS; i++)
	{
		left1 = f(left, keys[i]) ^ right;                                
		right1 = left;                                                  

		if(i == (ROUNDS-1))
		{
			left = right1;
			right = left1;
			printf("Round = %u str = 0x%X\n", i,
					((uint32_t)left<<16 | right));
			mt[i].per_round_e_str = ((uint32_t)left<<16 | right);
		}
		else {
			left = left1;
			right = right1;                                         
			printf("Round = %u str = 0x%X\n", i,
					((uint32_t)left<<16 | right));
			mt[i].per_round_e_str = ((uint32_t)left<<16 | right);
                }                                                               
        }                                                                       
        return ((uint32_t)left<<16 | right);                                      
}

/*
 * get_substr: function to get two parts of the Text L0 and R0
 * @left : L0 part of passed text
 * @right: R0 part of passed text
 * @t_string: Text string,  can be plain text for encryption or 
 * cipher text for decryption
 */
void get_substr(uint16_t *left, uint16_t *right, uint32_t t_string)
{
	*left = (t_string>>16) & 0xFFFF;
	printf("left hex = 0x%X\n", *left);

	*right = t_string & 0xFFFF;
	printf("right hex = 0x%X\n", *right);
}

uint32_t standard_input()
{
	uint32_t text;
	int ch;

	printf("Enter your choice\n"
			"1. for default plain text : 0x7E81817E\n"
			"2. for standard input\n");
	scanf("%d", &ch);

	switch(ch) {
		case 1:
		text = 0x7E81817E;
		break;

		case 2:
		printf("Enter the 32 bit pain text in Hexadecimal form: ");
		scanf("%x", &text);
		break;

		default:
		printf("Invalid option\n");
		exit(0);
	}

	return text;
}

int main()
{
	//32 Bit Plain Text ; In Hexadecimal = 0x7E81817E
	uint32_t plain_text = 0b01111110100000011000000101111110;


	uint32_t cipher_text;
	uint32_t decoded_text;

	uint16_t *left = malloc(sizeof(uint16_t));
	uint16_t *right = malloc(sizeof(uint16_t));
	

	plain_text = standard_input();
	
	printf("plain_text hex = 0x%X\n", plain_text);
	get_substr(left, right, plain_text);
	cipher_text = encrypt(*left, *right);
	printf("cipher text hex = 0x%X\n", cipher_text);

	printf("\n-----------------Decrypt Cipher text-----------------\n");
	get_substr(left, right, cipher_text);
	decoded_text = decrypt(*left, *right);
	printf("Decoded Plain text hex = 0x%X\n", decoded_text);


	printf("\n-----------------HD[u(i),v(i)]-----------------\n");
	for (int i = 0; i < ROUNDS; i++)
		printf("Round = %u, Hamming_Distance(0x%X, 0x%X) = %u\n",
			        i,	
				mt[i].per_round_e_str,
				mt[i].per_round_d_str,
				hammingDistance(mt[i].per_round_e_str, mt[i].per_round_d_str));

	printf("\nFinal Hamming_Distance(plain_text, cipher_text) = %u\n\n",
			hammingDistance(plain_text, cipher_text));
	
	free(left);
	free(right);

	return 0;
}
