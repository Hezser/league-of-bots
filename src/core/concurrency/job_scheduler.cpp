#include <mutex>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <future>
#include <boost/fiber/fiber.hpp>
#include "job_scheduler.hpp"

/* class Job */

Job::Job(Action* action, uintptr_t param, JobPriority priority): action{action}, param{param},
        priority{priority} {
    status = new JobStatus;
    status->counter.store(1);  // To be overriden by a job batch, if the job is a part of it
}

// TODO: what if we destruct the job scheduler before a job completes?
void Job::join() {
    std::unique_lock<std::mutex> lock(status->mutex);
    if (status->counter.load() != 0) {
        status->cv.wait(lock, [this]{ return status->counter.load() == 0; });
    }
    lock.unlock();
}

Action* Job::getAction() {
    return action;
}

uintptr_t Job::getParam() {
    return param;
}

JobPriority Job::getPriority() {
    return priority;
}

bool Job::GreaterComparator::operator() (const Job* lhs, const Job* rhs) {   
    return lhs->priority > rhs->priority;                   
}

/* class JobBatch */

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

/* class JobScheduler */

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
