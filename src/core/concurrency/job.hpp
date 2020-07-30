/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef JOB_HPP
#define JOB_HPP

#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <atomic>

typedef void Action(uintptr_t param);

typedef enum JobPriority {
    low = 0, medium = 1, high = 2
} JobPriority;

typedef struct JobStatus {
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<int> counter;
} JobStatus;

class Job {
    public:
        JobStatus* status;  // It's public because of the atomic operations
        Job(Action* action, uintptr_t param, JobPriority priority);
        void join();
        Action* getAction();
        uintptr_t getParam();
        JobPriority getPriority();
        struct GreaterComparator {
            bool operator() (const Job* lhs, const Job* rhs);
        };

    private:
        Action* action;
        uintptr_t param;
        JobPriority priority;
};

#endif
