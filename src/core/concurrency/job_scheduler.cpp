#include <mutex>
#include <chrono>
#include <thread>
#include <vector>
#include <future>
#include <boost/fiber/fiber.hpp>
#include "job_scheduler.hpp"

void JobScheduler::work(std::shared_future<void> signal) {
    while(signal.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
        // Wait for a job
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_jobs.empty()) {
            m_job_available.wait(lock, [this, signal]{
                return (!m_jobs.empty()) ||
                signal.wait_for(std::chrono::seconds(0)) != std::future_status::timeout; 
            });
            if (signal.wait_for(std::chrono::seconds(0)) != std::future_status::timeout) {
                break;
            }
        }
        // Get a job
        Job* job = m_jobs.top();
        m_jobs.pop();
        lock.unlock();
        /* Execute the job on a fiber, so that a job can sleep or wait (yield) without
         * blocking a thread/core */
        boost::fibers::fiber fiber(job->getAction(), job->getParam());
        fiber.join();
        job->status->counter.fetch_sub(1);
        job->status->cv.notify_all();
    }
}

// TODO: Affinity with CPU cores
JobScheduler::JobScheduler(): m_n_cpus{std::thread::hardware_concurrency()},
        m_n_threads{m_n_cpus - 2} {
    std::shared_future<void> signal = m_exit_signal.get_future();
    for (auto i=0; i<m_n_threads; i++) {
        m_threads.emplace_back(&JobScheduler::work, this, signal);
    }
}

JobScheduler::~JobScheduler() {
    m_exit_signal.set_value();
    m_job_available.notify_all();
    for (auto i=0; i<m_threads.size(); i++) {
        m_threads[i].join();
    }
}

void JobScheduler::kickJob(Job* job) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_jobs.push(std::move(job));
    }
    m_job_available.notify_one();
}

void JobScheduler::kickJobBatch(JobBatch* job_batch) {
    for (auto job : job_batch->getJobs()) {
        kickJob(job);
    }
}

unsigned int JobScheduler::getNCpus() {
    return m_n_cpus;
}

unsigned int JobScheduler::getNThreads() {
    return m_n_threads;
}
