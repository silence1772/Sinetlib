#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <chrono>

using std::chrono::system_clock;

using Nanosecond  = std::chrono::nanoseconds;
using Microsecond = std::chrono::microseconds;
using Millisecond = std::chrono::milliseconds;
using Second      = std::chrono::seconds;
using Minute      = std::chrono::minutes;
using Hour        = std::chrono::hours;

using Timestamp   = std::chrono::time_point<system_clock, Nanosecond>;

#endif // TIMESTAMP_H