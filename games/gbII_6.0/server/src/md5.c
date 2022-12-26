/*
 *  MD5.c - RSA Data Security, Inc., MD5 message-digest algorithm
 *
 * Copyright (C) 1991-92, RSA Data Secutiry, Inc. Created 1991. All rights
 * reserved.
 *
 * License to copy and use this software is granted provided that it is
 * identified as the "RSA Data Security, Inc. MD5 Message-Digest Algorithm" in
 * all material mentioning or referencing this software or this function.
 *
 * License is also granted to make and use derivative works provided that such
 * works are identified as "derived from the RSA Data Security, Inc. MD5
 * Message-Digest Algorithm" in all material mentioning or referencing the
 * derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either the
 * merchantability of this software or the suitability of this software for any
 * particular purpose. It is provided "as is" without express or implied
 * warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */
#include "md5.h"

#include <stdio.h>
#include <string.h>

#include "config.h"

#ifdef CHAP_AUTH

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* MD5 context */
typedef struct {
    UINT4 state[4]; /* State (ABCD) */
    UINT4 count[2]; /* Number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64]; /* Input buffer */
} MD5_CTX;

/* Constants for MD5Transform routine. */

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

void MD5String(char *, char *);
void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char[16], MD5_CTX *);

static void MD5Transform(UINT4[4], unsigned char[64]);
static void Encode(unsigned char *, UINT4 *, unsigned int);
static void Decode(UINT4 *, unsigned char *, unsigned int);
static void MD5_memcpy(POINTER, POINTER, unsigned int);
static void MD5_memset(POINTER, int, unsigned int);

static unsigned char PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/*
 * FF, GG, HH, and II transforms for rounds 1, 2, 3, and 4. Rotation is separate
 * from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) {                      \
        (a) += F((b), (c), (d)) + (x) + (UINT4)(ac);    \
        (a) = ROTATE_LEFT((a), (s));                    \
        (a) += (b);                                     \
    }

#define GG(a, b, c, d, x, s, ac) {                      \
        (a) += G((b), (c), (d)) + (x) + (UINT4)(ac);    \
        (a) = ROTATE_LEFT((a), (s));                    \
        (a) += (b);                                     \
    }

#define HH(a, b, c, d, x, s, ac) {                      \
        (a) += H((b), (c), (d)) + (x) + (UINT4)(ac);    \
        (a) = ROTATE_LEFT((a), (s));                    \
        (a) += (b);                                     \
    }

#define II(a, b, c, d, x, s, ac) {                      \
        (a) += I((b), (c), (d)) + (x) + (UINT4)(ac);    \
        (a) = ROTATE_LEFT((a), (s));                    \
        (a) += (b);                                     \
    }

/* Digests a string and returns the result. */
void MD5String(char *string, char *hexdigest)
{
    MD5_CTX context;
    unsigned char digest[16];
    unsigned int len = strlen(string);
    unsigned int i;
    char hextmp[4];

    MD5Init(&context);
    MD5Update(&context, (unsigned char *)string, len);
    MD5Final(digest, &context);

    /* Converts to hexadecimal. */
    for (i = 0; i < 16; ++i) {
        sprintf(hextmp, "%02x", digest[i]);
        strcat(hexdigest, hextmp);
    }
}

/* MD5 Initialization. Begins an MD5 operation, writing a new context. */
void MD5Init(MD5_CTX *context)
{
    context->count[1] = 0;
    context->count[0] = context->count[1];

    /* Load magic initialization constants. */
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
}

/*
 * MD5 block update operation. Continues an MD5 message-digest operation,
 * processing another message block, and update the context.
 */
