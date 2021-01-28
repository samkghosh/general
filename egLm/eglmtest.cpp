/*********************************************
* fileName: eglmtest.cpp
*********************************************/
#include <stdio.h>

#include "../inc/egLm.h"

/********************************************/

int main(int argc, char * argv[])
{
 
   CEgLm lm("Arcane");
   
   int iRc;

   iRc = lm.check();
printf("RetVal=%d\n", iRc);
   switch(iRc)
   {

	case EGLM_NOLOCATION:
		printf("Location not found\n");
	   break;

	case EGLM_NOACCESS: //			0x02
		printf("No Access right\n");
           break;

        case EGLM_NOENTRYFOUND:
		printf("Entry Not Found\n");
		break;
	case EGLM_OPENERROR:
		printf("Could not open License\n");
		break;
	case EGLM_TIMEEXPIRED:
		printf("Time Has expired\n");
		break;
	case EGLM_OK:
		printf("License found Ok\n");
		break;
	case EGLM_INVALID:
		printf("Invalid License\n");
		break;
	default:
		printf("Donot know\n");
   }
   return 0;
}
