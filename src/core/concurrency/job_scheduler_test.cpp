#include <iostream>
#include "job_scheduler.hpp"

void action(uintptr_t param) {
    std::cout << param << std::endl;
}

int main() {
    JobScheduler* js = new JobScheduler();
    
    std::cout << "N. of CPUs: " << js->getNCpus() << std::endl;
    std::cout << "N. of threads: " << js->getNThreads() << std::endl;
    
    for (uintptr_t i=0; i<3; i++) {
        Job* job = new Job{action, i, (JobPriority)i};
        js->kickJob(job);
    }

    delete js;

    return 0;
}
