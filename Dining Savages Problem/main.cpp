#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include "utilities.hpp"

class DiningSavages {
    std::mutex mu;
    std::condition_variable cv;
    int M, m;
public:    
    DiningSavages(int M) : M(M), m(0) {}

    void savage(Helper& helper) {
        while(true) {
            std::unique_lock lk(mu);
            if (m < 1)
                cv.wait(lk, [this] { return m>0; });
            m--;
            cv.notify_all();

            helper.getServingFromPot();
            
            helper.eat();
        }
    }

    void cook(Helper& helper) {
        while(true) {
            std::unique_lock lk(mu);
            if (m > 0)
                cv.wait(lk, [this] { return m < 1; });

            helper.putServingInPot(M);
            m += M;
            cv.notify_all();
        }
    }
};

void process(int M, int n) {
    DiningSavages ds {M};
    Helper helper;

    //create thread pool
    std::vector<std::thread> pool;
    pool.emplace_back(&DiningSavages::cook, &ds, std::ref(helper));
    for (int i=0; i<n; i++) {
        pool.emplace_back(&DiningSavages::savage, &ds, std::ref(helper));
    }

    for (auto& thr : pool)
        thr.join();

    std::cout << std::endl;
}

int main() {
    int M = 8;
    int n = 20;
    process(M, n);
    return 0;
}