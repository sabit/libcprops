#include "bstr.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

cp_bstr *cp_bstr_create(int length, unsigned char *bits)
{
	int bytecount;
	cp_bstr *seq = (cp_bstr *) malloc(sizeof(cp_bstr));
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
cp_bstr *cstr_to_bstr(char *str)
{
	int i, pos, len, bytecount, bit;
	cp_bstr *seq;

	seq = (cp_bstr *) malloc(sizeof(cp_bstr));
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

void cp_bstr_destroy(cp_bstr *seq)
{
	if (seq)
	{
		free(seq->bits);
		free(seq);
	}
}

void cp_bstr_dump(cp_bstr *seq)
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

char *cp_bstr_to_string(cp_bstr *seq)
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

cp_bstr *cp_bstr_dup(cp_bstr *seq)
{
	int bytecount;
	cp_bstr *dup;
   
	dup	= (cp_bstr *) malloc(sizeof(cp_bstr));
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

cp_bstr *cp_bstr_cpy(cp_bstr *dst, cp_bstr *src)
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

cp_bstr *cp_bstr_cat(cp_bstr *head, cp_bstr *tail)
{
	int old_bytecount = BYTECOUNT(head);
	int pos = head->length;
	int cpos = old_bytecount << 3;
	int diff = cpos - pos;
	int newlen = head->length + tail->length;
	int new_bytecount = (newlen + 7) >> 3;

	int in_place = head == tail;
	if (in_place)
		tail = cp_bstr_dup(tail);

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
		cp_bstr_destroy(tail);

	return head;
}

int cp_bstr_cmp(cp_bstr *a, cp_bstr *b, int *rpos)
{
	int pos;
	int ca, cb, len;
	int atail, btail;
	int acmp, bcmp, shift;
	unsigned long long *llpa;
	unsigned long long *llpb;
	int llen;

	ca = BYTECOUNT(a);
	atail = ca * 8 - a->length;
	if (atail > 0) ca--;

	cb = BYTECOUNT(b);
	btail = cb * 8 - b->length;
	if (btail > 0) cb--;

	len = ca < cb ? ca : cb;

	pos = 0;

	if (len > sizeof(long long))
	{
		llpa = a->bits;
		llpb = b->bits;
		llen = len / 8;

		for ( ; pos < llen; pos++)
		{
			if (*llpa != *llpb)
				break;
			llpa++;
			llpb++;
		}
		if (pos != llen)
		{
			pos *= sizeof(long long);
			pos += CP_CLZ_LONG_LONG(*llpa ^ *llpb);
			goto FOUND;
		}
		pos *= sizeof(long long);
	}

	for ( ; pos < len; pos++)
		if (a->bits[pos] != b->bits[pos]) 
			break;
FOUND:
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
		*rpos = pos * 8;

	return 0;
}

/*
 * shift bits left by the specified count, resulting in a shorter bit sequence
 */
int cp_bstr_shift_left(cp_bstr *seq, int count)
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

