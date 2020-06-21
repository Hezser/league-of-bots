#include <mutex>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <future>
#include "job_scheduler.hpp"

/* struct Job */

bool Job::GreaterComparator::operator() (const Job* lhs, const Job* rhs) {   
    return lhs->priority > rhs->priority;                   
}

/* class JobScheduler */

void JobScheduler::work(std::shared_future<void> signal) {
    while(signal.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout) {
        // Wait for a job
        std::unique_lock<std::mutex> lock(m_mutex);
        m_job_available.wait(lock);
        Job* job = m_jobs.top();
        m_jobs.pop();
        lock.unlock();
        // Execute the job
        job->action(job->param);
    }
}

JobScheduler::JobScheduler(): m_n_cpus{std::thread::hardware_concurrency()},
        m_n_threads{m_n_cpus - 2} {
    std::shared_future<void> signal = m_exit_signal.get_future();
    for (auto i=0; i<m_n_threads; i++) {
        m_threads.emplace_back(&JobScheduler::work, this, signal);
    }
}

JobScheduler::~JobScheduler() {
    std::cout << "Exiting...\n";
    m_exit_signal.set_value();
    for (auto i=0; i<m_threads.size(); i++) {
        std::cout << " - Exiting thread...\n";
        m_threads[i].join();
    }
    std::cout << "Exited\n";
}

void JobScheduler::kickJob(Job* job) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_jobs.push(job);
    }
    m_job_available.notify_one();
}

unsigned int JobScheduler::getNCpus() {
    return m_n_cpus;
}

unsigned int JobScheduler::getNThreads() {
    return m_n_threads;
}
