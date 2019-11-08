#include <benchmark/benchmark.h>

#include <papipp/papipp.h>

static void start_stop1(benchmark::State& state) {
  papi::event_set<PAPI_TOT_INS> events;

  for (auto _ : state) {
    events.start_counters();
    events.stop_counters();
  }
}

static void start_stop2(benchmark::State& state) {
  papi::event_set<PAPI_TOT_INS, PAPI_TOT_CYC> events;

  for (auto _ : state) {
    events.start_counters();
    events.stop_counters();
  }
}

static void start_stop3(benchmark::State& state) {
  papi::event_set<PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L3_LDM> events;

  for (auto _ : state) {
    events.start_counters();
    events.stop_counters();
  }
}

BENCHMARK(start_stop1);
BENCHMARK(start_stop2);
BENCHMARK(start_stop3);
BENCHMARK_MAIN();
