#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <condition_variable>

using namespace std;

class DynamicThreadSum
{
public:
    DynamicThreadSum();
        ~DynamicThreadSum();
    void start();
    void stop();
    void add(int value);
    void remove(int value);

private:
    void run();
    int sum();
    vector<int> data;
    mutex dataMutex;
    condition_variable cv;
    thread worker;
    bool stopRequested;
};

DynamicThreadSum::DynamicThreadSum() : stopRequested(false) {}

DynamicThreadSum::~DynamicThreadSum()
{
    stop();
}

void DynamicThreadSum::start()
{
    if (worker.joinable()){
        return;
    }
    stopRequested = false;
    worker = thread(&DynamicThreadSum::run, this);
}

void DynamicThreadSum::stop(){
    {
        lock_guard<mutex> lock(dataMutex);
        stopRequested = true;
    }
    cv.notify_all();
    if (worker.joinable()){
        worker.join();
    }
}

void DynamicThreadSum::add(int value)
{
    lock_guard<mutex> lock(dataMutex);
    data.push_back(value);
    cv.notify_all();
}

void DynamicThreadSum::remove(int value)
{
    lock_guard<mutex> lock(dataMutex);
    data.erase(std::remove(data.begin(), data.end(), value), data.end());
    cv.notify_all();
}

void DynamicThreadSum::run()
{
    while (true)
    {
        unique_lock<mutex> lock(dataMutex);
        cv.wait_for(lock, chrono::seconds(5), [this] { return stopRequested; });
        
        if (stopRequested)
        {
            break;
        }

        auto current_time = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(current_time);

        std::cout << "Current time: " << std::ctime(&time)
                  << "Sum: " << sum() << std::endl;
    }
}

int DynamicThreadSum::sum()
{
    lock_guard<mutex> lock(dataMutex);
    int result = 0;
    for (int value : data)
    {
        result += value;
    }
    return result;
}

int main()
{
    DynamicThreadSum sum;
    sum.start();

    sum.add(10);
    sum.add(15);
    this_thread::sleep_for(std::chrono::seconds(35));
    sum.add(20);
    sum.remove(15);
    sum.stop();
    return 0;
} 