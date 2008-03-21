#ifndef __MEMORY_SAMPLER_H_
#define __MEMORY_SAMPLER_H_


//-----------------------------------------------------------------------------
#include <pthread.h>

namespace statistics {

#include "memory_statistics.h"

class MemorySampler {
private:
    statistics::MemoryStatistics memstats_;

    pthread_t sampler_thread_;
    pthread_mutex_t lock_;
    pthread_cond_t cond_;
    bool done_;

    unsigned int intervalSecs_;

    static void *mem_sampler_thread (void *arg);

public:
    MemorySampler (unsigned int intervalSecs = 1);
    virtual ~MemorySampler ();

    void run ();
    void stop ();

    statistics::MemoryStatistics getMemStats ();
};


}; /* namespace statistics */

#endif /* __MEMORY_SAMPLER_H_ */