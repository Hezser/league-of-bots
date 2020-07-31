/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "job.hpp"

using namespace adamant::concurrency;

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
