#include <cstdint>
#include <atomic>

typedef void Action(uintptr_t param);

typedef enum JobPriority {
    low, medium, high
} JobPriority;

typedef struct Job {
    Action* action;
    uintptr_t param;
    JobPriority priority;
    std::atomic<int> counter;
} Job;

class JobScheduler {
    public:
        void kickJob(Job* job);
        JobScheduler();
    private:

};
