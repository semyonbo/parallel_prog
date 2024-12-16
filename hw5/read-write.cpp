#include <iostream>
#include <thread>
#include <shared_mutex>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

using namespace std;

// переделал на shared_mutex!

class ReadWrite {
public:
    ReadWrite() : alive(true) {}
    ~ReadWrite() { stop(); }
    void read(int id);
    void write(int id);
    void stop();

private:
    vector<string> data;
    shared_mutex rw_mutex;
    bool alive;
};

void ReadWrite::read(int id) {
    while (alive) {
        {
           shared_lock<shared_mutex> lock(rw_mutex); // Разрешаем одновременное чтение
            if (!data.empty()) {
                cout << "Reader " << id << " reads: ";
                for (const auto& info : data) {
                    cout << info << " ";
                }
                cout << endl;
            } else {
                cout << "Reader " << id << " sees no data." << endl;
            } 
        }
        this_thread::sleep_for(chrono::seconds(1)); // а тут отдохнем
    }
    cout << "Reader " << id << " stopped." << endl;
}

void ReadWrite::write(int id) {
    int count = 0;
    while (alive) {
        {
            lock_guard<shared_mutex> lock(rw_mutex); // Блокируем запись
            string new_data = "Data_" + to_string(id) + "_" + to_string(count++);
            data.push_back(new_data);
            cout << "Writer " << id << " wrote: " << new_data << endl;
        }
        this_thread::sleep_for(chrono::seconds(2)); // отдыхаем
    }
}

void ReadWrite::stop() {
    alive = false; // НАДОЕЛО ВСЁ!
}

int main() {
    ReadWrite storage;

    thread reader1(&ReadWrite::read, &storage, 1);
    thread writer1(&ReadWrite::write, &storage, 1);
    thread reader2(&ReadWrite::read, &storage, 2);
    thread writer2(&ReadWrite::write, &storage, 2);

    this_thread::sleep_for(chrono::seconds(5));

    storage.stop();

    reader1.join();
    writer1.join();
    reader2.join();
    writer2.join();

    cout << "Finished." << endl;
    return 0;
}
