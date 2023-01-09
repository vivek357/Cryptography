#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "big_int1024.h"



/* Functions for shifting number in-place. */
static void one_bit_lshift(struct big_int1024* a);
static void one_bit_rshift(struct big_int1024* a);
static void word_lshift(struct big_int1024* a, int nwords);
static void word_rshift(struct big_int1024* a, int nwords);


void big_int1024_init(struct big_int1024* n)
{
  require(n, "n is null");

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    n->array[i] = 0;
  }
}


void big_int1024_from_int(struct big_int1024* n, uint64_t i)
{
  require(n, "n is null");

  big_int1024_init(n);

  n->array[0] = i;
  uint64_t num_32 = 32;
  uint64_t tmp = i >> num_32; /* bit-shift with U64 operands to force 64-bit results */
  n->array[1] = tmp;
}


int big_int1024_to_int(struct big_int1024* n)
{
  require(n, "n is null");

  int ret = 0;
  ret += n->array[0];

  return ret;
}


void big_int1024_from_string(struct big_int1024* n, char* str, int nbytes)
{
  require(n, "n is null");
  require(str, "str is null");
  require(nbytes > 0, "nbytes must be positive");
  require((nbytes & 1) == 0, "string format must be in hex -> equal number of bytes");
  require((nbytes % (sizeof(uint32_t) * 2)) == 0, "string length must be a multiple of (sizeof(uint32_t) * 2) characters");
  
  big_int1024_init(n);

  uint32_t tmp;
  int i = nbytes - (2 * WORD_SIZE); /* index into string */
  int j = 0;                        /* index into array */

  while (i >= 0)
  {
    tmp = 0;
    sscanf(&str[i], "%8x", &tmp);
    n->array[j] = tmp;
    i -= (2 * WORD_SIZE); /* step WORD_SIZE hex-byte(s) back in the string. */
    j += 1;               /* step one element forward in the array. */
  }
}


void big_int1024_to_string(struct big_int1024* n, char* str, int nbytes)
{
  require(n, "n is null");
  require(str, "str is null");
  require(nbytes > 0, "nbytes must be positive");
  require((nbytes & 1) == 0, "string format must be in hex -> equal number of bytes");

  printf("nbytes = %d\n", nbytes);
  int j = BN_ARRAY_SIZE - 1; 
  int i = 0;               


  while ((j >= 0) && (nbytes > (i + 1)))
  {
    sprintf(&str[i], "%.08x", n->array[j]);
    i += (2 * WORD_SIZE); 
    j -= 1;              
  }

  
  j = 0;
  while (str[j] == '0')
  {
    j += 1;
  }
 
   
  for (i = 0; i < (nbytes - j); ++i)
  {
    str[i] = str[i + j];
  }

  
  str[i] = 0;
}

void big_int1024_dec(struct big_int1024* n)
{
  require(n, "n is null");

  uint32_t tmp; /* copy of n */
  uint32_t res;

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    tmp = n->array[i];
    res = tmp - 1;
    n->array[i] = res;

    if (!(res > tmp))
    {
      break;
    }
  }
}


void big_int1024_inc(struct big_int1024* n)
{
  require(n, "n is null");

  uint32_t res;
  uint64_t tmp; /* copy of n */

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    tmp = n->array[i];
    res = tmp + 1;
    n->array[i] = res;

    if (res > tmp)
    {
      break;
    }
  }
}


void big_int1024_add(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  uint64_t tmp;
  int carry = 0;
  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    tmp = (uint64_t)a->array[i] + b->array[i] + carry;
    carry = (tmp > MAX_VAL);
    c->array[i] = (tmp & MAX_VAL);
  }
}


void big_int1024_sub(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  uint64_t res;
  uint64_t tmp1;
  uint64_t tmp2;
  int borrow = 0;
  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    tmp1 = (uint64_t)a->array[i] + (MAX_VAL + 1); /* + number_base */
    tmp2 = (uint64_t)b->array[i] + borrow;;
    res = (tmp1 - tmp2);
    c->array[i] = (uint32_t)(res & MAX_VAL); /* "modulo number_base" == "% (number_base - 1)" if number_base is 2^N */
    borrow = (res <= MAX_VAL);
  }
}


void big_int1024_mul(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  struct big_int1024 row;
  struct big_int1024 tmp;
  int i, j;

  big_int1024_init(c);

  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    big_int1024_init(&row);

    for (j = 0; j < BN_ARRAY_SIZE; ++j)
    {
      if (i + j < BN_ARRAY_SIZE)
      {
        big_int1024_init(&tmp);
        uint64_t intermediate = ((uint64_t)a->array[i] * (uint64_t)b->array[j]);
        big_int1024_from_int(&tmp, intermediate);
        word_lshift(&tmp, i + j);
        big_int1024_add(&tmp, &row, &row);
      }
    }
    big_int1024_add(c, &row, c);
  }
}


