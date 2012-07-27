#include <stdio.h>
#include <stdlib.h>
#include <cprops/wtrie.h>
#include <wchar.h>

int main()
{
	void *prm;
	char *v;
	cp_wtrie *t = cp_wtrie_create(0);
	wchar_t FERGUSON[] = { 'F', 'E', 'R', 'G', 'U', 'S', 'O', 'N', 0 };
	wchar_t FERRY[] = { 'F', 'E', 'R', 'R', 'Y', 0 };
	wchar_t FERGUSONST[] = { 'F', 'E', 'R', 'G', 'U', 'S', 'O', 'N', 'S', 'T', 0 };;
	wchar_t FERMAT[] = { 'F', 'E', 'R', 'M', 'A', 'T', 0 };

	
	if (t == NULL)
	{
		printf("can\'t create wtrie\n");
		exit(1);
	}
	
	cp_wtrie_add(t, FERGUSON, "xxx");
	cp_wtrie_add(t, FERMAT, "yyy");

	cp_wtrie_prefix_match(t, FERRY, &prm);
	v = prm;
	printf("prefix match for ferry: %s\n", v ? v : "none");

	v = cp_wtrie_exact_match(t, FERRY);
	printf("exact match for ferry: %s\n", v ? v : "none");

	cp_wtrie_prefix_match(t, FERGUSONST, &prm);
	v = prm;
	printf("prefix match for fergusonst: %s\n", v ? v : "none");

	v = cp_wtrie_exact_match(t, FERGUSONST);
	printf("exact match for fergusonst: %s\n", v ? v : "none");

	cp_wtrie_destroy(t);

	return 0;
}
