#include "logger.h"
#include <unistd.h>

int main()
{
	LOG_INFO << "info";
	LOG_DEBUG << "debug";
	LOG_ERROR << "error";
	LOG_INFO << 122;
	sleep(5);

}