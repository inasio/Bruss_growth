#include "BM.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#define GLOBAL extern
#include "globals.h"



float ran(void)
BEGIN
   float dum;
   static int iff=0;
   int j,idumcopy;
   static float z,maxran,v[98];

   idumcopy = random_seed;

   IF (iff==0) THEN
      iff=1;
      maxran=RAND_MAX+1.0;
      srand(idumcopy);
      idumcopy=1;
      FOR (j=1;j<=97;j++) DO
           dum=rand();
      OD
      FOR (j=1;j<=97;j++) DO
           v[j]=rand();
      OD
      z=rand();
   FI
   j=1+(97*z/maxran);
   IF (j > 97 || j < 1) THEN
	 fprintf(stderr, "\nSRD_Growth[%d]: rand() has an error !!!\n",pid);
   FI
   z=v[j];
   v[j]=rand();
   return z/maxran;
END


