#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

using namespace std;

class BarberShop {
private:
    int waitingSeats;
    queue<int> waitingRoom;
    mutex mtx;
    condition_variable barber_cv;
    condition_variable customer_cv;
    bool shopOpen = true;

public:
    BarberShop(int seats) : waitingSeats(seats) {}

    void closeShop() {
        shopOpen = false;
    }

    void customer(int id) {
        unique_lock<mutex> lock(mtx);
        if (!shopOpen || waitingRoom.size() >= static_cast<size_t>(waitingSeats)) {
            cout << "Client " << id << " left (no space or closed).\n";
            return;
        }

        cout << "Client " << id << " sits in the waiting room.\n";
        waitingRoom.push(id);
        barber_cv.notify_one();
        customer_cv.wait(lock, [this, id]() {
            return waitingRoom.empty() || (waitingRoom.front() != id);
        });
    }

    void barber() {
        while (shopOpen || !waitingRoom.empty()) {
            unique_lock<mutex> lock(mtx);

            barber_cv.wait(lock, [this]() {
                return !waitingRoom.empty() || !shopOpen;
            });

            if (!shopOpen && waitingRoom.empty()) {
                cout << "Barber closed shop and goes home.\n";
                return;
            }

            int client_id = waitingRoom.front();
            waitingRoom.pop();
            cout << "Barber serves client " << client_id << ".\n";
            lock.unlock();
            
            this_thread::sleep_for(chrono::seconds(2));

            lock.lock();
            customer_cv.notify_all();
        }
    }
};

void simulateCustomer(BarberShop &shop, int id) {
    shop.customer(id);
}

int main() {
    const int waitingSeats = 3;
    BarberShop shop(waitingSeats);

    thread barberThread(&BarberShop::barber, &shop);

    thread customers[10];
    for (int i = 0; i < 10; ++i) {
        this_thread::sleep_for(chrono::milliseconds(500));
        customers[i] = thread(simulateCustomer, ref(shop), i + 1);
    }

    shop.closeShop();
    cout << "\nClosing the barber shop...\n";
    

    for (auto &customer : customers) {
        if (customer.joinable()) {
            customer.join();
        }
    }

    if (barberThread.joinable()) {
        barberThread.join();
    }

    return 0;
}