/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "job_batch.hpp"

// Generic constructor
JobBatch::JobBatch(std::vector<Job*> jobs): m_jobs{jobs} {
    status = new JobStatus;
    status->counter.store(m_jobs.size());
    // Override each of the jobs' counter
    for (auto job : m_jobs) {     
        job->status = status;
    }
}

// SIMD constructor
JobBatch::JobBatch(Action* action, std::vector<uintptr_t> params, JobPriority priority) {
    status = new JobStatus;
    status->counter.store(params.size());
    for (auto i=0; i<params.size(); i++) {
        Job* job = new Job(action, params[i], priority);
        // Override each of the jobs' counter
        job->status = status;
        m_jobs.push_back(job);
    }
}

void JobBatch::join() {
    std::unique_lock<std::mutex> lock(status->mutex);
    if (status->counter.load() != 0) {
        status->cv.wait(lock, [this]{ return status->counter.load() == 0; });
    }
    lock.unlock();
}

std::vector<Job*> JobBatch::getJobs() {
    return m_jobs;
}
