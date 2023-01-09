#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <stdint.h>
#include <assert.h>


#define WORD_SIZE 4
#define require(p, msg) assert(p && msg)

/* Size of big-numbers in bytes */
#define BN_ARRAY_SIZE	(128 / WORD_SIZE)
#define MAX_VAL		((uint64_t)0xFFFFFFFF)


/* Data-holding structure: array of uint32_t,  1024, bits (32 * 32) */
struct big_int1024
{
  uint32_t array[BN_ARRAY_SIZE];
};



/* Tokens returned by big_int1024_cmp() for value comparison */
enum { SMALLER = -1, EQUAL = 0, LARGER = 1 };


/* Initialization functions: */
void big_int1024_init(struct big_int1024* n);
void big_int1024_from_int(struct big_int1024* n, uint64_t i);
int  big_int1024_to_int(struct big_int1024* n);
void big_int1024_from_string(struct big_int1024* n, char* str, int nbytes);
void big_int1024_to_string(struct big_int1024* n, char* str, int maxsize);

/* Basic arithmetic operations: */
void big_int1024_add(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* c = a + b */
void big_int1024_sub(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* c = a - b */
void big_int1024_mul(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* c = a * b */
void big_int1024_div(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* c = a / b */
void big_int1024_mod(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* c = a % b */
void big_int1024_divmod(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c, struct big_int1024* d); /* c = a/b, d = a%b */

/* Bitwise operations: */
void big_int1024_and(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* c = a & b */
void big_int1024_or(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c);  /* c = a | b */
void big_int1024_xor(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* c = a ^ b */
void big_int1024_lshift(struct big_int1024* a, struct big_int1024* b, int nbits); /* b = a << nbits */
void big_int1024_rshift(struct big_int1024* a, struct big_int1024* b, int nbits); /* b = a >> nbits */

/* Special operators and comparison */
int  big_int1024_cmp(struct big_int1024* a, struct big_int1024* b);               /* Compare: returns LARGER, EQUAL or SMALLER */
int  big_int1024_is_zero(struct big_int1024* n);                         /* For comparison with zero */
void big_int1024_inc(struct big_int1024* n);                             /* Increment: add one to n */
void big_int1024_dec(struct big_int1024* n);                             /* Decrement: subtract one from n */
void big_int1024_pow(struct big_int1024* a, struct big_int1024* b, struct big_int1024* c); /* Calculate a^b -- e.g. 2^10 => 1024 */
void big_int1024_isqrt(struct big_int1024* a, struct big_int1024* b);             /* Integer square root -- e.g. isqrt(5) => 2*/
void big_int1024_assign(struct big_int1024* dst, struct big_int1024* src);        /* Copy src into dst -- dst := src */


#endif /* #ifndef __BIGINT_H__ */

