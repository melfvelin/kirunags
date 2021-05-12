/* This is the main file that calls all other files and functions
*
*/
#include "server.h"
#include "tmtc.h"
#include "tables.h"


int main()
{
	server::setup();
	//tmtc::encapsulate("hello", sizeof("hello"));
	return 0;
}