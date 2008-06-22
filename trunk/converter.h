/**
* File : converter.h
* Description : Manage conversion between types
* Author(s) : ALucchi
* Date of creation : 12/08/2005
* Modification(s) :
*/

#ifndef CONVERTER_H
#define CONVERTER_H

//-------------------------------------------------------------------- INCLUDES

//------------------------------------------------------------------- FUNCTIONS

double parseReal(const char* val);

int parseInt(const char* val);

unsigned short getshort(unsigned char *source);

unsigned long getlong(unsigned char *source);

#endif // CONVERTER_H
