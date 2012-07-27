#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cprops/rtrie.h"
#include "cprops/vector.h"

int main(int argc, char *argv[])
{
	char *bstr1 = "0011100011110000011111";
	char *bstr2 = "0011100011110010011111";
	char *bstr3 = "0011100011110000011111001";
	char *bstr4 = "01";
	char *bstr5 = "011001";

	char *l1 = "001110001111000001111";
	char *l2 = "0011100011110000011110";
	char *l3 = "00111000111100000111111";

	void *x;
	cp_vector *v;
	int len;
	char *tmp;

	cp_bstr *a, *b, *c, *d, *e;
	cp_bstr *bl1, *bl2, *bl3;

	cp_rtrie *grp;

	a = cstr_to_bstr(bstr1);
	b = cstr_to_bstr(bstr2);
	c = cstr_to_bstr(bstr3);
	d = cstr_to_bstr(bstr4);
	e = cstr_to_bstr(bstr5);

	grp = cp_rtrie_create_rtrie(COLLECTION_MODE_NOSYNC | 
							    COLLECTION_MODE_COPY | COLLECTION_MODE_DEEP,
							    (cp_copy_fn) strdup, free);

	printf("empty trie\n");
	cp_rtrie_dump(grp);

	cp_rtrie_add(grp, a, "aaa");
	printf("adding aaa\n");
	cp_rtrie_dump(grp);

	cp_rtrie_add(grp, b, "bbb");
	printf("adding bbb\n");
	cp_rtrie_dump(grp);

	cp_rtrie_add(grp, c, "ccc");
	printf("adding ccc\n");
	cp_rtrie_dump(grp);

	cp_rtrie_add(grp, d, "ddd");
	printf("adding ddd\n");
	cp_rtrie_dump(grp);

	cp_rtrie_add(grp, e, "eee");
	printf("adding eee\n");
	cp_rtrie_dump(grp);

	bl1 = cstr_to_bstr(l1);
	printf("looking up [%s]\n", l1);
	x = cp_rtrie_exact_match(grp, bl1);
	printf("expected: 0, actual: %p [%s] - %s\n", x, x != 0 ? (char *) x : "NULL", x == NULL ? "success" : "error");
	cp_bstr_destroy(bl1);
	
	bl2 = cstr_to_bstr(l2);
	printf("looking up [%s]\n", l2);
	x = cp_rtrie_exact_match(grp, bl2);
	printf("expected: 0, actual: %p [%s] - %s\n", x, x != 0 ? (char *) x : "NULL", x == NULL ? "success" : "error");
	cp_bstr_destroy(bl2);

	bl3 = cstr_to_bstr(l3);
	printf("looking up [%s]\n", l3);
	x = cp_rtrie_exact_match(grp, bl3);
	printf("expected: 0, actual: %p [%s] - %s\n", x, x != 0 ? (char *) x : "NULL", x == NULL ? "success" : "error");
	cp_bstr_destroy(bl3);

	printf("looking up [%s]\n", bstr1);
	x = cp_rtrie_exact_match(grp, a);
	printf("expected: [aaa], actual: %p [%s] - %s\n", x, x != 0 ? (char *) x : "NULL", ((x != NULL) && (strcmp((char *) x, "aaa") == 0)) ? "success" : "error");
	
	printf("testing cp_rtrie_fetch_matches\n");
	printf("looking up [%s]\n", bstr3);
	v = cp_rtrie_fetch_matches(grp, c);
	if (v != NULL)
	{
		int i;
		char *s;
		i = cp_vector_size(v);
		if (i != 5)
			printf("expected 5 entries, got %d - error\n", i);
		else
		{
			s = cp_vector_element_at(v, 0);
			if (s != NULL)
				printf("expected NULL element 0, got %p [%s] - error\n", s, s);
			else 
				printf("[0] [NULL]\n");

			s = cp_vector_element_at(v, 1);
			if (s != NULL)
				printf("expected NULL element 1, got %p [%s] - error\n", s, s);
			else 
				printf("[1] [NULL]\n");

			s = cp_vector_element_at(v, 2);
			if (s != NULL)
				printf("expected NULL element 2, got %p [%s] - error\n", s, s);
			else 
				printf("[2] [NULL]\n");

			s = cp_vector_element_at(v, 3);
			if (strcmp(s, "aaa") != 0)
				printf("expected element 3 [aaa], got %p [%s] - error\n", s, s);
			else 
				printf("[3] [aaa]\n");

			s = cp_vector_element_at(v, 4);
			if (strcmp(s, "ccc") != 0)
				printf("expected element 4 [ccc], got %p [%s] - error\n", s, s);
			else 
				printf("[4] [ccc]\n");
		}
	}
	else
		printf("lookup failed, error\n");
	cp_vector_destroy(v);
				
	tmp = strdup(bstr3);
	len = strlen(tmp);
	tmp[len - 1] = '\0';
	c->length--;
	printf("looking up [%s]\n", tmp);
	v = cp_rtrie_fetch_matches(grp, c);
	if (v != NULL)
	{
		int i;
		char *s;
		i = cp_vector_size(v);
		if (i != 4)
			printf("expected 4 entries, got %d - error\n", i);
		else
		{
			s = cp_vector_element_at(v, 0);
			if (s != NULL)
				printf("expected NULL element 0, got %p [%s] - error\n", s, s);
			else 
				printf("[0] [NULL]\n");

			s = cp_vector_element_at(v, 1);
			if (s != NULL)
				printf("expected NULL element 1, got %p [%s] - error\n", s, s);
			else 
				printf("[1] [NULL]\n");

			s = cp_vector_element_at(v, 2);
			if (s != NULL)
				printf("expected NULL element 2, got %p [%s] - error\n", s, s);
			else 
				printf("[2] [NULL]\n");

			s = cp_vector_element_at(v, 3);
			if (strcmp(s, "aaa") != 0)
				printf("expected element 3 [aaa], got %p [%s] - error\n", s, s);
			else 
				printf("[3] [aaa]\n");
		}
	}
	else
		printf("lookup failed, error\n");
	c->length++;
	cp_vector_destroy(v);

	printf("\ntesting cp_rtrie_submatch\n");
	v = cp_rtrie_submatch(grp, a);
	if (v == NULL)
		printf("got no result for [%s], error\n", bstr1);
	else if (cp_vector_size(v) != 2)
		printf("expected 2 entries for [%s], got %d, error\n", bstr1, cp_vector_size(v));
	else
	{
		char *s = cp_vector_element_at(v, 0);
		if (strcmp(s, "aaa") != 0)
			printf("expected element 0 [aaa], got %p [%s] - error\n", s, s);
		else 
			printf("[0] [aaa]\n");

		s = cp_vector_element_at(v, 1);
		if (strcmp(s, "ccc") != 0)
			printf("expected element 1 [ccc], got %p [%s] - error\n", s, s);
		else 
			printf("[1] [ccc]\n");
	}
	
	printf("\ntesting_cp_rtrie_prefix_match\n");
	
	len = cp_rtrie_prefix_match(grp, c, &x);
	if (len != 2)
		printf("expected 2 nodes, got %d - error\n", len);
	if (strcmp((char *) x, "ccc") != 0)
		printf("expected [ccc], got [%s], error\n", (char *) x);

	c->length--;
	len = cp_rtrie_prefix_match(grp, c, &x);
	if (len != 1)
		printf("expected 1 node, got %d - error\n", len);
	if (strcmp((char *) x, "aaa") != 0)
		printf("expected [aaa], got [%s], error\n", (char *) x);
	c->length++;

	printf("\nremoving aaa\n");
	cp_rtrie_remove(grp, a, NULL);
	cp_rtrie_dump(grp);

	printf("removing bbb\n");
	cp_rtrie_remove(grp, b, NULL);
	cp_rtrie_dump(grp);

	printf("removing ccc\n");
	cp_rtrie_remove(grp, c, NULL);
	cp_rtrie_dump(grp);

	printf("removing ddd\n");
	cp_rtrie_remove(grp, d, NULL);
	cp_rtrie_dump(grp);

	printf("removing eee\n");
	cp_rtrie_remove(grp, e, NULL);
	cp_rtrie_dump(grp);

	cp_bstr_destroy(a);
	cp_bstr_destroy(b);
	cp_bstr_destroy(c);
	cp_bstr_destroy(d);
	cp_bstr_destroy(e);
	
	cp_rtrie_destroy(grp);

	return 0;
}

