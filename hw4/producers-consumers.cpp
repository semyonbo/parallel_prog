#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

using namespace std;

class ProdCons
{
public:
    ProdCons(size_t max_size);
    ~ProdCons();
    void stop();
    void consume();
    void produce();

private:
    queue<int> msg_storage;
    const size_t max_msgs;
    mutex mtx;
    condition_variable cv_not_full;
    condition_variable cv_not_empty;
    int products_id = 0;
    bool alive = true;
};

ProdCons::ProdCons(size_t max_size) : max_msgs(max_size) {}

ProdCons::~ProdCons()
{
    stop();
}

void ProdCons::stop()
{
    if (!alive)
    {
        return;
    }
    unique_lock<mutex> lock(mtx);
    alive = false;
}

void ProdCons::produce()
{
    while (alive)
    {
        this_thread::sleep_for(chrono::seconds(1));
        unique_lock<mutex> lock(mtx);

        cv_not_full.wait(lock, [this]
                         { return msg_storage.size() < max_msgs || !alive; });

        ++products_id;
        msg_storage.push(products_id);
        cout << "Produced: " << products_id << endl;
        cv_not_empty.notify_one();
    }
}

void ProdCons::consume()
{
    while (alive)
    {
        this_thread::sleep_for(chrono::seconds(1));
        unique_lock<mutex> lock(mtx);
        cv_not_empty.wait(lock, [this]
                          { return !msg_storage.empty() || !alive; });
        int prod = msg_storage.front();
        msg_storage.pop();
        cout << "Consumed: " << prod << endl;
        cv_not_full.notify_one();
    }
}

int main()
{
    const size_t max_msgs = 5;
    ProdCons pc(max_msgs);

    thread producer(&ProdCons::produce, &pc);
    thread consumer(&ProdCons::consume, &pc);

    this_thread::sleep_for(chrono::seconds(5));

    pc.stop();

    producer.join();
    consumer.join();

    cout << "Finished." << endl;
    return 0;
}