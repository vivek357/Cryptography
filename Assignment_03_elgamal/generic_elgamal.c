#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef struct key {
	/*Public Key*/
	int64_t q;
	int64_t alpha;
	int64_t Ya;

	/*Private Key*/
	int64_t Xa;
}key_gen_t;

typedef struct sign {
	int64_t s1;
	int64_t s2;
	int64_t k;
}sign_t;

typedef struct hash_map {
	char m[256];
	int64_t hash_m; 
}msg_t;

typedef struct primitive_roots {
	int64_t zp[100];
	int no_of_primitive_roots;
}primitive_roots_t;

int64_t random_integer(key_gen_t gen_key)
{
	int64_t max_number = gen_key.q - 2;
	int64_t minimum_number = 2;

	return (rand() % (max_number + 1 - minimum_number) + minimum_number);
}

int64_t gcd_calculator(key_gen_t gen_key, int64_t k)
{
	int gcd;

	while (k < 1) {
		k = random_integer(gen_key);
		for (int i = 1; i <= k && i <= (gen_key.q - 1); i++) {
			if ((k % i == 0) && ((gen_key.q - 1) % i == 0))
				gcd = i;
		
			if (gcd != 1)
				k = 0;
		}
	}
	printf("%s: k = %ld, (q-1) = %ld\n", __func__, k, gen_key.q - 1);

	return k;
}

int64_t compute_inverse_k(int64_t k, int64_t q)
{
	int64_t k_inverse = 1;

	for (k_inverse = 1; k_inverse < q; k_inverse++)
		if (((k % q) * (k_inverse % q)) % q == 1)
			break;

	printf("%s: k range<1, %ld> = %ld, q = %ld, k_inverse = %ld\n",
			__func__,q-1, k, q, k_inverse);
	return k_inverse;
}

key_gen_t Key_generation(key_gen_t gen_key)
{	
	int64_t ya = 0;	
	srand(time(0));

	gen_key.Xa = random_integer(gen_key);
	ya = (pow(gen_key.alpha, gen_key.Xa));
	gen_key.Ya = ya % (gen_key.q);

	printf("%s: \nPrivate Key: Xa = %ld; \nPublic Key: q = %ld; Ya = %ld; alpha = %ld\n",
			__func__, gen_key.Xa, gen_key.q, gen_key.Ya, gen_key.alpha);
	return gen_key;
}

/* Multi Exponentiation */
int64_t power(int64_t alpha, int64_t u1)
{
	int64_t x = alpha;
	int64_t y = u1;

	int64_t result = 1;

	while (y != 0)	{
		if (y % 2 == 0) {
			x = x*x;
			y = y/2;	
		}
		else {
			result = result * x;
			y = y - 1;
		}
	}

	printf("%s: x = %ld, y = %ld : x ^ y = %ld\n\n",
			__func__, alpha, u1, result);
	return result;
}

// Checks if number is prime
int checkIfPrime (int newPrime)
{
	if (newPrime <= 1)
		return 0;

	if (newPrime > 2) {
		for (int i = 2; i < newPrime; i++) {
			if (newPrime % i == 0) {
				printf("Not a Prime Number, enter again\n\n");
				return 0;
			}
		}
	}
	return 1;
}

void findPrimefactors(int64_t *p, int64_t n, int *no_of_factor)
{
	int  i = 0, isPrime = 0;

	for (int j = 2; j <= n; j = j + 1)
	{
		if (n % j == 0) {

			isPrime = 1;
			for(int k=2; k <= j/2; k++)
			{
				if (j % k == 0)
				{
					isPrime = 0;
					break;
				}
			}

			if(isPrime==1) {
				p[i++] = j;
			}
		}
	}

	*no_of_factor = i;
}

/* 
 * Code to Generate all the primitive roots
 */
primitive_roots_t primitive_root(key_gen_t g_key, primitive_roots_t p_roots)
{
	int i = 0;
	int64_t n = g_key.q;
	int64_t prime_factor[n];
	int no_of_factor = 0;
	int64_t power = 0;
	
	int64_t phi = n - 1;
	int flag = 0;

	findPrimefactors(prime_factor, phi, &no_of_factor);
	printf("%s: phi(n) = %ld; No of factors = %d; factor list : ",
			__func__, phi, no_of_factor);

	for (int j = 0; j < no_of_factor; j++)
		printf("%ld ", prime_factor[j]);

	printf("\n\n");
	for (int64_t g = 2; g < phi; g++) {
		for (int p = 0; p < no_of_factor; p++)
		{
			power = pow(g, (phi/prime_factor[p]));
			if ((power % n) == 1)
			{
				flag = 1;	
			}
			printf("%s: itr= %d, g = %ld, prime_factor[p]: %ld, phi/prime_factor[p]: %ld, flag: %d\n",
					__func__, p, g, prime_factor[p], phi/prime_factor[p], flag);
		}
		printf("\n");
		if (flag == 0) {
			p_roots.zp[i++] = g;
			p_roots.no_of_primitive_roots = i;
		}
		flag = 0;
	}
	
	printf("\nPrimitive roots are: \n");
	for (int j = 0; j < p_roots.no_of_primitive_roots; j++)
		printf("%ld ", p_roots.zp[j]);

	printf("\n\n");
	//g_key.alpha = 10;

	return p_roots;
}

