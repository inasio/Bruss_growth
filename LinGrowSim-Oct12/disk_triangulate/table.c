#include <stdio.h>

int f(int border_order, int center_order)
{
	return (
 center_order*
                        (border_order*(border_order+1)/2 - border_order) + 1);
}
main (int argc, char **argv)
{
	int min_bo, max_bo, min_co, max_co;
	int co, bo;

	if (argc<5) {
		fprintf(stderr, "Usage: %s min_b.o. max_b.o. min_c.o. max_c.o.\n", argv[0]);
		return;
	}

	min_bo = atoi(argv[1]);
	max_bo = atoi(argv[2]);
	min_co = atoi(argv[3]);
	max_co = atoi(argv[4]);

	printf("Number of nodes in disk-triangulation. \n  V Border-order\tCenter-order ->\n");
	printf("  // ");
	for (co=min_co; co<=max_co; co++)
		printf("\t%4d ", co);
	printf("\n");
	for (bo = min_bo; bo <= max_bo; bo ++ ) {
		printf("%3d ", bo);
		for (co = min_co; co <= max_co; co++)
			printf("\t%4d", f(bo,co));
		printf("\n");
	}
}
	