void big_int1024_div(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  struct big_int1024 current;
  struct big_int1024 denom;
  struct big_int1024 tmp;

  big_int1024_from_int(&current, 1);               // int current = 1;
  big_int1024_assign(&denom, b);                   // denom = b
  big_int1024_assign(&tmp, a);                     // tmp   = a

  const uint64_t half_max = 1 + (uint64_t)(MAX_VAL / 2);
  bool overflow = false;
  while (big_int1024_cmp(&denom, a) != LARGER)     // while (denom <= a) {
  {
    if (denom.array[BN_ARRAY_SIZE - 1] >= half_max)
    {
      overflow = true;
      break;
    }
    one_bit_lshift(&current);                //   current <<= 1;
    one_bit_lshift(&denom);                  //   denom <<= 1;
  }
  if (!overflow)
  {
    one_bit_rshift(&denom);                  // denom >>= 1;
    one_bit_rshift(&current);                // current >>= 1;
  }
  big_int1024_init(c);                             // int answer = 0;

  while (!big_int1024_is_zero(&current))           // while (current != 0)
  {
    if (big_int1024_cmp(&tmp, &denom) != SMALLER)  //   if (dividend >= denom)
    {
      big_int1024_sub(&tmp, &denom, &tmp);         //     dividend -= denom;
      big_int1024_or(c, &current, c);              //     answer |= current;
    }
    one_bit_rshift(&current);                //   current >>= 1;
    one_bit_rshift(&denom);                  //   denom >>= 1;
  }                                           // return answer;
}


void big_int1024_lshift(struct big_int1024* a, struct big_int1024* b, int nbits)
{
  require(a, "a is null");
  require(b, "b is null");
  require(nbits >= 0, "no negative shifts");

  big_int1024_assign(b, a);
  /* Handle shift in multiples of word-size */
  const int nbits_pr_word = (WORD_SIZE * 8);
  int nwords = nbits / nbits_pr_word;
  if (nwords != 0)
  {
    word_lshift(b, nwords);
    nbits -= (nwords * nbits_pr_word);
  }

  if (nbits != 0)
  {
    int i;
    for (i = (BN_ARRAY_SIZE - 1); i > 0; --i)
    {
      b->array[i] = (b->array[i] << nbits) | (b->array[i - 1] >> ((8 * WORD_SIZE) - nbits));
    }
    b->array[i] <<= nbits;
  }
}


void big_int1024_rshift(struct big_int1024* a, struct big_int1024* b, int nbits)
{
  require(a, "a is null");
  require(b, "b is null");
  require(nbits >= 0, "no negative shifts");
  
  big_int1024_assign(b, a);
  /* Handle shift in multiples of word-size */
  const int nbits_pr_word = (WORD_SIZE * 8);
  int nwords = nbits / nbits_pr_word;
  if (nwords != 0)
  {
    word_rshift(b, nwords);
    nbits -= (nwords * nbits_pr_word);
  }

  if (nbits != 0)
  {
    int i;
    for (i = 0; i < (BN_ARRAY_SIZE - 1); ++i)
    {
      b->array[i] = (b->array[i] >> nbits) | (b->array[i + 1] << ((8 * WORD_SIZE) - nbits));
    }
    b->array[i] >>= nbits;
  }
  
}


void big_int1024_mod(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  /*
    Take divmod and throw away div part
  */
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  struct big_int1024 tmp;

  big_int1024_divmod(a,b,&tmp,c);
}

void big_int1024_divmod(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c, struct big_int1024* d)
{
  /*
    Puts a%b in d
    and a/b in c
    mod(a,b) = a - ((a / b) * b)
    example:
      mod(8, 3) = 8 - ((8 / 3) * 3) = 2
  */
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  struct big_int1024 tmp;

  /* c = (a / b) */
  big_int1024_div(a, b, c);

  /* tmp = (c * b) */
  big_int1024_mul(c, b, &tmp);

  /* c = a - tmp */
  big_int1024_sub(a, &tmp, d);
}


void big_int1024_and(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    c->array[i] = (a->array[i] & b->array[i]);
  }
}


void big_int1024_or(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    c->array[i] = (a->array[i] | b->array[i]);
  }
}


void big_int1024_xor(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    c->array[i] = (a->array[i] ^ b->array[i]);
  }
}


