#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "utilities.hpp"

// build this fn
class RollerCoaster {
    std::mutex mu;
    int C, boarders, unboarders;
    Semaphore boardQueue;
    Semaphore unboardQueue;
    Semaphore allAbroad;
    Semaphore allAshore;

public:
    RollerCoaster(int C) : 
        C(C), boardQueue(0), unboardQueue(0), allAbroad(0),
        allAshore(0) {

    }

    void car(CarFunctions& fn) {

        fn.load();

        boardQueue.signal(C);
        allAbroad.wait();

        fn.run();

        unboardQueue.signal(C);
        allAshore.wait();
        fn.unload();
    }

    void passenger(PassengerFunctions& fn) {
        boardQueue.wait();
        fn.board();
        
        {
            std::unique_lock lk(mu);
            boarders++;
            if (boarders == C) {
                allAbroad.signal();
                boarders = 0;
            }
        }
        
        unboardQueue.wait();
        fn.unboard();
        {
            std::unique_lock lk(mu);
            unboarders++;
            if (unboarders == C) {
                allAshore.signal();
                unboarders = 0;
            }
        }

    }
};

int main() {
    int C, n;
    std::cin >> C >> n;

    PassengerFunctions pfn;
    CarFunctions cfn;
    RollerCoaster rc {C};

    std::vector<std::thread> pool;

    pool.emplace_back( &RollerCoaster::car, &rc, std::ref(cfn));
    for(int i=0; i<n; i++)
        pool.emplace_back( &RollerCoaster::passenger, &rc, std::ref(pfn) );

    for (auto& th: pool)
        th.join();

    return 0;
}