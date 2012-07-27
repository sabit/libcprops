#include "bit_sequence.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

cp_bit_sequence *cp_bit_sequence_create(int length, unsigned char *bits)
{
	int bytecount;
	cp_bit_sequence *seq = (cp_bit_sequence *) malloc(sizeof(cp_bit_sequence));
	if (seq == NULL) return NULL;

	bytecount = (length + 7) >> 3;
	seq->bits = (unsigned char *) malloc(bytecount);
	if (seq->bits == NULL)
	{
		free(seq);
		return NULL;
	}

	if (bits != NULL)
		memcpy(seq->bits, bits, bytecount);

	seq->length = length;

	return seq;
}

/*
 * initialize from a string that looks like "1010101" - convenience function 
 * for debugging.
 */
cp_bit_sequence *cstr_to_bit_sequence(char *str)
{
	int i, pos, len, bytecount, bit;
	cp_bit_sequence *seq;

	seq = (cp_bit_sequence *) malloc(sizeof(cp_bit_sequence));
	if (seq == NULL) return NULL;

	len = strlen(str);
	bytecount = (len + 7) / 8;
	
	seq->bits = (unsigned char *) calloc(1, bytecount);
	if (seq->bits == NULL)
	{
		free(seq);
		return NULL;
	}

	seq->length = len;
	for (i = 0, pos = 0, bit = 0x80; i < len; i++)
	{
		if (str[i] == '1')
			seq->bits[pos] |= bit;
		bit >>= 1;
		if (bit == 0)
		{
			bit = 0x80;
			pos++;
		}
	}
		
	return seq;
}

void cp_bit_sequence_destroy(cp_bit_sequence *seq)
{
	if (seq)
	{
		free(seq->bits);
		free(seq);
	}
}

void cp_bit_sequence_dump(cp_bit_sequence *seq)
{
	int i, pos;

	printf("bit sequence: %d bits\n", seq->length);

	for (i = 0, pos = 0; i < seq->length; i++)
	{
		if (seq->bits[pos] & (0x80 >> (i % 8))) 
			printf("1");
		else
			printf("0");

		if (((i + 1) % 8) == 0) pos++;
	}

	printf("\n");
}

char *cp_bit_sequence_to_string(cp_bit_sequence *seq)
{
	int i, pos;
	char *res = (char *) malloc(seq->length + 1);
	if (res == NULL) return NULL;
	
	for (i = 0, pos = 0; i < seq->length; i++)
	{
		if (seq->bits[pos] & (0x80 >> (i % 8))) 
			res[i] = '1';
		else
			res[i] = '0';

		if (((i + 1) % 8) == 0) pos++;
	}

	res[i] = '\0';

	return res;
}

cp_bit_sequence *cp_bit_sequence_dup(cp_bit_sequence *seq)
{
	int bytecount;
	cp_bit_sequence *dup;
   
	dup	= (cp_bit_sequence *) malloc(sizeof(cp_bit_sequence));
	if (dup == NULL) return NULL;

	bytecount = BYTECOUNT(seq);
	dup->bits = (unsigned char *) malloc(bytecount);
	if (dup->bits == NULL)
	{
		free(dup);
		return NULL;
	}

	memcpy(dup->bits, seq->bits, bytecount);
	dup->length = seq->length;

	return dup;
}

cp_bit_sequence *cp_bit_sequence_cpy(cp_bit_sequence *dst, cp_bit_sequence *src)
{
	int bytecount_src, bytecount_dst;

	bytecount_src = BYTECOUNT(src);
	bytecount_dst = BYTECOUNT(dst);

	if (bytecount_src > bytecount_dst)
	{
		if (dst->length > 0)
			free(dst->bits);

		dst->bits = (unsigned char *) malloc(bytecount_src);
		if (dst->bits == NULL) return NULL;
	}

	memcpy(dst->bits, src->bits, bytecount_dst);
	src->length = dst->length;

	return dst;
}

static unsigned char bstr_mask[] = 
{ 
	0x80, // 10000000
	0xC0, // 11000000 
	0xE0, // 11100000 
	0xF0, // 11110000 
	0xF8, // 11111000 
	0xFC, // 11111100 
	0xFE, // 11111110 
	0xFF  // 11111111
};

