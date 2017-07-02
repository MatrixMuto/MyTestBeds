
#include "base/timeutils.h"
#include "base/basictypes.h"

namespace fruite {

    uint64_t SystemTimeNanos() {
        int64_t ticks;
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        ticks = kNumNanosecsPerSec * static_cast<int64_t>(ts.tv_sec) +
                static_cast<int64_t>(ts.tv_nsec);
        return ticks;
    }

} // namespace fruite
