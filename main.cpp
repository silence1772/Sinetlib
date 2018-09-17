#include "logger.h"
#include <unistd.h>

int main()
{
	for (int i = 0; i < 10000; i++)
	{
		LOG_INFO << "test";
	}
	sleep(4);
}