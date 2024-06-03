#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::vector<int> sharedVector;
std::mutex mtx;

void threadFunc(int threadID) {
    for (int i = 0; i < sharedVector.size(); ++i) {
        mtx.lock();

        if (sharedVector[i] == 0) {
            sharedVector[i]++;
            std::cout << "Thread " << threadID << " incremented element " << i  << std::endl;
        }

        mtx.unlock();
    }
}

int main() {
    const int N = 5; // Количество садовников
    sharedVector = std::vector<int>(100000, 0); // Инициализируем вектор цветов нулями
    std::vector<std::thread> threads;

    for (int i = 0; i < N; ++i) {
        threads.push_back(std::thread(threadFunc, i));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Возврат 1, если не удалось
    for (int i = 0; i < sharedVector.size(); ++i) {
        if ((sharedVector[i] == 0) || (sharedVector[i] > 1) ){ return 1;}
    }

    return 0;
}
