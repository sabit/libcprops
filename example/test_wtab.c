#include <stdlib.h>
#include <stdio.h>

#include "cprops/wtab.h"

int main(int argc, char *argv[])
{
	void *res;
	wchar_t key = 0;
	int v;
	int v1, v2, v3;

	wtab *tbl = wtab_new(10);
	if (tbl == NULL)
	{
		fprintf(stderr, "can\'t allocate wtab, sorry\n");
		exit(1);
	}

	v1 = 1; v2 = 2; v3 = 3;

	((char *) &key)[0] = 30; ((char *) &key)[1] = 40;
	wtab_put(tbl, key, &v1, NULL);
	res = wtab_get(tbl, key);
	if (res == NULL)
	{
		fprintf(stderr, "retrieval failed, ciao [1]\n");
		exit(2);
	}
	v = *(int *) ((wtab_node *) res)->value;
	printf("retrieved value %d, expected %d - %s\n", v, v1, v == v1 ? "success" : "error");

	((char *) &key)[0] = 50; ((char *) &key)[1] = 60;
	wtab_put(tbl, key, &v2, NULL);
	res = wtab_get(tbl, key);
	if (res == NULL)
	{
		fprintf(stderr, "retrieval failed, ciao [2]\n");
		exit(3);
	}
	v = *(int *) ((wtab_node *) res)->value;
	printf("retrieved value %d, expected %d - %s\n", v, v2, v == v2 ? "success" : "error");

	((char *) &key)[0] = 70; ((char *) &key)[1] = 80;
	wtab_put(tbl, key, &v3, NULL);
	res = wtab_get(tbl, key);
	if (res == NULL)
	{
		fprintf(stderr, "retrieval failed, ciao [3]\n");
		exit(4);
	}
	v = *(int *) ((wtab_node *) res)->value;
	printf("retrieved value %d, expected %d - %s\n", v, v3, v == v3 ? "success" : "error");

	printf("\ntesting all values:\n\n");

	((char *) &key)[0] = 30; ((char *) &key)[1] = 40;
	res = wtab_get(tbl, key);
	if (res == NULL)
	{
		fprintf(stderr, "retrieval failed, ciao [1]\n");
		exit(2);
	}
	v = *(int *) ((wtab_node *) res)->value;
	printf("retrieved value %d, expected %d - %s\n", v, v1, v == v1 ? "success" : "error");

	((char *) &key)[0] = 50; ((char *) &key)[1] = 60;
	res = wtab_get(tbl, key);
	if (res == NULL)
	{
		fprintf(stderr, "retrieval failed, ciao [2]\n");
		exit(3);
	}
	v = *(int *) ((wtab_node *) res)->value;
	printf("retrieved value %d, expected %d - %s\n", v, v2, v == v2 ? "success" : "error");

	((char *) &key)[0] = 70; ((char *) &key)[1] = 80;
	res = wtab_get(tbl, key);
	if (res == NULL)
	{
		fprintf(stderr, "retrieval failed, ciao [3]\n");
		exit(4);
	}
	v = *(int *) ((wtab_node *) res)->value;
	printf("retrieved value %d, expected %d - %s\n", v, v3, v == v3 ? "success" : "error");

	return 0;
}
	
