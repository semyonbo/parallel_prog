#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

using namespace std;

class ReadWrite
{
public:
    ReadWrite() : alive(true) {};
    ~ReadWrite(){
        stop();
    };
    void read(int id);
    void write(int id);
    void stop();

private:
    bool writer_works = false;
    vector<string> data;
    mutex mtx;
    condition_variable cv;
    atomic<bool> alive;
};

void ReadWrite::read(int id)
{
    while (alive)
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !writer_works || !alive; });

        cout << "Reader " << id << " reads: ";
        for (auto &info : data)
        {
            cout << info << " ";
        }
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    cout << "Reader " << id << " stopped." << endl;
}

void ReadWrite::write(int id)
{
    int count = 0 ;
    while (alive)
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !writer_works || !alive; });

        writer_works = true;
        string new_data = "Data_" + to_string(id) + "_" + to_string(count++);
        data.push_back(new_data);
        cout << "Writer " << id << " wrote: " << new_data << endl;
        writer_works = false;
        cv.notify_all();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void ReadWrite::stop(){
    alive = false;
    cv.notify_all();
}

int main() {
    ReadWrite storage;

    thread reader1(&ReadWrite::read, &storage, 1);
    thread writer1(&ReadWrite::write, &storage, 1);
    thread reader2(&ReadWrite::read, &storage, 2);

    this_thread::sleep_for(chrono::seconds(10));

    storage.stop();

    reader1.join();
    writer1.join();
    reader2.join();

    std::cout << "Finished."<<endl;
    return 0;
}
