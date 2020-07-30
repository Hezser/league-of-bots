/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include "../core/concurrency/job_scheduler.hpp"

std::mutex mutex;

void action(uintptr_t param) {
    std::lock_guard<std::mutex> lock(mutex);
    std::cout << "Job " << param << std::endl;
}

int main() {
    JobScheduler* js = new JobScheduler();
    
    std::unique_lock<std::mutex> lock(mutex);
    std::cout << "\nN. of CPUs: " << js->getNCpus() << std::endl;
    std::cout << "N. of threads: " << js->getNThreads() << std::endl;
    std::cout << "\n --- JOB TEST ---\n\n";
    lock.unlock();
    
    // Should print 1 to 100 in order
    for (uintptr_t i=1; i<101; i++) {
        Job* job = new Job{action, i, low};
        js->kickJob(job);
        job->join();
    }

    lock.lock();
    std::cout << "\n --- JOB BATCH TEST ---\n\n";
    lock.unlock();

    // Should print 1 to 100 (not necessarily in order)
    std::vector<uintptr_t> params = {};
    for (int i=1; i<100000; i++) {
        params.push_back(i);
    }
    JobBatch* batch = new JobBatch(action, params, low);
    js->kickJobBatch(batch);
    batch->join();

    lock.lock();
    std::cout << "\nFinishing...\n\n";
    lock.unlock();

    delete js;

    return 0;
}
