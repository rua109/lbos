#pragma once

#include <pthread.h>
#include <stdio.h>

struct CarFunctions {
    void load() {
        std::cout << "load" << std::endl;
    }

    void unload() {
        std::cout << "unload" << std::endl;
    }

    void run() {
        std::cout << "run" << std::endl;
    }
};

struct PassengerFunctions {
    void board() {
        std::cout << "board" << std::endl;
    }

    void unboard() {
        std::cout << "unboard" << std::endl;
    }
};

class Semaphore {
    int n, wakeups;
    pthread_mutex_t mu;
    pthread_cond_t cv;

public:
    Semaphore(int n) {
        pthread_cond_init(&cv, 0);
        pthread_mutex_init(&mu, 0);
    }

    void wait() {
        pthread_mutex_lock(&mu);
        n--;
        if (n < 0) {
            do {
                pthread_cond_wait(&cv, &mu);
            } while (wakeups < 1);
            wakeups--;
        }
        pthread_mutex_unlock(&mu);
    }

    void signal() {
        pthread_mutex_lock(&mu);
        n++;
        if (n <= 0) {
            wakeups++;
            pthread_cond_broadcast(&cv);
        }
        pthread_mutex_unlock(&mu);
    }

    void signal(int cnt) {
        while (cnt-- > 0)
            signal();
    }

    ~Semaphore() {
        pthread_mutex_destroy(&mu);
        pthread_cond_destroy(&cv);
    }
};