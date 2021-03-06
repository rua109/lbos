#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace::std;

class RiverCrossing {
    int h;          // number of Hackers on boat
    int s;          // number of Serfs on boat
    bool go;
    std::mutex mu;
    std::condition_variable cv, cb;

public:
    RiverCrossing() {
        s = h = 0;
    }
    
    /* steps for handling a Linux Hacker */
    void hacker(function<void()> boardHacker) {
        bool isCaptain = false;

        std::unique_lock lk(mu);

        cv.wait(lk, [this] { return !((h + s == 4) || (h == 0 && s == 3) || (h == 2 && s == 1)); });

        // block if h = 0 and s = 3 or s = 1 and h = 2 
        boardHacker();
        h++;
        go = false;

        if (h + s == 4) {
            h = 0;
            s = 0;
            isCaptain = true;
            go = true;
            cb.notify_all();
        }

        cb.wait(lk, [this] { return go; });

        if (isCaptain)
            std::cout << " rowBoat ";

        cv.notify_all();
    }

    /* steps for handling a Microsoft Serf */
    void serf(function<void()> boardSerf) {
        bool isCaptain = false;

        std::unique_lock lk(mu);

        cv.wait(lk, [this] { return !((h + s == 4) || (s == 0 && h == 3) || (s == 2 && h == 1)); });

        boardSerf();
        s++;
        go = false;

        if (h + s == 4) {
            h = 0;
            s = 0;
            isCaptain = true;
            go = true;
            cb.notify_all();
        }

        cb.wait(lk, [this] { return go; });

        if (isCaptain)
            std::cout << " rowBoat ";

        cv.notify_all();
    }
};


void boardHacker() {
    std::cout << "H";
}


void boardSerf() {
    std::cout << "S";
}

void process(std::string input) {
    int n = input.size();
    
    RiverCrossing r;

    //each thread represents a hacker or a serf.
    std::vector<std::thread> pool;
    for (int i=0; i<n; i++) {
        if (input[i] == 'S')
            pool.emplace_back(&RiverCrossing::serf, &r, boardSerf);
        else
            pool.emplace_back(&RiverCrossing::hacker, &r, boardHacker);
    }

    for (auto& thr : pool)
        thr.join();

    std::cout << std::endl;
}

int main() {
    std::string s;
    while (std::cin >> s)
        process(s);

    return 0;
}
