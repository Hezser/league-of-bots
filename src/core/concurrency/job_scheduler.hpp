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

class JobBatch {
    public:
        JobStatus* status;  // It's public because of the atomic operations
        // Generic constructor
        JobBatch(std::vector<Job*> jobs);
        // SIMD constructor
        JobBatch(Action* action, std::vector<uintptr_t> params, JobPriority priority);
        void join();
        std::vector<Job*> getJobs();

    private:
        std::vector<Job*> m_jobs;
};

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
