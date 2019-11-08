#include <benchmark/benchmark.h>

#include "../maciek/benchmarks1/mtrace.h"
#include "../maciek/benchmarks1/short_alloc.h"

#include <algorithm>
#include <vector>

struct Random
{
  int operator()() const {
    return rand();
  }
};

template <typename T>
struct Sequential
{
  T operator()() {
    return last++;
  }
  T last{};
};

template <class T, std::size_t BufSize = 1024 * 1024>
using SmallVector = std::vector<T, short_alloc<T, BufSize, alignof(T)>>;

template <typename Container, typename Generator>
Container generateContainer(size_t size) {
  Container result;
  Generator generator;
  result.resize(size);
  std::generate(result.begin(), result.end(), generator);
  return result;
}

template <typename Container>
static void sequential(benchmark::State& state) {
  auto size = state.range(0);

  auto data = generateContainer<Container, Random>(size);
  auto indices = generateContainer<Container, Sequential<int>>(size);

  for (auto _ : state) {
    int res = 0;
    for (auto i : indices) {
      res += data[i];
    }

    benchmark::DoNotOptimize(res);
  }

  state.SetBytesProcessed(2 * size * sizeof(int) * state.iterations());
  state.counters["data"] = 2 * size * sizeof(int);
}

template <typename Container>
static void random(benchmark::State& state) {
  auto size = state.range(0);

  auto data = generateContainer<Container, Random>(size);
  auto indices = generateContainer<Container, Sequential<int>>(size);

  std::random_shuffle(indices.begin(), indices.end());

  for (auto _ : state) {
    int res = 0;
    for (auto i : indices) {
      res += data[i];
    }

    benchmark::DoNotOptimize(res);
  }

  state.SetBytesProcessed(2 * size * sizeof(int) * state.iterations());
  state.counters["data"] = 2 * size * sizeof(int);
}

BENCHMARK_TEMPLATE(sequential, std::vector<int>)->RangeMultiplier(2)->Range(128, 512 * 1024);
BENCHMARK_TEMPLATE(random, std::vector<int>)->RangeMultiplier(2)->Range(128, 512 * 1024);

BENCHMARK_MAIN();
