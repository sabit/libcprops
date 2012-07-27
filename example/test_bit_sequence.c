#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cprops/bit_sequence.h"

void test_short_strings()
{
	char *cstr;
	cp_bit_sequence *a = cstr_to_bit_sequence("100");
	cp_bit_sequence *c = cstr_to_bit_sequence("100");
	cp_bit_sequence *b = cstr_to_bit_sequence("101");

	cstr = cp_bit_sequence_to_string(a);
	if (strcmp(cstr, "100") != 0)
		printf("cstr_to_bit_sequence(\"100\"): expected \"100\", received \"%s\" - error\n", cstr);
	else 
		cp_bit_sequence_dump(a);
	free(cstr);

	cstr = cp_bit_sequence_to_string(b);
	if (strcmp(cstr, "101") != 0)
		printf("cstr_to_bit_sequence(\"101\"): expected \"101\", received \"%s\" - error\n", cstr);
	else 
		cp_bit_sequence_dump(b);
	free(cstr);

	cp_bit_sequence_cat(a, b);
	cstr = cp_bit_sequence_to_string(a);
	if (strcmp(cstr, "100101") != 0)
		printf("cstr_to_bit_sequence(\"100101\"): expected \"100101\", received \"%s\" - error\n", cstr);
	else 
		cp_bit_sequence_dump(a);
	free(cstr);

	cp_bit_sequence_cat(a, b);
	cstr = cp_bit_sequence_to_string(a);
	if (strcmp(cstr, "100101101") != 0)
		printf("cstr_to_bit_sequence(\"100101101\"): expected \"100101101\", received \"%s\" - error\n", cstr);
	else 
		cp_bit_sequence_dump(a);
	free(cstr);

	cp_bit_sequence_cat(a, c);
	cstr = cp_bit_sequence_to_string(a);
	if (strcmp(cstr, "100101101100") != 0)
		printf("cstr_to_bit_sequence(\"100101101100\"): expected \"100101101100\", received \"%s\" - error\n", cstr);
	else 
		cp_bit_sequence_dump(a);
	free(cstr);

	cp_bit_sequence_cat(a, b);
	cstr = cp_bit_sequence_to_string(a);
	if (strcmp(cstr, "100101101100101") != 0)
		printf("cstr_to_bit_sequence(\"100101101100101\"): expected \"100101101100101\", received \"%s\" - error\n", cstr);
	else 
		cp_bit_sequence_dump(a);
	free(cstr);

	cp_bit_sequence_destroy(a);
	cp_bit_sequence_destroy(b);
	cp_bit_sequence_destroy(c);
}

int main(int argc, char *argv[])
{
	unsigned long p = 0xAA0000AA;
	unsigned long q = 0x00550055;
	unsigned long r = 0x00570057;

	char buf[0x1000];
	char *pre1, *pre2, *pre3, *res;
	int pos, cpos, cmp, ccmp;

	cp_bit_sequence *a = cp_bit_sequence_create(7, (unsigned char *) &p);
	cp_bit_sequence *b = cp_bit_sequence_create(23, (unsigned char *) &q);
	cp_bit_sequence *c = cp_bit_sequence_create(23, (unsigned char *) &r);

	pre1 = cp_bit_sequence_to_string(a);
	pre2 = cp_bit_sequence_to_string(b);
	pre3 = cp_bit_sequence_to_string(c);
	
	printf("comparing sequences b [%s] and c [%s]\n", pre2, pre3);
	for (pos = 0; pos < c->length; pos++)
		if (pre2[pos] != pre3[pos]) break;
	cmp = pre2[pos] - pre3[pos];
	printf("expected: b %c c, difference at bit %d\n", cmp > 0 ? '>' : '<', pos + 1);
	
	ccmp = cp_bit_sequence_cmp(b, c, &cpos);
#define SIGN(p) ((p) > 0 ? 1 : -1)
	printf("actual: b %c c, difference at bit %d - %s\n", cmp > 0 ? '>' : '<', cpos + 1, 
		(cpos == pos && SIGN(cmp) == SIGN(ccmp) ? "success" : "error"));

	cp_bit_sequence_destroy(c);
	free(pre3);

	strcpy(buf, pre1);
	strcat(buf, pre2);
	cp_bit_sequence_cat(a, b);
	res = cp_bit_sequence_to_string(a);
	printf("testing [%s] + [%s]\n", pre1, pre2);
	printf("expected: [%s]\n", buf);
	printf("actual  : [%s] - %s\n", res, (strcmp(res, buf) == 0) ? "success" : "error");
	free(pre1); free(pre2); free(res);

	cp_bit_sequence_cat(b, a);

	pre1 = cp_bit_sequence_to_string(b);
	pre2 = cp_bit_sequence_to_string(a);
	strcpy(buf, pre1);
	strcat(buf, pre2);
	cp_bit_sequence_cat(b, a);
	res = cp_bit_sequence_to_string(b);
	printf("testing [%s] + [%s]\n", pre1, pre2);
	printf("expected: [%s]\n", buf);
	printf("actual  : [%s] - %s\n", res, (strcmp(res, buf) == 0) ? "success" : "error");

	free(pre1); free(pre2); 
	cp_bit_sequence_destroy(a);

	printf("\ntesting shift left\n");
	pre1 = res;
	while (1)
	{
		pos = (b->length / 2) - 1;
		if (pos <= 0) break;
		a = cp_bit_sequence_dup(b);
		cp_bit_sequence_shift_left(a, pos);
		pre2 = cp_bit_sequence_to_string(a);
		printf("testing [%s] << %d\n", pre1, pos);
		pre1 = &pre1[pos];
		printf("expected [%s]\n", pre1);
		printf("actual   [%s] - %s\n", pre2, (strcmp(pre1, pre2) == 0) ? "success" : "error");
		cp_bit_sequence_destroy(b);
		free(pre2);
		b = a;
	} 
		
	free(res);
	cp_bit_sequence_destroy(b);

	printf("\ntesting initialization from string\n");
	pre1 = "10101010101010101000111010101011101000011100101010101010101111000111000110001";
	a = cstr_to_bit_sequence(pre1);
	pre2 = cp_bit_sequence_to_string(a);
	printf("testing  [%s]\n", pre1);
	printf("expected [%s]\n", pre1);
	printf("actual   [%s] - %s\n", pre2, (strcmp(pre1, pre2) == 0) ? "success" : "error");
	free(pre2);

	pre2 = "101";
	b = cstr_to_bit_sequence(pre2);
	printf("testing compare: [%s], [%s]\n", pre1, pre2);
	cmp = cp_bit_sequence_cmp(a, b, &pos);
	printf("expected: a > b , pos 3\n");
	printf("actual: cmp %s 0, pos %d - %s\n", cmp > 0 ? ">" : cmp < 0 ? "<" : "==", pos, (cmp > 0 && pos == 3) ? "success" : "error");
	cp_bit_sequence_destroy(b);

	pre2 = "100";
	b = cstr_to_bit_sequence(pre2);
	printf("testing compare: [%s], [%s]\n", pre2, pre1);
	cmp = cp_bit_sequence_cmp(a, b, &pos);
	printf("expected: cmp > 0, pos 2\n");
	printf("actual: cmp %s 0, pos %d - %s\n", cmp > 0 ? ">" : cmp < 0 ? "<" : "==", pos, (cmp > 0 && pos == 2) ? "success" : "error");

	cp_bit_sequence_destroy(a);
	cp_bit_sequence_destroy(b);

	test_short_strings();

	return 0;
}

