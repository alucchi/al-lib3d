/**
* File : converter.cpp
* Description : Manage conversion between types
* Author(s) : ALucchi
* Date of creation : 12/08/2005
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include "converter.h"

#include <stdlib.h>

//------------------------------------------------------------------- FUNCTIONS

double parseReal(const char* val)
{
   return atof(val);
}

int parseInt(const char* val)
{
   return atoi(val);
}

/*  read 2 bytes from current position in file 
 *  and convert little endian to short
 */
unsigned short getshort(unsigned char *source)
{
   unsigned short number;

   number = source[0];
   number += source[1] * 256;

   return(number);
}

/*  read 4 bytes from current position in file 
 *  and convert little endian to long int 
 */
unsigned long getlong(unsigned char *source)
{
   unsigned long number;

   number = source[0];
   number += source[1] * 256;
   number += source[2] * 256 * 256;
   number += source[3] * 256 * 256 * 256;

   return(number);
}
