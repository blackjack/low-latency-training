#include <benchmark/benchmark.h>

#include <iostream>
#include <thread>
#include <atomic>

std::atomic_int X = 0, Y = 0, r1 = 0, r2 = 0;

void thread1() {
  X = 1;
  r1 = Y.load();
}

void thread2() {
  Y = 1;
  r2 = X.load();
}

int main() {
  int iteration = 0;
  while (true) {
    X = Y = r1 = r2 = 0;
    std::thread one(thread1);
    std::thread two(thread2);
    one.join();
    two.join();

    if (r1 == 0 && r2 == 0) {
      std::cout << iteration << " error! " << std::endl;
    }
    ++iteration;
  }
}

