#include <benchmark/benchmark.h>

#include <array>
#include <atomic>
#include <numeric>
#include <thread>

struct Data
{
  std::array<int, 8> a;
  std::array<int, 8> b;
};
static_assert(sizeof(Data) == 64);
static_assert(offsetof(Data, b) == 32);

struct DataWithPadding
{
  std::array<int, 8> a;
  std::array<int, 8> gap;
  std::array<int, 8> b;
};
static_assert(sizeof(DataWithPadding) > 64);
static_assert(offsetof(DataWithPadding, b) == 64);

inline void randomize(std::array<int, 8>& array) {
  std::generate(array.begin(), array.end(), std::rand);
  benchmark::DoNotOptimize(array.front());
}

inline void accumulate(std::array<int, 8>& array) {
  auto res = std::accumulate(array.begin(), array.end(), 0);
  benchmark::DoNotOptimize(res);
}

template <typename DataType>
static void sum(benchmark::State& state) {
  DataType data;
  randomize(data.a);
  randomize(data.b);

  for (auto _ : state) {
    accumulate(data.a);
  }
}

void smashThread(std::array<int, 8>* b, std::atomic_bool* stop) {
  while (!*stop) {
    randomize(*b);
  }
}

template <typename DataType>
static void falseSharing(benchmark::State& state) {
  DataType data;
  randomize(data.a);
  randomize(data.b);

  std::atomic_bool stop = false;
  auto t = std::thread(smashThread, &data.b, &stop);

  for (auto _ : state) {
    accumulate(data.a);
  }

  stop = true;
  t.join();
}

void readThread(std::array<int, 8>* b, std::atomic_bool* stop) {
  while (!*stop) {
    auto res = std::accumulate(b->begin(), b->end(), 0);
  }
}

template <typename DataType>
static void sharedRead(benchmark::State& state) {
  DataType data;
  randomize(data.a);
  randomize(data.b);

  std::atomic_bool stop = false;
  auto t = std::thread(readThread, &data.b, &stop);

  for (auto _ : state) {
    accumulate(data.a);
  }

  stop = true;
  t.join();
}

BENCHMARK_TEMPLATE(sum, Data);
BENCHMARK_TEMPLATE(sum, Data);
BENCHMARK_TEMPLATE(sum, Data);
BENCHMARK_TEMPLATE(sum, DataWithPadding);
BENCHMARK_TEMPLATE(sum, DataWithPadding);
BENCHMARK_TEMPLATE(sum, DataWithPadding);
BENCHMARK_TEMPLATE(falseSharing, Data);
BENCHMARK_TEMPLATE(falseSharing, DataWithPadding);
BENCHMARK_TEMPLATE(sharedRead, Data);
BENCHMARK_TEMPLATE(sharedRead, DataWithPadding);

BENCHMARK_MAIN();
