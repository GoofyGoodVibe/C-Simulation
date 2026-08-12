#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define MAX_WORKER_THREADS 64 // Number of worker threads 10 for now untill I can figure out how to make it dynamic
#define MAX_TASKS 500 // Maximum number of tasks in the queue

typedef struct{
    void(*function)(void*); // Function pointer to the function that will be executed by the thread
    void* arg; // Argument to be passed to the function
} Task;

typedef struct{
    SDL_Thread* threads[MAX_WORKER_THREADS]; // Array of worker threads
    Task tasks[MAX_TASKS]; // Array of tasks to be executed by the threads
    int size; // Number of tasks in the queue
    int num_workers; // Number of worker threads
    SDL_Mutex* mutex; // Mutex to protect access to the task queue
    SDL_Condition* condition; // Condition variable to signal when a task is added to the queue
    bool shutdown; // Flag to indicate if the thread pool is shutting down
} ThreadPool;

// Global instance accessible by any .c file
extern ThreadPool g_pool;

void pool_init(void);
void pool_push_task(void (*func)(void*), void* arg);
void pool_shutdown(void);

#endif