#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

const int NUM_PHILOSOPHERS = 5;

class DiningPhilosophers {
public:
    DiningPhilosophers() : forks(NUM_PHILOSOPHERS) {}

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
    bool stop_flag = false;
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
        //начинаем с левой вилки
        if (leftFork > rightFork) {
            swap(leftFork, rightFork);
        }

        forks[static_cast<size_t>(leftFork)].lock();
        printMessage(id, "took left fork " + to_string(leftFork) + ".");

        forks[static_cast<size_t>(rightFork)].lock();
        printMessage(id, "took right fork " + to_string(rightFork) + ".");

        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::seconds(3));

        // Кладем вилки
        forks[static_cast<size_t>(rightFork)].unlock();
        printMessage(id, "returned right fork " + to_string(rightFork) + ".");

        forks[static_cast<size_t>(leftFork)].unlock();
        printMessage(id, "returned left fork " + to_string(leftFork) + ".");
    }
};

int main() {
    DiningPhilosophers table;

    vector<thread> philosophers;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(&DiningPhilosophers::philosopher, &table, i);
    }

    table.stop();


    for (auto& phil : philosophers) {
        phil.join();
    }

    return 0;
}
