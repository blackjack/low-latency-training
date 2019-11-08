#include <benchmark/benchmark.h>

#include <papipp/papipp.h>

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

  papi::event_set<PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L3_LDM> events;
  events.start_counters();

  for (auto _ : state) {
    int res = 0;
    for (auto i : indices) {
      res += data[i];
    }

    benchmark::DoNotOptimize(res);
  }

  events.stop_counters();
  double ipc = double(events.get<PAPI_TOT_INS>().counter()) / events.get<PAPI_TOT_CYC>().counter();
  state.counters["ipc"] = ipc;
  state.counters["l3m"] = events.get<PAPI_L3_LDM>().counter();
  state.SetBytesProcessed(2 * size * sizeof(int) * state.iterations());
  state.counters["data"] = 2 * size * sizeof(int);
}

template <typename Container>
static void random(benchmark::State& state) {
  auto size = state.range(0);

  auto data = generateContainer<Container, Random>(size);
  auto indices = generateContainer<Container, Sequential<int>>(size);

  std::random_shuffle(indices.begin(), indices.end());

  papi::event_set<PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L3_LDM> events;
  events.start_counters();

  for (auto _ : state) {
    int res = 0;
    for (auto i : indices) {
      res += data[i];
    }

    benchmark::DoNotOptimize(res);
  }

  events.stop_counters();
  double ipc = double(events.get<PAPI_TOT_INS>().counter()) / events.get<PAPI_TOT_CYC>().counter();
  state.counters["ipc"] = ipc;
  state.counters["l3m"] = events.get<PAPI_L3_LDM>().counter();
  state.SetBytesProcessed(2 * size * sizeof(int) * state.iterations());
  state.counters["data"] = 2 * size * sizeof(int);
}

BENCHMARK_TEMPLATE(sequential, std::vector<int>)->RangeMultiplier(2)->Range(128, 1024 * 1024 * 4);
BENCHMARK_TEMPLATE(random, std::vector<int>)->RangeMultiplier(2)->Range(128, 1024 * 1024 * 4);

BENCHMARK_MAIN();
