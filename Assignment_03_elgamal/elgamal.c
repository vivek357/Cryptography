#include <stdio.h>
#include <string.h> /* for memcpy */
#include "big_int1024.h"

struct big_int1024 multi_exponentiation_function(struct big_int1024* a, struct big_int1024* b, struct big_int1024* n, struct big_int1024* res)
{
  big_int1024_from_int(res, 1); /* r = 1 */

  struct big_int1024 tmpa;
  struct big_int1024 tmpb;
  struct big_int1024 tmp;
  big_int1024_assign(&tmpa, a);
  big_int1024_assign(&tmpb, b);

  while (1)
  {
    if (tmpb.array[0] & 1)     /* if (b % 2) */
    {
      big_int1024_mul(res, &tmpa, &tmp);  /*   r = r * a % m */
      big_int1024_mod(&tmp, n, res);
    }
    big_int1024_rshift(&tmpb, &tmp, 1); /* b /= 2 */
    big_int1024_assign(&tmpb, &tmp);

    if (big_int1024_is_zero(&tmpb))
      break;

    big_int1024_mul(&tmpa, &tmpa, &tmp);
    big_int1024_mod(&tmp, n, &tmpa);
  }

  return *res;
}


void test_elgamal(void)
{
  char buf[8192];
  struct big_int1024 Q, Xa, G, Ya;


  const int xa = 2053;
  const int q = 8209;
  const int g = 17;
  int64_t m_result;

  big_int1024_init(&Q);
  big_int1024_init(&Xa);
  big_int1024_init(&G);
  big_int1024_init(&Ya);

  big_int1024_from_int(&Q, q);
  big_int1024_from_int(&Xa, xa);
  big_int1024_from_int(&G, g);

  printf("\n");


  printf("  %d ^ %d mod %d = ? \n", g, xa, q);
  multi_exponentiation_function(&G, &Xa, &Q, &Ya);
  m_result = big_int1024_to_int(&Ya);
  big_int1024_to_string(&Ya, buf, sizeof(buf));
//  printf("  %d ^ %d mod %d = %s \n", g, xa, q, buf);
  printf("  %d ^ %d mod %d = %d \n", g, xa, q, m_result);

  printf("\n");
}




static void test_elgamal1024(void)
{
  // a large prime number, q (1024 bits)
  char prime_number[]  = "86F5CA03DCFEB225063FF830A0C769B9DD9D6153AD91D7CE27F787C43278B447E6533B86B18BED6E8A48B784A14C252C5BE0DBF60B86D6385BD2F12FB763ED8873ABFD3F5BA2E0A8C0A59082EAC056935E529DAF7C610467899C77ADEDFC846C881870B7B19B2B58F9BE0521A17002E3BDD6B86685EE90B3D9A1B02B782B1779";

  //a sufficiently large prime number Xa, s.t., (1 <  Xa < q-1)
  char random_integer[] = "996F967F6C8E388D9E28D01E205FBA957A5698B1";

  // a generator, g for the multiplicative subgroup of order Xa of integers mod q
  char alpha[] = "07B0F92546150B62514BB771E2A0C0CE387F03BDA6C56B505209FF25FD3C133D89BBCD97E904E09114D9A7DEFDEADFC9078EA544D2E401AEECC40BB9FBBF78FD87995A10A1C27CB7789B594BA7EFB5C4326A9FE59A070E136DB77175464ADCA417BE5DCE2F40D10A46A3A3943F26AB7FD9C0398FF8C76EE0A56826A8A88F1DBD";
 
  char buf[8192];

  struct big_int1024 q;		/* prime_number, q */
  struct big_int1024 Xa;	/* private key */
  struct big_int1024 g;		/* generator or alpha*/
//  struct big_int1024 m;	/* clear text message */
  struct big_int1024 Ya;	/* Ya = g ^ Xa mod q */
  struct big_int1024 q_minus_1;
  struct big_int1024 k;

  char one_str[] = "1";
  struct big_int1024 one;
  int i = 0;
//  int m = 54321;		/* Consider it as a Hash(m) */

  big_int1024_init(&q);
  big_int1024_init(&Xa);
  big_int1024_init(&g);
//  big_int1024_init(&m);
  big_int1024_init(&Ya);
  big_int1024_init(&k);
  big_int1024_init(&q_minus_1);

  big_int1024_from_string(&q, prime_number,  256);
  big_int1024_from_string(&Xa, random_integer, 256);
  big_int1024_from_string(&g, alpha, 256);
  big_int1024_from_string(&g, alpha, 256);
  big_int1024_from_string(&one, one_str, 256);

//  compute Ya
  multi_exponentiation_function(&g, &Xa, &q, &Ya);
  printf("  Done...\n\n");

  big_int1024_to_string(&Ya, buf, sizeof(buf));
  while(buf[i++] != 0)
      printf("Ya = %c \n", buf[i]);

// Compute S1 = g ^ k mod q
  big_int1024_sub(&q, &one, &q_minus_1);
  big_int1024_sub(&q_minus_1, &one, &k);

  multi_exponentiation_function(&g, &k, &q, &Ya);

 
  printf("\n");

}


int main()
{
  printf("\n");
  printf("Testing Elgamal implemented with big_int1024. \n");

  test_elgamal();
  test_elgamal1024();

  printf("\n");
  printf("\n");



  return 0;
}

