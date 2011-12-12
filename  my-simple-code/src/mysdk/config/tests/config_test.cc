/*
 * config_test.cc
 *
 *  Created on: 2011-10-24
 *    
 */

#include <mysdk/config/Config.h>

#include <string.h>

int main()
{
	Config conf;
	const char* content = "#file test\nkey1 = value1\nkey2 = value2\n worldname = 我你他";

	conf.parse(content, strlen(content));
	conf.printfInfo();
	return 0;
}
