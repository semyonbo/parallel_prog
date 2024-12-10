#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <chrono>
#include <atomic>

using namespace std;

class ReadWrite {
public:
    ReadWrite() : alive(true), writer_active(false), has_data(false) {}
    ~ReadWrite() { stop(); }
    void read(int id);
    void write(int id);
    void stop();

private:
    vector<string> data;
    mutex mtx;
    condition_variable cv;
    atomic<bool> alive;
    bool writer_active;
    bool has_data;
};

void ReadWrite::read(int id) {
    while (alive) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return (!writer_active && has_data) || !alive; }); //ждем пока никто не пишет и данные есть
        cout << "Reader " << id << " reads: ";
        for (const auto& info : data) {
            cout << info << " ";
        }
        cout << endl;

        lock.unlock(); // пусть другие читают
        this_thread::sleep_for(chrono::seconds(1)); // а тут отдохнем
    }
    cout << "Reader " << id << " stopped." << endl;
}

void ReadWrite::write(int id) {
    int count = 0;
    while (alive) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return !writer_active || !alive; }); // никто не пишет и живы
        writer_active = true; // я пишу - всем молчать!
        string new_data = "Data_" + to_string(id) + "_" + to_string(count++);
        data.push_back(new_data);
        has_data = true; // теперь есть что читать
        cout << "Writer " << id << " wrote: " << new_data << endl;
        writer_active = false; // надоело писать
        lock.unlock(); // пусть другие развлекаются
        this_thread::sleep_for(chrono::seconds(2)); // отдыхаем
    }
}

void ReadWrite::stop() {
    alive = false; // НАДОЕЛО ВСЁ!
    cv.notify_all();
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