cp_bit_sequence *cp_bit_sequence_cat(cp_bit_sequence *head, cp_bit_sequence *tail)
{
	int old_bytecount = BYTECOUNT(head);
	int pos = head->length;
	int cpos = old_bytecount << 3;
	int diff = cpos - pos;
	int newlen = head->length + tail->length;
	int new_bytecount = (newlen + 7) >> 3;

	int in_place = head == tail;
	if (in_place)
		tail = cp_bit_sequence_dup(tail);

	if (new_bytecount > old_bytecount)
	{
		unsigned char *p = (unsigned char *) malloc(new_bytecount);
		if (p == NULL) return NULL;

		memcpy(p, head->bits, old_bytecount);
		free(head->bits);
		head->bits = p;
	}

	if (diff == 0) // we're on a character boundary
	{
		memcpy(&head->bits[old_bytecount], tail->bits, BYTECOUNT(tail));
	}
	else
	{
		int i;

		/* example:
		 *   head      tail               
		 *   101      10100001 
		 *      ^---- pos: head->length = 3  diff = 5
		 */
		int stretch = new_bytecount - old_bytecount;
		head->bits[old_bytecount - 1] &= bstr_mask[(head->length % 8) - 1];
		for (i = 0; i < stretch; i++)
		{
			head->bits[old_bytecount - 1 + i] |= tail->bits[i] >> (8 - diff);
			head->bits[old_bytecount + i] = tail->bits[i] << diff;
		}
		if (stretch < BYTECOUNT(tail))
			head->bits[old_bytecount + stretch - 1] |= tail->bits[stretch] >> (8 - diff);
	}

	head->length = newlen;

	if (in_place)
		cp_bit_sequence_destroy(tail);

	return head;
}

//~~ optimize later: we want hardware CLZ instructions where available - gcc has builtins
#ifndef CP_CLZ_CHAR
#define CP_CLZ_CHAR(x) cp_clz_char_slow(x)
#endif

int cp_clz_char_slow(unsigned char x)
{
	int bit = 0x80;
	int lz = 0;
	for (lz = 0, bit = 0x80; bit > 0 && ((bit & x) == 0); lz++, bit >>= 1);
	return lz;
}

//~~ optimize later: better to compare sizeof(long long) bytes at a time
int cp_bit_sequence_cmp(cp_bit_sequence *a, cp_bit_sequence *b, int *rpos)
{
	int cmp, pos;
	int ca, cb, len;
	int atail, btail;
	int acmp, bcmp, shift;

	ca = BYTECOUNT(a);
	atail = ca * 8 - a->length;
	if (atail > 0) ca--;

	cb = BYTECOUNT(b);
	btail = cb * 8 - b->length;
	if (btail > 0) cb--;

	len = ca < cb ? ca : cb;

	for (pos = 0; pos < len; pos++)
		if (a->bits[pos] != b->bits[pos]) 
			break;

	if (pos < len)
	{
		if (rpos != NULL)
			*rpos = pos * 8 + CP_CLZ_CHAR(a->bits[pos] ^ b->bits[pos]);

		return a->bits[pos] - b->bits[pos];
	}

	acmp = bcmp = -1;
	shift = 0;

	if (len < ca) 
		acmp = a->bits[len];
	else if (atail > 0)
	{
		acmp = a->bits[len] & bstr_mask[7 - atail];
		shift = atail;
	}

	if (len < cb)
		bcmp = b->bits[len];
	else if (btail > 0)
	{
		bcmp = b->bits[len] & bstr_mask[7 - btail];
#define BMAX(a, b) ((a) > (b) ? (a) : (b))
		shift = BMAX(shift, btail);
	}

	if (acmp != -1 && bcmp != -1)
	{
		if (shift)
		{
			acmp >>= shift;
			bcmp >>= shift;
		}

		if (acmp == bcmp)
		{
			len = a->length - b->length;
			if (rpos != NULL)
				*rpos = len < 0 ? a->length : b->length;
			return len;
		}
		else
		{
			if (rpos != NULL)
				*rpos = (pos * 8 + CP_CLZ_CHAR(acmp ^ bcmp)) - shift;
			return (acmp - bcmp) << shift;
		}
	}

	if (rpos != NULL)
		*rpos = pos * 8; // + CP_CLZ_CHAR(a->bits[pos] ^ b->bits[pos]);

	return 0; // a->bits[pos] - b->bits[pos];
}

#if 0 //~~ 
	if (ca > sizeof(long) && cb > sizeof(long))
	{
		unsigned long *lpa = (unsigned long *) a->bits;
		unsigned long *lpb = (unsigned long *) b->bits;

		while (1)
		{
			cmp = *lpa - *lpb;
			if (cmp != 0) break;
			ca -= sizeof(long);
			cb -= sizeof(long);
			if (ca < sizeof(long) || cb < sizeof(long)) break;
			pos += sizeof(long) * 8;
		} 

		if (cmp != 0)
		{
			unsigned long *t = *lpa ^ *lpb;
			
		}
			
	}
#endif

/*
 * shift bits left by the specified count, resulting in a shorter bit sequence
 */
int cp_bit_sequence_shift_left(cp_bit_sequence *seq, int count)
{
	int i;
	int bytes = count / 8;
	int bits = count - bytes * 8;
	int len = BYTECOUNT(seq) - bytes;

	if (len > 0)
		memmove(seq->bits, &seq->bits[bytes], len);

	if (bits > 0)
	{
		seq->bits[0] <<= bits;
		for (i = 1; i < len; i++)
		{
			seq->bits[i - 1] |= seq->bits[i] >> (8 - bits);
			seq->bits[i] <<= bits;
		}
	}

	seq->length -= count;

	return seq->length;
}