void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen)
{
    unsigned int i;
    unsigned int index;
    unsigned int partLen;

    /* Compute number of bytes mod 64 */
    index = (unsigned int)((context->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    context->count[0] += ((UINT4)inputLen << 3);

    if (context->count[0] < ((UINT4)inputLen << 3)) {
        ++context->count[1];
    }

    context->count[1] += ((UINT4)inputLen >> 29);
    partLen = 64 - index;

    /* Transform as many times as possible. */
    if (inputLen >= partLen) {
        MD5_memcpy((POINTER)&context->buffer[index], (POINTER)input, partLen);
        MD5Transform(context->state, context->buffer);

        for (i = partLen; (i + 63) < inputLen; i += 64) {
            MD5Transform(context->state, &input[i]);
        }

        index = 0;
    } else {
        i = 0;
    }

    /* Buffer remaining input */
    MD5_memcpy((POINTER)&context->buffer[index],
               (POINTER)&input[i],
               inputLen - i);
}

/*
 * MD5 finalization. Ends an MD5 message-digest operation, writing the message
 * digest and zeroing the context.
*/
void MD5Final(unsigned char *digest, MD5_CTX *context)
{
    unsigned char bits[8];
    unsigned int index;
    unsigned int padLen;

    /* Save number of bits */
    Encode(bits, context->count, 8);

    /* Pad out to 56 mod 64 */
    index = (unsigned int)((context->count[0] >> 3) & 0x3F);

    if (index < 56) {
        padLen = 56 - index;
    } else {
        padLen = 120 - index;
    }

    MD5Update(context, PADDING, padLen);

    /* Append length (before padding) */
    MD5Update(context, bits, 8);

    /* Store state in digest */
    Encode(digest, context->state, 16);

    /* Zero sensitive information */
    MD5_memset((POINTER)context, 0, sizeof(*context));
}

/* MD5 basic transformation. Transforms state based on block. */
static void MD5Transform(UINT4 *state, unsigned char *block)
{
    UINT4 a = state[0];
    UINT4 b = state[1];
    UINT4 c = state[2];
    UINT4 d = state[3];
    UINT4 x[16];

    Decode(x, block, 64);

    /* Round 1 */
    FF(a, b, c, d,  x[0], S11, 0xD76AA478); /*  1 */
    FF(d, a, b, c,  x[1], S12, 0xE8C7B756); /*  2 */
    FF(c, d, a, b,  x[2], S13, 0x242070DB); /*  3 */
    FF(b, c, d, a,  x[3], S14, 0xC1BDCEEE); /*  4 */
    FF(a, b, c, d,  x[4], S11, 0xF57C0FAF); /*  5 */
    FF(d, a, b, c,  x[5], S12, 0x4787C62A); /*  6 */
    FF(c, d, a, b,  x[6], S13, 0xA8304613); /*  7 */
    FF(b, c, d, a,  x[7], S14, 0xFD469501); /*  8 */
    FF(a, b, c, d,  x[8], S11, 0x698098D8); /*  9 */
    FF(d, a, b, c,  x[9], S12, 0x8B44F7AF); /* 10 */
    FF(c, d, a, b, x[10], S13, 0xFFFF5BB1); /* 11 */
    FF(b, c, d, a, x[11], S14, 0x895CD7BE); /* 12 */
    FF(a, b, c, d, x[12], S11, 0x6B901122); /* 13 */
    FF(d, a, b, c, x[13], S12, 0xFD987193); /* 14 */
    FF(c, d, a, b, x[14], S13, 0xA679438E); /* 15 */
    FF(b, c, d, a, x[15], S14, 0x49B40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d,  x[1], S21, 0xF61E2562); /* 17 */
    GG(d, a, b, c,  x[6], S22, 0xC040B340); /* 18 */
    GG(c, d, a, b, x[11], S23, 0x265E5A51); /* 19 */
    GG(b, c, d, a,  x[0], S24, 0xE9B6C7AA); /* 20 */
    GG(a, b, c, d,  x[5], S21, 0xD62F105D); /* 21 */
    GG(d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG(c, d, a, b, x[15], S23, 0xD8A1E681); /* 23 */
    GG(b, c, d, a,  x[4], S24, 0xE7D3FBC8); /* 24 */
    GG(a, b, c, d,  x[9], S21, 0x21E1CDE6); /* 25 */
    GG(d, a, b, c, x[14], S22, 0xC33707D6); /* 26 */
    GG(c, d, a, b,  x[3], S23, 0xF4D50D87); /* 27 */
    GG(b, c, d, a,  x[8], S24, 0x455A14ED); /* 28 */
    GG(a, b, c, d, x[13], S21, 0xA9E3E905); /* 29 */
    GG(d, a, b, c,  x[2], S22, 0xFCEFA3F8); /* 30 */
    GG(c, d, a, b,  x[7], S23, 0x676F02D9); /* 31 */
    GG(b, c, d, a, x[12], S24, 0x8D2A4C8A); /* 32 */

    /* Round 3 */
    HH(a, b, c, d,  x[5], S31, 0xFFFA3942); /* 33 */
    HH(d, a, b, c,  x[8], S32, 0x8771F681); /* 34 */
    HH(c, d, a, b, x[11], S33, 0x6D9D6122); /* 35 */
    HH(b, c, d, a, x[14], S34, 0xFDE5380C); /* 36 */
    HH(a, b, c, d,  x[1], S31, 0xA4BEEA44); /* 37 */
    HH(d, a, b, c,  x[4], S32, 0x4BDECFA9); /* 38 */
    HH(c, d, a, b,  x[7], S33, 0xF6BB4B60); /* 39 */
    HH(b, c, d, a, x[10], S34, 0xBEBFBC70); /* 40 */
    HH(a, b, c, d, x[13], S31, 0x289B7EC6); /* 41 */
    HH(d, a, b, c,  x[0], S32, 0xEAA127FA); /* 42 */
    HH(c, d, a, b,  x[3], S33, 0xD4EF3085); /* 43 */
    HH(b, c, d, a,  x[6], S34,  0x4881D05); /* 44 */
    HH(a, b, c, d,  x[9], S31, 0xD9D4D039); /* 45 */
    HH(d, a, b, c, x[12], S32, 0xE6DB99E5); /* 46 */
    HH(c, d, a, b, x[15], S33, 0x1FA27CF8); /* 47 */
    HH(b, c, d, a,  x[2], S34, 0xC4AC5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d,  x[0], S41, 0xF4292244); /* 49 */
    II(d, a, b, c,  x[7], S42, 0x432AFF97); /* 50 */
    II(c, d, a, b, x[14], S43, 0xAB9423A7); /* 51 */
    II(b, c, d, a,  x[5], S44, 0xFC93A039); /* 52 */
    II(a, b, c, d, x[12], S41, 0x655B59C3); /* 53 */
    II(d, a, b, c,  x[3], S42, 0x8F0CCC92); /* 54 */
    II(c, d, a, b, x[10], S43, 0xFFEFF47D); /* 55 */
    II(b, c, d, a,  x[1], S44, 0x85845DD1); /* 56 */
    II(a, b, c, d,  x[8], S41, 0x6FA87E4F); /* 57 */
    II(d, a, b, c, x[15], S42, 0xFE2CE6E0); /* 58 */
    II(c, d, a, b,  x[6], S43, 0xA3014314); /* 59 */
    II(b, c, d, a, x[13], S44, 0x4E0811A1); /* 60 */
    II(a, b, c, d,  x[4], S41, 0xF7537E82); /* 61 */
    II(d, a, b, c, x[11], S42, 0xBD3AF235); /* 62 */
    II(c, d, a, b,  x[2], S43, 0x2AD7D2BB); /* 63 */
    II(b, c, d, a,  x[9], S44, 0xEB86D391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /* Zero sensitive information */
    MD5_memset((POINTER)x, 0, sizeof(x));
}

/*
 * Encodes input (UINT4) into output (unsigned char). Assumes len is multiple of
 * 4.
 */
static void Encode(unsigned char *output, UINT4 *input, unsigned int len)
{
    unsigned int i;
    unsigned int j;

    i = 0;

    for (j = 0; j < len; j += 4) {
        output[j] = (unsigned char)(input[i] & 0xFF);
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xFF);
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xFF);
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xFF);
        ++i;
    }
}

/*
 * Decodes input (unsigned char) into output (UINT4). Assumes len is a multiple
 * of 4.
 */
static void Decode(UINT4 *output, unsigned char *input, unsigned int len)
{
    unsigned int i;
    unsigned int j;

    i = 0;

    for (j = 0; j < len; j += 4) {
        output[i] = ((UINT4)input[j])
            | (((UINT4)input[j + 1]) << 8)
            | (((UINT4)input[j + 2]) << 16)
            | (((UINT4)input[j + 3]) << 24);

        ++i;
    }
}

/* Note: Replace "for loop" with standard memcpy if possible. */
static void MD5_memcpy(POINTER output, POINTER input, unsigned int len)
{
    unsigned int i;

    for (i = 0; i < len; ++i) {
        output[i] = input[i];
    }
}

/* Note: Replace "for loop" with standard memset if possible. */
static void MD5_memset(POINTER output, int value, unsigned int len)
{
    unsigned int i;

    for (i = 0; i < len; ++i) {
        ((char *)output)[i] = (char)value;
    }
}

#endif
