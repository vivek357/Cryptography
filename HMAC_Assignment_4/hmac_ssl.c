#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <err.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

#define INNER_PADDING '\x36'
#define OUTER_PADDING '\x5c'

#define BLOCK_LENGTH 64

#define DIGEST_LENGTH 32

static void usage( const char * const prog ) {
	printf("\nPlease run as per the Usage:\n");
	errx( 1, "usage: %s <key>\n", prog );
}

static void normalize_key(uint8_t * const key, const char * const input_key) {
	const size_t len = strlen(input_key);

	if(len <= BLOCK_LENGTH) {
		memcpy(key, input_key, len);
		memset(key + len, '\0', BLOCK_LENGTH - len);
	}
	else {
		SHA256_CTX s;
		SHA256_Init(&s);
		SHA256_Update(&s, input_key, len);
		SHA256_Final(key, &s);
		memset(key + DIGEST_LENGTH, '\0', BLOCK_LENGTH - DIGEST_LENGTH);
	}
}

void Hmac_Sha256(uint8_t *key)
{
	SHA256_CTX inner_s;
	SHA256_Init(&inner_s);

	uint8_t inner_key[BLOCK_LENGTH];
	uint8_t outer_key[BLOCK_LENGTH];
	for( size_t i = 0; i < BLOCK_LENGTH; i++ ) {
		inner_key[i] = key[i] ^ INNER_PADDING;
		outer_key[i] = key[i] ^ OUTER_PADDING;
	}

	SHA256_Update(&inner_s, inner_key, BLOCK_LENGTH);

	char buf[BUFSIZ];

	printf("Enter the message Buffer\n");
	scanf("%s", buf);

	SHA256_Update( &inner_s, buf, strlen(buf) );

	unsigned char inner_hash[DIGEST_LENGTH];
	SHA256_Final(inner_hash, &inner_s);

	SHA256_CTX outer_s;
	SHA256_Init(&outer_s);
	SHA256_Update(&outer_s, outer_key, BLOCK_LENGTH);
	SHA256_Update(&outer_s, inner_hash, DIGEST_LENGTH);

	unsigned char hmac[DIGEST_LENGTH];
	SHA256_Final(hmac, &inner_s);


	printf("\nGenerated HMAC using SHA256\n");
	for( size_t i = 0; i < DIGEST_LENGTH; i++ )
		printf( "%02x", hmac[i] );
	putchar( '\n' );

}

void Hmac_Md5(uint8_t *key)
{
	MD5_CTX inner_s;
	MD5_Init(&inner_s);

	uint8_t inner_key[BLOCK_LENGTH];
	uint8_t outer_key[BLOCK_LENGTH];
	for( size_t i = 0; i < BLOCK_LENGTH; i++ ) {
		inner_key[i] = key[i] ^ INNER_PADDING;
		outer_key[i] = key[i] ^ OUTER_PADDING;
	}

	MD5_Update(&inner_s, inner_key, BLOCK_LENGTH);

	char buf[BUFSIZ];

	printf("Enter the message Buffer\n");
	scanf("%s", buf);

	MD5_Update( &inner_s, buf, strlen(buf) );

	unsigned char inner_hash[DIGEST_LENGTH];
	MD5_Final(inner_hash, &inner_s);

	MD5_CTX outer_s;
	MD5_Init(&outer_s);
	MD5_Update(&outer_s, outer_key, BLOCK_LENGTH);
	MD5_Update(&outer_s, inner_hash, DIGEST_LENGTH);

	unsigned char hmac[DIGEST_LENGTH];
	MD5_Final(hmac, &inner_s);


	printf("\nGenerated HMAC using MD5\n");
	for( size_t i = 0; i < DIGEST_LENGTH; i++ )
		printf( "%02x", hmac[i] );
	putchar( '\n' );
}

int main( int argc, char ** argv )
{
	if (argc != 2)
		usage(argv[0]);

	uint8_t key[BLOCK_LENGTH];
	normalize_key(key, argv[1]);

	printf("\n------------HMAC WITH SHA256 -------------\n");
	Hmac_Sha256(key);

	printf("\n");
	printf("\n------------  HMAC WITH MD5 --------------\n");
	Hmac_Md5(key);
	printf("\n");

	return 0;
}
