#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

// Количество философов и вилок
const int NUM_PHILOSOPHERS = 5;

class DiningPhilosophers {
public:
    DiningPhilosophers() : forks(NUM_PHILOSOPHERS) {}

    // Метод философа
    void philosopher(int id) {
        while (true) {
            think(id);      // Размышление
            eat(id);        // Еда
        }
    }

private:
    vector<mutex> forks; // Мьютексы для вилок

    // Философ размышляет
    void think(int id) {
        cout << "Philosopher " << id << " is thinking." << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    // Философ ест
    void eat(int id) {
        int leftFork = id;                    // Левая вилка
        int rightFork = (id + 1) % NUM_PHILOSOPHERS; // Правая вилка

        // Регламент: всегда сначала брать вилку с меньшим номером
        if (leftFork > rightFork) {
            swap(leftFork, rightFork);
        }

        // Берем вилки
        forks[leftFork].lock();
        cout << "Philosopher " << id << " picked up left fork " << leftFork << "." << endl;

        forks[rightFork].lock();
        cout << "Philosopher " << id << " picked up right fork " << rightFork << "." << endl;

        // Едим
        cout << "Philosopher " << id << " is eating." << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));

        // Кладем вилки
        forks[rightFork].unlock();
        cout << "Philosopher " << id << " put down right fork " << rightFork << "." << endl;

        forks[leftFork].unlock();
        cout << "Philosopher " << id << " put down left fork " << leftFork << "." << endl;
    }
};

int main() {
    DiningPhilosophers table;

    // Запускаем философов
    vector<thread> philosophers;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(&DiningPhilosophers::philosopher, &table, i);
    }

    // Ждем завершения (в данном случае потоки работают бесконечно)
    for (auto& phil : philosophers) {
        phil.join();
    }

    return 0;
}