int big_int1024_cmp(struct big_int1024* a, struct big_int1024* b)
{
  require(a, "a is null");
  require(b, "b is null");

  int i = BN_ARRAY_SIZE;
  do
  {
    i -= 1; /* Decrement first, to start with last array element */
    if (a->array[i] > b->array[i])
    {
      return LARGER;
    }
    else if (a->array[i] < b->array[i])
    {
      return SMALLER;
    }
  }
  while (i != 0);

  return EQUAL;
}


int big_int1024_is_zero(struct big_int1024* n)
{
  require(n, "n is null");

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    if (n->array[i])
    {
      return 0;
    }
  }

  return 1;
}


void big_int1024_pow(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c)
{
  require(a, "a is null");
  require(b, "b is null");
  require(c, "c is null");

  struct big_int1024 tmp;

  big_int1024_init(c);

  if (big_int1024_cmp(b, c) == EQUAL)
  {
    /* Return 1 when exponent is 0 -- n^0 = 1 */
    big_int1024_inc(c);
  }
  else
  {
    struct big_int1024 bcopy;
    big_int1024_assign(&bcopy, b);

    /* Copy a -> tmp */
    big_int1024_assign(&tmp, a);

    big_int1024_dec(&bcopy);
 
    /* Begin summing products: */
    while (!big_int1024_is_zero(&bcopy))
    {

      /* c = tmp * tmp */
      big_int1024_mul(&tmp, a, c);
      /* Decrement b by one */
      big_int1024_dec(&bcopy);

      big_int1024_assign(&tmp, c);
    }

    /* c = tmp */
    big_int1024_assign(c, &tmp);
  }
}

void big_int1024_isqrt(struct big_int1024 *a, struct big_int1024* b)
{
  require(a, "a is null");
  require(b, "b is null");

  struct big_int1024 low, high, mid, tmp;

  big_int1024_init(&low);
  big_int1024_assign(&high, a);
  big_int1024_rshift(&high, &mid, 1);
  big_int1024_inc(&mid);

  while (big_int1024_cmp(&high, &low) > 0) 
  {
    big_int1024_mul(&mid, &mid, &tmp);
    if (big_int1024_cmp(&tmp, a) > 0) 
    {
      big_int1024_assign(&high, &mid);
      big_int1024_dec(&high);
    }
    else 
    {
      big_int1024_assign(&low, &mid);
    }
    big_int1024_sub(&high,&low,&mid);
    one_bit_rshift(&mid);
    big_int1024_add(&low,&mid,&mid);
    big_int1024_inc(&mid);
  }
  big_int1024_assign(b,&low);
}


void big_int1024_assign(struct big_int1024* dst, struct big_int1024* src)
{
  require(dst, "dst is null");
  require(src, "src is null");

  int i;
  for (i = 0; i < BN_ARRAY_SIZE; ++i)
  {
    dst->array[i] = src->array[i];
  }
}


/* Private / Static functions. */
static void word_rshift(struct big_int1024* a, int nwords)
{
  /* Naive method: */
  require(a, "a is null");
  require(nwords >= 0, "no negative shifts");

  int i;
  if (nwords >= BN_ARRAY_SIZE)
  {
    for (i = 0; i < BN_ARRAY_SIZE; ++i)
    {
      a->array[i] = 0;
    }
    return;
  }

  for (i = 0; i < BN_ARRAY_SIZE - nwords; ++i)
  {
    a->array[i] = a->array[i + nwords];
  }
  for (; i < BN_ARRAY_SIZE; ++i)
  {
    a->array[i] = 0;
  }
}


static void word_lshift(struct big_int1024* a, int nwords)
{
  require(a, "a is null");
  require(nwords >= 0, "no negative shifts");

  int i;
  /* Shift whole words */
  for (i = (BN_ARRAY_SIZE - 1); i >= nwords; --i)
  {
    a->array[i] = a->array[i - nwords];
  }
  /* Zero pad shifted words. */
  for (; i >= 0; --i)
  {
    a->array[i] = 0;
  }  
}

static void one_bit_lshift(struct big_int1024* a)
{
  require(a, "a is null");

  int i;
  for (i = (BN_ARRAY_SIZE - 1); i > 0; --i)
  {
    a->array[i] = (a->array[i] << 1) | (a->array[i - 1] >> ((8 * WORD_SIZE) - 1));
  }
  a->array[0] <<= 1;
}

static void one_bit_rshift(struct big_int1024* a)
{
  require(a, "a is null");

  int i;
  for (i = 0; i < (BN_ARRAY_SIZE - 1); ++i)
  {
    a->array[i] = (a->array[i] >> 1) | (a->array[i + 1] << ((8 * WORD_SIZE) - 1));
  }
  a->array[BN_ARRAY_SIZE - 1] >>= 1;
}

