/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef JOB_SCHEDULER_HPP
#define JOB_SCHEDULER_HPP

#include "job.hpp"
#include "job_batch.hpp"
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <future>

namespace adamant {
namespace concurrency {

class JobScheduler {
    public:
        JobScheduler();
        ~JobScheduler();
        void kickJob(Job* job);
        void kickJobBatch(JobBatch* job_batch);
        unsigned int getNCpus();
        unsigned int getNThreads();

    private:
        const unsigned int m_n_cpus;
        const unsigned int m_n_threads;
        std::vector<std::thread> m_threads;
        std::priority_queue<Job*, std::vector<Job*>, Job::GreaterComparator> m_jobs;
        std::condition_variable m_job_available;
        std::mutex m_mutex;
        std::promise<void> m_exit_signal;
        void work(std::shared_future<void> signal);
};

}  // namespace concurrency
}  // namespace adamant

#endif
