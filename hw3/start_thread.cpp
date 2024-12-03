#include <iostream>
#include <vector>
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
    void add(uint64_t value);
    void remove(uint64_t value);

private:
    void run();
    atomic<uint64_t> sum =0;
    vector<uint64_t> data;
    mutex dataMutex;
    condition_variable cv;
    thread worker;
    bool stopRequested;
};

DynamicThreadSum::DynamicThreadSum() : stopRequested(true) {}

DynamicThreadSum::~DynamicThreadSum()
{
    stop();
}

void DynamicThreadSum::start()
{
    if (!stopRequested)
    {
        return;
    }
    cout << "started "<<endl;
    stopRequested = false;
    worker = thread(&DynamicThreadSum::run, this);
}

void DynamicThreadSum::stop()
{
    if (stopRequested){
        return;
    }
    stopRequested = true;
    cv.notify_all();
    if (worker.joinable())
    {
        worker.join();
    }
    cout<<"stopped "<<endl;
}

void DynamicThreadSum::add(uint64_t value)
{
    lock_guard<mutex> lock(dataMutex);
    data.push_back(value);
    sum += value;
    cout<<"added "<<value <<endl;
    //cv.notify_all();
}

void DynamicThreadSum::remove(uint64_t value)
{
    lock_guard<mutex> lock(dataMutex);
    auto elemen_id = find(data.begin(), data.end(), value);
    if (elemen_id != data.end())
    {
        sum -= value;
        data.erase(elemen_id);
        cout<<"removed "<<value<<endl;
    }
    //cv.notify_all();
}

void DynamicThreadSum::run()
{
    std::mutex m;

    while (!stopRequested)
    {
        //unique_lock<mutex> lock(m);
        unique_lock<mutex> lock(dataMutex);
        cv.wait_for(lock, chrono::seconds(5), [this]
                    { return stopRequested; });

        // if (stopRequested)
        // {
        //     break;
        // }

        auto current_time = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(current_time);

        cout << "Current time: " << ctime(&time)
             << "Sum: " << sum.load() << endl;

        cout << "Data: [";
        //unique_lock<mutex> lock(dataMutex);
        for (size_t i = 0; i < data.size(); ++i)
        {
            cout << data[i];
            if (i < data.size() - 1)
            {
                cout << ", ";
            }
        }
        cout << "]" << endl;
    }
}

int main()
{
    DynamicThreadSum sum;
    sum.start();
    sum.add(10);
    this_thread::sleep_for(chrono::seconds(10));
    sum.remove(10);
    this_thread::sleep_for(chrono::seconds(10));
    sum.stop();
    return 0;
}