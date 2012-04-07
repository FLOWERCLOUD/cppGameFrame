
#include <stdio.h>

#include "../md5.h"

int main()
{

	char str[] = "233839788花朵朵209B1492CF6AAE903F63FB7759D3565CD7";
	printf("md5: %s\n", MD5String(str));
	return 0;
}
