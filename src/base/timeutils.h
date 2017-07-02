#ifndef FRUITE_BASE_TIMEUTILS_H_
#define FRUITE_BASE_TIMEUTILS_H_

#include <time.h>

#include "base/basictypes.h"

namespace fruite {

static const int64_t kNumMillisecPerSec = 1000;
static const int64_t kNumMillisecsPerSec = INT64_C(1000);      
static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);   
static const int64_t kNumNanosecsPerSec = INT64_C(1000000000); 
                                                               
static const int64_t kNumMicrosecsPerMillisec =                
        kNumMicrosecsPerSec / kNumMillisecsPerSec;                 
static const int64_t kNumNanosecsPerMillisec =                 
        kNumNanosecsPerSec / kNumMillisecsPerSec;                  
static const int64_t kNumNanosecsPerMicrosec =                 
        kNumNanosecsPerSec / kNumMicrosecsPerSec;                  

uint64_t SystemTimeNanos();
} // namespace fruite 

#endif // XXK_BASE_TIMEUTILS_H_
