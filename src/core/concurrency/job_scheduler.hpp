#include <cstdint>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include <vector>
#include <thread>
#include <future>

typedef void Action(uintptr_t param);

typedef enum JobPriority {
    low = 0, medium = 1, high = 2
} JobPriority;

typedef struct Job {
    Action* action;
    uintptr_t param;
    JobPriority priority;
    std::atomic<int> counter;

    struct GreaterComparator {
        bool operator() (const Job* lhs, const Job* rhs);
    };
} Job;

class JobScheduler {
    public:
        JobScheduler();
        ~JobScheduler();
        void kickJob(Job* job);
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
