#include "util/Time.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::int64_t nowMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

std::string formatTimestamp(std::int64_t ts_ms) {
    using namespace std::chrono;
    auto dur = milliseconds(ts_ms);
    auto tp = time_point<system_clock>(duration_cast<system_clock::duration>(dur));
    std::time_t tt = system_clock::to_time_t(tp);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%F %T");
    return oss.str();
}


