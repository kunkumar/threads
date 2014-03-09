#include <stdio.h>
#include <queue>

#include <unistd.h>

#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>

// Base task for Tasks
// run() should be overloaded and expensive calculations done there
// showTask() is for debugging and can be deleted if not used
class Task {
public:
    Task() {}
    virtual ~Task() {}
    virtual void run()=0;
    virtual void showTask()=0;
};

// Wrapper around std::queue with some mutex protection
class WorkQueue {
public:
    WorkQueue() {
        // Initialize the mutex protecting the queue
        pthread_mutex_init(&qmtx,0);
        // wcond is a condition variable that's signaled
        // when new work arrives
        pthread_cond_init(&wcond, 0);
    }
    
    ~WorkQueue() {
        // Cleanup pthreads
        pthread_mutex_destroy(&qmtx);
        pthread_cond_destroy(&wcond);
    }
    // Retrieves the next task from the queue
    Task *nextTask() {
        // The return value
        Task *nt = 0;

        // Lock the queue mutex
        pthread_mutex_lock(&qmtx);
        // Check if there's work
        if (finished && tasks.size() == 0) {
            // If not return null (0)
            nt = 0;
        } else {
            // Not finished, but there are no tasks, so wait for
            // wcond to be signalled
            if (tasks.size()==0) {
printf("before wait\n");
                pthread_cond_wait(&wcond, &qmtx);
            }
            // get the next task
            nt = tasks.front();
            if(nt){
            tasks.pop();
		}

            // For debugging
            if (nt) nt->showTask();
        }
        // Unlock the mutex and return
        pthread_mutex_unlock(&qmtx);
        return nt;
    }
    // Add a task
    void addTask(Task *nt) {
        // Only add the task if the queue isn't marked finished
        if (!finished) {
            // Lock the queue
            pthread_mutex_lock(&qmtx);
            // Add the task
            tasks.push(nt);
            // signal there's new work
            pthread_cond_signal(&wcond);
            // Unlock the mutex
            pthread_mutex_unlock(&qmtx);
        }
    }
    // Mark the queue finished
    void finish() {
        pthread_mutex_lock(&qmtx);
        finished = true;
        // Signal the condition variable in case any threads are waiting
        pthread_cond_broadcast(&wcond);
        pthread_mutex_unlock(&qmtx);
    }

    // Check if there's work
    bool hasWork() {
	//printf("task queue size is %d\n",tasks.size());
        return (tasks.size()>0);
    }
    
private:
    std::queue<Task*> tasks;
    bool finished;
    pthread_mutex_t qmtx;
    pthread_cond_t wcond;
};


// stdout is a shared resource, so protected it with a mutex
static pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;



// Function that retrieves a task from a queue, runs it and deletes it
 void *getWork(void* param) {
    Task *mw = 0;
pthread_mutex_lock(&console_mutex);
WorkQueue *wq = (WorkQueue*)param;
    pthread_mutex_unlock(&console_mutex);
    
//printf("hi\n");
    while (mw = wq->nextTask()) {
        mw->run();
//printf("hello\n");
        delete mw;
    }
    pthread_exit(NULL);
}

class ThreadPool {
public:

    // Allocate a thread pool and set them to work trying to get tasks
    ThreadPool(int n) : _numThreads(n) {
	int rc;
        printf("Creating a thread pool with %d threads\n", n);
        threads = new pthread_t[n];
        for (int i=0; i< n; ++i) {
            rc = pthread_create(&(threads[i]), 0, getWork, &workQueue);
		if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      		}
        }
    }

    // Wait for the threads to finish, then delete them
    ~ThreadPool() {
        workQueue.finish();
        //waitForCompletion();
        for (int i=0; i<_numThreads; ++i) {
            pthread_join(threads[i], 0);
        }
        delete [] threads;
    }

    // Add a task
    void addTask(Task *nt) {
        workQueue.addTask(nt);
    }
    // Tell the tasks to finish and return
    void finish() {
        workQueue.finish();
    }
    
    // Checks if there is work to do
    bool hasWork() {
        return workQueue.hasWork();
    }
    // Super inefficient way to wait for all tasks to finish
    void waitForCompletion() {
        while (workQueue.hasWork()) {
	}
    }
    
private:
    pthread_t * threads;
    int _numThreads;
    WorkQueue workQueue;
};


// Debugging function
void showTask(int n) {
    pthread_mutex_lock(&console_mutex);
    pthread_mutex_unlock(&console_mutex);
}

// Task to compute fibonacci numbers
// It's more efficient to use an iterative algorithm, but
// the recursive algorithm takes longer and is more interesting
// than sleeping for X seconds to show parrallelism
class FibTask : public Task {
public:
    FibTask(int n) : Task(), _n(n) {}
    ~FibTask() {
        // Debug prints
        pthread_mutex_lock(&console_mutex);
        printf("tid(%d) - fibd(%d) being deleted\n", pthread_self(), _n);
        pthread_mutex_unlock(&console_mutex);        
    }
    virtual void run() {
        // Note: it's important that this isn't contained in the console mutex lock
        long long val = innerFib(_n);
        // Show results
        pthread_mutex_lock(&console_mutex);
        printf("Fibd %d = %lld\n",_n, val);
        pthread_mutex_unlock(&console_mutex);


        // The following won't work in parrallel:
        //   pthread_mutex_lock(&console_mutex);
        //   printf("Fibd %d = %lld\n",_n, innerFib(_n));
        //   pthread_mutex_unlock(&console_mutex);
    }
    virtual void showTask() {
        // More debug printing
        pthread_mutex_lock(&console_mutex);
        printf("thread %d computing fibonacci %d\n", pthread_self(), _n);
        pthread_mutex_unlock(&console_mutex);        
    }
private:
    // Slow computation of fibonacci sequence
    // To make things interesting, and perhaps imporove load balancing, these
    // inner computations could be added to the task queue
    // Ideally set a lower limit on when that's done
    // (i.e. don't create a task for fib(2)) because thread overhead makes it
    // not worth it
    long long innerFib(long long n) {
        if (n<=1) { return 1; }
        return innerFib(n-1) + innerFib(n-2);
    }
    long long _n;
};

int main(int argc, char *argv[]) {

    // Create a thread pool
    ThreadPool *tp = new ThreadPool(10);

    // Create work for it
    for (int i=0;i<100; ++i) {
        int rv = rand() % 40 + 1;
        showTask(rv);
        tp->addTask(new FibTask(rv));
    }
    delete tp;

    printf("\n\n\n\n\nDone with all work!\n");
}