/*	Key Generation for Elgamal Digital Signature
 *	1. Choose a large prime number q
 *	2. Choose primitive element alpha of Z*p
 *	3. Choose a random integer Xa  s.t, (1 < Xa < q -1)
 *	4. Compute Ya = pow(alpha, Xa) mod q
 *	5. Private Key : Xa   && Public Key : (q, alpha, Ya)
 */
sign_t sign_generation(sign_t sg, key_gen_t gen_key, msg_t msg)
{
	int64_t k = 0;
	int64_t k_inverse = 0;
	int64_t alpha_k = 0;

	srand(time(0));

	k = gcd_calculator(gen_key, k);
	k_inverse = compute_inverse_k(k, gen_key.q - 1);

	alpha_k = pow(gen_key.alpha, k);

	sg.s1 = (alpha_k % gen_key.q);
	sg.s2 = (k_inverse * (msg.hash_m - (gen_key.Xa * sg.s1))) % (gen_key.q - 1) ; 

	if (sg.s2 < 0) {
		printf("%s: less than 0, s2 = %ld \n", __func__, sg.s2);
		sg.s2 = sg.s2 + (gen_key.q - 1);
	}

	printf("\n\n%s: q = %ld; Xa = %ld; Ya = %ld; k = %ld; k_inverse = %ld\n",
			__func__, gen_key.q, gen_key.Xa, gen_key.Ya, k, k_inverse);

	printf("%s: s1 = %ld; s2 = %ld \n", __func__, sg.s1, sg.s2);

	return sg;	
}

/*
 * Verifier Algo:
 *
 * Compute m = H(m)
 * compute V1 = alpha ^ m mod q
 * compute V2 = (Ya ^ s1) * (s1 ^ s2) mod q
 * if (V1 == V2)  then ACCEPT
 * else REJECT
 */
void verification(sign_t sg, key_gen_t key_gen, msg_t msg)
{
	int64_t v1 = 0;
	int64_t v2 = 0;
	
	int64_t alpha_m = 0;

	int64_t Ya_s1 = 0;
	int64_t s1_s2 = 0;

	printf("%s: alpha = %ld, hash value = %ld\n\n",
			__func__, key_gen.alpha, msg.hash_m);
	alpha_m = pow(key_gen.alpha, msg.hash_m);
	v1 = alpha_m % key_gen.q;

	printf("%s: Ya = %ld, s1 = %ld\n",__func__, key_gen.Ya, sg.s1);
	Ya_s1 = power(key_gen.Ya, sg.s1);
	printf("%s: s1 = %ld, s2 = %ld\n",__func__, sg.s1, sg.s2);
	s1_s2 = power(sg.s1, sg.s2);

	// as (a * b )mod q = ((a mod q) * (b mod q)) mod q
	v2 = ((Ya_s1 % key_gen.q) * (s1_s2 % key_gen.q)) % key_gen.q;
	//v2 = (Ya_s1 * s1_s2) % key_gen.q;

	printf("%s: v1 = %ld, v2 = %ld, Ya_s1 = %ld, s1_s2 = %ld\n\n",
			__func__, v1, v2, Ya_s1, s1_s2);
	if(v1 == v2)
		printf("Accepted\n");
	else
		printf("Rejected\n");

}

int main()
{
	key_gen_t gen_key;
	primitive_roots_t p_roots;
	sign_t sg;
	msg_t msg;
	int primeCheck = 0;

	do {
		printf("Enter a large prime number, q\n");
		scanf("%ld", &gen_key.q);
		printf("q = %ld\n", gen_key.q);
		primeCheck = checkIfPrime(gen_key.q);
	} while (primeCheck == 0);

	p_roots = primitive_root(gen_key, p_roots);
	printf("\n\n");
	gen_key.alpha = p_roots.zp[2];

	gen_key = Key_generation(gen_key);
	printf("\n\n");

	printf("Enter Message (Max 256 Character): ");
	scanf("%s", msg.m);

	msg.hash_m = random_integer(gen_key);
	printf("Enterred Message: %s, with Hash(m) = %ld\n\n", (msg.m), msg.hash_m);

	sg = sign_generation(sg, gen_key, msg);
	printf("\n\n");
	verification(sg, gen_key, msg);

	//multiple_expo(2, 3, 10, 9);
	//printf("%ld\n", power(2, 10));
	//printf("%ld\n", power(3, 9));

	return 0;
}
