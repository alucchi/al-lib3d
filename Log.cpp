/**
* File : Log.cpp
* Description : Class Log that must be used to document events all over the
* engine
* Author(s) : ALucchi
* Date of creation : 12/08/2005
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES

#include "defs.h"
#include "Log.h"

//------------------------------------------------------------------------ VARS

Log sysLog(LOG_NAME);

//--------------------------------------------------------------------- CLASSES

Log::Log(void)
{
	fp		= NULL;
	name	= NULL;

	// create the reference counter
	refs	= new int;
	*refs	= 0;
}

Log::Log(const char *filename)
{
	fp		= NULL;
	name	= NULL;

	// create the reference counter
	refs	= new int;
	*refs	= 0;

	// open the file
	open(filename);
}

Log::~Log(void)
{
	// take appropriate actions depending on how many (extra) references there are
	if(!*refs) {
		close();
		if(name) {
			delete [] name;
			name = NULL;
		}	

		// free the reference counter
		delete refs;
	}
	else {
		*refs -= 1;
	}
	
	refs = NULL;
}

Log::Log(Log& log)
{
	// get pointer to the reference counter of the object, which is beeing cloned
	refs = log.refs;

	// increment the reference counter
	*refs += 1;

	// copy the other fields as usual (not that some other ref will have access to them :))
	fp		= log.fp;
	name	= log.name;
}

bool Log::open(const char *filename)
{
	assert(filename != NULL);

	// ehm... file is/was already opended d00d
	if(fp || name) {
		return false;	
	}

	// cannot open a log file, while there are references, since
	// modifing one, will screw all the others
	if(*refs) {
		return false;
	}

	// but if everything is ok, proceed with creating the file
	fp = fopen(filename, "wt");
	if(!fp) {
		return false;
	}

	// write some stuff to the new file

	fprintf(fp, "// log file created on "__DATE__" "__TIME__"\n//\n\n");
	
	// copy the name of the log (for re-opening)
	name = new char[strlen(filename) + 1];
	strcpy(name, filename);

	// all done!
	return true;
}

void Log::close(void)
{
	if(!fp) {
		return;
	}

	// cannot close if there are more references, since changing one
	// will 'cause the others to malfunction
	if(*refs) {
		return;
	}

	// else, shut it down
	fclose(fp);
	fp = NULL;
}

bool Log::reopen(void)
{
	if(fp) {
		return true;
	}

	if(!name) {
		return false;
	}

	// cannot reopen if there are more references, see the funcs above to know why!
	if(*refs) {
		return false;
	}

	// else, try to reload it
	fp = fopen(name, "a+");
	if(!fp) {
		return true;
	}

	// all done!
	return true;
}

void Log::write(const char *str,...)
{
	va_list argList;

	assert(str != NULL);
	
	if(!fp)
		return;

	va_start(argList, str);
	vfprintf(fp, str, argList);
	va_end(argList);

	fflush(fp);
}

Log& Log::operator<<(const char *str)
{
	write(str);
	return *this;
}

Log& Log::operator<<(int num)
{
	write("%d", num);
	return *this;
}

Log& Log::operator<<(uint num)
{
	write("%u", num);
	return *this;
}

Log& Log::operator<<(long num)
{
	write("%d", num);
	return *this;
}

Log& Log::operator<<(dword num)
{
	write("%u", num);
	return *this;
}

Log& Log::operator<<(double num)
{
	write("%f", num);
	return *this;
}

Log& Log::operator<<(Vector3 v)
{
	write("%f %f %f", v.x,v.y,v.z);
	return *this;
}