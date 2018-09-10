#include "logger.h"

int main()
{
	LOG_INFO << "info";
	LOG_DEBUG << "debug";
	LOG_ERROR << "error";
	LOG_INFO << 122;
}