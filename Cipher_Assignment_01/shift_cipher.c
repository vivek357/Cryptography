#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define ALPHABETS 26

char cipher_text[] = "xultpaajcxitltlxaarpjhtiwtgxktghidhipxciwtvgtpilpitghlxiwiwtxgqadds";


typedef struct
{
	int freq;			//Frequency of Alphabet in our Cipher Text
	int isVisited;			//To go through/Print the freq per character only single time
} cipher_freq;

//Default initialization to 0
static cipher_freq freq_in_cipher_text[ALPHABETS] = {
	{.freq = 0, .isVisited = 0, },
};


void print_calculated_freq()
{
	int i = 0;

	while (cipher_text[i] != '\0')
	{
		if (freq_in_cipher_text[cipher_text[i]].isVisited != 1)
		{
			printf("freq of %c is %d\n",
				cipher_text[i],
				freq_in_cipher_text[cipher_text[i]].freq);

		}
		freq_in_cipher_text[cipher_text[i]].isVisited = 1;
		i++;
	}

}

void frequency_calculator()
{
	int i = 0;
	while (cipher_text[i++] != '\0')
	{
		freq_in_cipher_text[cipher_text[i]].freq += 1;
	}

	printf("\n");
}

void decrypt_cipher(int shift)
{
	int i = 0;
	int ch = 0;
	int diff = 0;
	printf("Cipher text is:\n");
	printf("%s\n", cipher_text);
	printf("Decoded Text is: \n");
	while (cipher_text[i] != '\0')
	{
		ch = (cipher_text[i] - 'a');					//Getting letter position in Alphabet Seq from ASCII
		
		diff = (ch - shift);							
		if (diff >= 0) {
			ch = diff;
			ch = ('a' + ch);
		}
		else {
			/* if total number of Element  = n, then
			 * rth position from last is equal to
			 * (n-r)th position from first
			 * */
			ch = ('a' + (ALPHABETS + diff)); 
		}

		printf("%c", ch);
		i++;
	}
	printf("\n");
}

void cipher_compute_engine(char ch)
{
	int i = 0;
	int j = 0;
	int max_freq = 0;
	char max_freq_alphabet;
	int shift = 0;

	
	max_freq = 0;
	for (i = 0; i < strlen(cipher_text); i++)
	{
		if (freq_in_cipher_text[cipher_text[i]].freq > max_freq) 
		{
			max_freq = freq_in_cipher_text[cipher_text[i]].freq;
			max_freq_alphabet = cipher_text[i];
		}
	}


	printf("max_freq_alphabet = %c\n", max_freq_alphabet);

	shift = max_freq_alphabet - ch;
	printf("shift = '%d'\n", shift);	
	decrypt_cipher(shift);
	printf("\n");
}


int main()
{
	frequency_calculator();
	print_calculated_freq();

	/*
	 * Passing Letters as per the relative letter frequencies
	 * of the english language
	 */
	printf("\n--------------------e----------------");

	cipher_compute_engine('e');
	printf("\n");
/*	
 *	printf("\n--------------------t----------------");
 *	cipher_compute_engine('t');
 *	printf("\n");
 *	printf("\n--------------------a----------------");
 *	cipher_compute_engine('a');
 *	printf("\n");
 *	printf("\n--------------------o----------------");
 *	cipher_compute_engine('o');
 *	printf("\n");
 *	printf("\n--------------------n----------------");
 *	cipher_compute_engine('n');
 *	printf("\n");
 */
 	return 0;	
}

/* Solution
 *
 * --------------------e----------------max_freq_alphabet = t
 *  shift = '15'
 *  Cipher text is:
 *  xultpaajcxitltlxaarpjhtiwtgxktghidhipxciwtvgtpilpitghlxiwiwtxgqadds
 *  Decoded Text is: 
 *  ifweallunitewewillcausetheriverstostainthegreatwaterswiththeirblood
 * 
 */
