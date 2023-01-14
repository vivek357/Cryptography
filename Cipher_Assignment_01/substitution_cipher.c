#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define ALPHABETS 26

char cipher_text[] = "lrvmnir bpr sumvbwvr jx bpr lmiwv yjeryrkbi jx qmbm wi bpr xjvni mkd ymibrut jx irhx wi bpr riirkvr jx ymbinlmtmipw utn qmumbr dj w ipmhh but bj rhnvwdmbr bpr yjeryrkbi jx bpr qmbm mvvjudwko bj yt wkbrusurbmbwjk lmird jk xjubt trmui jx ibndt wb wi kjb mk rmit bmiq bj rashmwk rmvp yjeryrkb mkd wbi iwokwxwvmkvr mkd ijyr ynib urymwk nkrashmwkrd bj ower m vjyshrbr rashmkmbwjk jkr cjnhd pmer bj lr fnmhwxwrd mkd wkiswurd bj invp mk rabrkb bpmb pr vjnhd urmvp bpr ibmbr jx rkhwopbrkrd ywkd vmsmlhr jx urvjokwgwko ijnkdhrii ijnkd mkd ipmsrhrii ipmsr w dj kjb drry ytirhx bpr xwkmh mnbpjuwbt lnb yt rasruwrkvr cwbp qmbm pmi hrxb kj djnlb bpmb bpr xjhhjcwko wi bpr sujsru msshwvmbwjk mkd wkbrusurbmbwjk w jxxru yt bprjuwri wk bpr pjsr bpmb bpr riirkvr jx jqwkmcmk qmumbr cwhh urymwk wkbmvb";

char global_freq[] = {'e','t','a','o','i','n','s','h','r','d','l','c','u','m',
	'f','w','g','y','b','p','v','k','x','q','j','z'};

char freq_analysis_text[sizeof(cipher_text)];

typedef struct
{
	int freq;			//Frequency of Alphabet in our Cipher Text
	int isVisited;			//To go through/Print the freq per character only single time
} cipher_freq;

//Default initialization to 0
static cipher_freq freq_in_cipher_text[ALPHABETS] = {
	{.freq = 0, .isVisited = 0, },
};

struct freq_array {
	char a;
	int freq;
} f_arr[26];

int count = 0;

char letter_manual_swap[][14] = {{'n','i'}, {'n','o'}, {'g','y'}, {'g','b'},
				 {'u','f'}, {'p','g'}, {'j','v'}, {'p','k'},
				 {'w','p'}, {'w','x'}, {'q','z'}, {'j','v'},
				 {'j','q'}
				};

int comp_freq (const void * elem1, const void * elem2) 
{
	struct freq_array *f = (struct freq_array *)elem1;
	struct freq_array *s = (struct freq_array *)elem2;
    
	return (s->freq - f->freq);
}

int comp_letter (const void * elem1, const void * elem2) 
{
	struct freq_array *f = (struct freq_array *)elem1;
	struct freq_array *s = (struct freq_array *)elem2;
    
	return (f->a - s->a);
}

void print_calculated_freq()
{
	int i = 0;
	while (cipher_text[i] != '\0')
	{
		if (freq_in_cipher_text[cipher_text[i]].isVisited != 1)
		{
//			printf("%d = freq of %c is %d\n", i,
//				cipher_text[i],
//				freq_in_cipher_text[cipher_text[i]].freq);

			if (' ' == cipher_text[i]) {
				i++;
				continue;
			}
			f_arr[count].a = cipher_text[i];	
			f_arr[count].freq = freq_in_cipher_text[cipher_text[i]].freq;
			count = count + 1;
		}
		freq_in_cipher_text[cipher_text[i]].isVisited = 1;
		i++;
	}

	printf("===============================================\n");
	for (i = 0; i < count; i++) {
		printf("%d = freq of %c is %d\n", i, 
				f_arr[i].a, f_arr[i].freq);
	}

	while (i < ALPHABETS) {
		f_arr[i].a = 'a' + i;
		f_arr[i].freq = 0;
		i++;
	}


	printf("======================SORTED BY FREQUENCY====================\n");
	qsort (f_arr, count, sizeof(f_arr[0]), comp_freq);
	for (i = 0; i < 26; i++)
		printf("%d = freq of %c is %d\n", i, 
				f_arr[i].a, f_arr[i].freq);
	
/*	qsort (f_arr, count, sizeof(f_arr[0]), comp_letter);
 *	printf("======================SORTED BY ALBHABET=====================\n");
 *	for (int i = 0; i < count; i++)
 *	printf("%d = freq of %c is %d\n", i, f_arr[i].a, f_arr[i].freq);
 */
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

void cipher_compute_engine(char *cipher_text)
{
	int i = 0;
	int j = 0;

	strncpy(freq_analysis_text, cipher_text, strlen(cipher_text) + 1);
	for (i = 0; i < ALPHABETS; i++) {

		for (j = 0; j < strlen(cipher_text); j++)
		{
			if (f_arr[i].a == cipher_text[j])
			{
				freq_analysis_text[j] = global_freq[i];				
			}	
		}
	}

	printf("\n%s\n", freq_analysis_text);
	printf("\n");
}

void swap_engine(int *pos1, int *pos2, int k, int l ,char l1, char l2)
{
	for (int i = 0; i < strlen(freq_analysis_text); i++)
	{
		for(int j = 0; j < k; j++)
			freq_analysis_text[pos1[j]] = l2;
		
		for(int j = 0; j < l; j++)
			freq_analysis_text[pos2[j]] = l1;
	}

	printf("\n%s\n", freq_analysis_text);
	printf("\n");
}

void swap_letter_manually()
{
	char temp;
	int pos1[100], pos2[100] = {-1};
	int k, l = 0;

	for (int i = 0; i < 14; i++)
	{	k = 0;
		l = 0;
		for (int j = 0; j < strlen(freq_analysis_text); j++)
		{
			if (freq_analysis_text[j] == letter_manual_swap[i][0])
			{
				pos1[k++] = j;
			}			
			if (freq_analysis_text[j] == letter_manual_swap[i][1])
			{
				pos2[l++] = j;
			}
		}

		printf("\nCipher Value after Manual Substition : %d\n", i);
		swap_engine(pos1, pos2, k, l ,letter_manual_swap[i][0], letter_manual_swap[i][1]);
		printf("\n");
	}
}

int main()
{
	frequency_calculator();
	print_calculated_freq();

	printf("\n---------SWAP LETTERS AS PER THE FREQUENCY ANALYSIS----------------\n");

	cipher_compute_engine(cipher_text);
	printf("\n");

	printf("\n--------Statistical Analysis generated a Text i.e., stil Garbled----------------------\n");
	printf("--------But Some words stand out, so doing Maunual Swapping as per defined Array----------------------\n\n");
	swap_letter_manually(freq_analysis_text);

	return 0;	
}

