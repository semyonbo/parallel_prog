#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

const int NUM_PHILOSOPHERS = 5;


//Пределал на std::unique_lock  

class DiningPhilosophers {
public:
    DiningPhilosophers() : forks(NUM_PHILOSOPHERS), stop_flag(false) {}

    void philosopher(int id) {
        while (!stop_flag) {
            think(id);
            eat(id);
        }
        printMessage(id, "has finished working.");
    }

    void stop(){
        stop_flag = true;
    }

private:
    vector<mutex> forks; //вилки
    bool stop_flag;
    mutex print_mutex;

    void printMessage(int id, const string& message) {
        lock_guard<mutex> lock(print_mutex);
        cout << "Philosopher " << id << " " << message << endl;
    }

    void think(int id) {
        printMessage(id, "is thinking.");
        this_thread::sleep_for(chrono::seconds(2));
    }

    void eat(int id) {
        int leftFork = id;
        int rightFork = (id + 1) % NUM_PHILOSOPHERS;

        {
            lock(forks[leftFork], forks[rightFork]);
            lock_guard<mutex> leftLock(forks[leftFork], adopt_lock);
            lock_guard<mutex> rightLock(forks[rightFork], adopt_lock);

            printMessage(id, "picked up forks " + to_string(leftFork) + " and " + to_string(rightFork) + ".");

            printMessage(id, "is eating.");
            this_thread::sleep_for(chrono::milliseconds(1000));
        }

        printMessage(id, "put down forks.");
    }
};

int main() {
    DiningPhilosophers table;

    vector<thread> philosophers;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(&DiningPhilosophers::philosopher, &table, i);
    }

    this_thread::sleep_for(chrono::seconds(5));
    table.stop();
    

    for (auto& phil : philosophers) {
        if (phil.joinable()){
            phil.join();
        }
    }

    return 0;
}
