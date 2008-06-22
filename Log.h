/**
* File : Log.h
* Description : Class Log that must be used to document events all over the
* engine
* Author(s) : ALucchi
* Date of creation : 12/08/2005
* Modification(s) :
*/

#ifndef LOG_H
#define LOG_H

//-------------------------------------------------------------------- INCLUDES
#include "defs.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------- CONSTS

#define LOG_NAME	("log.txt")

//--------------------------------------------------------------------- CLASSES

class Log
{

private:

	FILE	*fp;			// pointer to log file
	char	*name;			// name of log file

	int		*refs;			// used to count the EXTRA references to the same log object

public:

	Log(void);
	Log(const char *filename);
	~Log(void);

	/**
	 * copy ctor
	 *
	 * this is here because of the funny tricks
	 * that can be performed with a reference to the same
	 * object recieved with the input operator (<<). 10x
	 * to the copy ctor this will be harmless:
	 *
	 * Log copyLog = sysLog << "no funny tricks please!";
	 * copyLog.close();
	 * sysLog.write("shit?! where is the file???"); 
	 */

	Log(Log& log);

	bool open(const char *filename);
	void close(void);
	bool reopen(void);

	void write(const char *str,...);

	Log& operator<<(const char *str);
	Log& operator<<(int num);
	Log& operator<<(uint num);
	Log& operator<<(long num);
	Log& operator<<(dword num);
	Log& operator<<(double num);
	Log& operator<<(Vector3 v);
};

//------------------------------------------------------------------------ VARS

extern Log sysLog;

#endif // LOG_H
