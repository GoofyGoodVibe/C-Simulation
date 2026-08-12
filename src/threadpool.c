#include "threadpool.h"
ThreadPool g_pool;

static int worker_thread(void* arg) {
    (void)arg; // unused for now
    ThreadPool* pool = (ThreadPool*)arg;

    while (true) {
        SDL_LockMutex(pool->mutex); // Lock the mutex to protect access to the task queue

        // Wait for a task to be added to the queue
        while (pool->size == 0 && !pool->shutdown) {
            SDL_WaitCondition(pool->condition, pool->mutex);
        }

        // If the thread pool is shutting down, exit the loop
        if (pool->shutdown) {
            SDL_UnlockMutex(pool->mutex);
            break;
        }

        // Get the next task from the queue
        Task task = pool->tasks[--pool->size];

        SDL_UnlockMutex(pool->mutex); // Unlock the mutex before executing the task

        // Execute the task
        task.function(task.arg);
    }

    return 0;
}

void pool_init(void) {
    g_pool.size = 0;
    g_pool.mutex = SDL_CreateMutex();
    g_pool.condition = SDL_CreateCondition();
    g_pool.shutdown = false;
    
    int cores = SDL_GetNumLogicalCPUCores();
    if(cores <= 0) cores = 1; // Fallback to 1 if SDL_GetNumLogicalCPUCores() fails
    if (cores > MAX_WORKER_THREADS) {
        g_pool.num_workers = MAX_WORKER_THREADS;
    } else {
        g_pool.num_workers = cores;
    }

    // Create worker threads
    for (int i = 0; i < g_pool.num_workers; ++i) {
        g_pool.threads[i] = SDL_CreateThread(worker_thread, "WorkerThread", &g_pool);
    }
}

void pool_push_task(void (*func)(void*), void* arg) {
    SDL_LockMutex(g_pool.mutex); // Lock the mutex to protect access to the task queue

    if(g_pool.size >= MAX_TASKS) {
        SDL_Log("Task queue is full. Task not added.");
        SDL_UnlockMutex(g_pool.mutex);
        return;
    }

    // Add the new task to the queue
    g_pool.tasks[g_pool.size++] = (Task){func, arg};

    SDL_SignalCondition(g_pool.condition); // Signal a worker thread that a new task is available
    SDL_UnlockMutex(g_pool.mutex);
}

void pool_shutdown(void) {
    SDL_LockMutex(g_pool.mutex); // Lock the mutex to protect access to the task queue

    g_pool.shutdown = true; // Set the shutdown flag

    SDL_BroadcastCondition(g_pool.condition); // Wake up all waiting threads
    SDL_UnlockMutex(g_pool.mutex); // Unlock the mutex

    // Wait for all threads to finish
    for (int i = 0; i < g_pool.num_workers; ++i) {
        SDL_WaitThread(g_pool.threads[i], NULL);
    }

    // Clean up resources
    SDL_DestroyCondition(g_pool.condition);
    SDL_DestroyMutex(g_pool.mutex);

    SDL_Log("Thread pool shutdown complete.");
}