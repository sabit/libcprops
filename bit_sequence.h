#ifndef _CP_BIT_SEQUENCE_H
#define _CP_BIT_SEQUENCE_H

#include "common.h"

typedef struct _cp_bit_sequence
{
	unsigned char *bits;
	int length;
} cp_bit_sequence;

#define BYTECOUNT(b) (((b)->length + 7) >> 3)

CPROPS_DLL
cp_bit_sequence *cp_bit_sequence_create(int length, unsigned char *bits);

/**
 * convenience function for debugging - initialize from a string that looks 
 * like "1010101" (actually any non-null character but '1' is interpreted as '0')
 */
cp_bit_sequence *cstr_to_bit_sequence(char *str);

CPROPS_DLL
void cp_bit_sequence_destroy(cp_bit_sequence *seq);

CPROPS_DLL
cp_bit_sequence *cp_bit_sequence_dup(cp_bit_sequence *seq);

CPROPS_DLL
cp_bit_sequence *cp_bit_sequence_cpy(cp_bit_sequence *dst, cp_bit_sequence *src);

CPROPS_DLL
cp_bit_sequence *cp_bit_sequence_cat(cp_bit_sequence *head, cp_bit_sequence *tail);

/**
 * shift bits left by the specified count, resulting in a shorter bit sequence
 */
CPROPS_DLL
int cp_bit_sequence_shift_left(cp_bit_sequence *seq, int count);

/**
 * compares bit sequences a and b, strcmp semantics. if pos is not null it is
 * set to the index of the first differing bit. if the sequences are identical
 * to the length they are defined, and pos is not null, it is set to the length
 * of the shorter sequence - e.g. 010 and 0101 are identical to the third bit, 
 * hence *pos is set to 3.
 */
CPROPS_DLL
int cp_bit_sequence_cmp(cp_bit_sequence *a, cp_bit_sequence *b, int *pos);

#define cp_bit_sequence_length(seq) (seq)->length

CPROPS_DLL
void cp_bit_sequence_dump(cp_bit_sequence *seq);

CPROPS_DLL
char *cp_bit_sequence_to_string(cp_bit_sequence *seq);

#endif /* _CP_BIT_SEQUENCE_H */

