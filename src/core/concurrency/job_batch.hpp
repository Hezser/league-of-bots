#ifndef JOB_BATCH_HPP
#define JOB_BATCH_HPP

#include "job.hpp"
#include <vector>

class JobBatch {
    public:
        JobStatus* status;  // It's public because of the atomic operations
        // Generic constructor
        JobBatch(std::vector<Job*> jobs);
        // SIMD constructor
        JobBatch(Action* action, std::vector<uintptr_t> params, JobPriority priority);
        void join();
        std::vector<Job*> getJobs();

    private:
        std::vector<Job*> m_jobs;
};

#endif
