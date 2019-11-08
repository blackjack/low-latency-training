#include <benchmark/benchmark.h>

#include "open_addressing_map.hpp"

#include "../maciek/dictionary/utils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <cstring>

const auto DICT = "/etc/dictionaries-common/words";

struct BinarySearch
{
  template <typename It, typename Value>
  bool operator()(It&& begin, It&& end, Value&& value) const {
    return std::binary_search(begin, end, std::forward<Value>(value));
  }
};

struct LinearSearch
{
  template <typename It, typename Value>
  bool operator()(It&& begin, It&& end, Value&& value) const {
    return std::find(begin, end, std::forward<Value>(value)) != end;
  }
};

template <typename Algorithm>
struct Vector : std::vector<std::string>
{
  Vector(const std::vector<std::string>& data)
    : std::vector<std::string>(data) {
    std::sort(begin(), end());
  }

  bool has(std::string_view word) const {
    return Algorithm()(begin(), end(), word);
  }
};

template <typename Underlying>
struct Set : Underlying
{
  template <typename T>
  Set(const T& data)
    : Underlying(std::begin(data), std::end(data)) {}

  bool has(std::string_view word) const {
    using Key = typename Underlying::key_type;
    if constexpr (std::is_convertible_v<std::string_view, Key>) {
      return Underlying::find(word) != Underlying::end();
    } else {
      Key key(word);
      return Underlying::find(key) != Underlying::end();
    }
  }
};

template <typename Table>
class MyDictionary
{
public:
  MyDictionary(const std::vector<std::string>& words)
    : t(words) {}

  bool isInDictionary(std::string_view word) const {
    return t.has(word);
  }

  size_t size() const {
    return t.size();
  }

private:
  Table t;
};

std::vector<std::string> wordsIn;    // = load();
std::vector<std::string> wordsNotIn; // = loadMangled();

template <typename Table>
static void InDictionary(benchmark::State& state) {
  MyDictionary<Table> dict(wordsIn);

  bool allIn = true;
  auto it = wordsIn.begin();
  for (auto _ : state) {
    allIn = allIn && dict.isInDictionary(*it);
    ++it;
    if (it == wordsIn.end())
      it = wordsIn.begin();
  }

  if (!allIn)
    throw std::runtime_error("Expected all words to be in");

  state.counters["dict_size"] = dict.size();
  state.counters["words_in_size"] = wordsIn.size();
}

template <typename Table>
static void NotInDictionary(benchmark::State& state) {
  MyDictionary<Table> dict(wordsIn);

  bool someIn = false;
  auto it = wordsNotIn.begin();
  for (auto _ : state) {
    someIn = someIn || dict.isInDictionary(*it);
    ++it;
    if (it == wordsNotIn.end())
      it = wordsNotIn.begin();
  }

  if (someIn)
    throw std::runtime_error("Expected no words to be in");

  state.counters["dict_size"] = dict.size();
  state.counters["words_not_in_size"] = wordsNotIn.size();
}

BENCHMARK_TEMPLATE(InDictionary, Vector<LinearSearch>);
BENCHMARK_TEMPLATE(InDictionary, Vector<BinarySearch>);
BENCHMARK_TEMPLATE(InDictionary, Set<std::set<std::string_view>>);
BENCHMARK_TEMPLATE(InDictionary, Set<std::unordered_set<std::string_view>>);
BENCHMARK_TEMPLATE(InDictionary, Set<std::set<std::string>>);
BENCHMARK_TEMPLATE(InDictionary, Set<std::unordered_set<std::string>>);
BENCHMARK_TEMPLATE(InDictionary, OpenAddressingMap);
BENCHMARK_TEMPLATE(NotInDictionary, Vector<LinearSearch>);
BENCHMARK_TEMPLATE(NotInDictionary, Vector<BinarySearch>);
BENCHMARK_TEMPLATE(NotInDictionary, Set<std::set<std::string_view>>);
BENCHMARK_TEMPLATE(NotInDictionary, Set<std::unordered_set<std::string_view>>);
BENCHMARK_TEMPLATE(NotInDictionary, Set<std::set<std::string>>);
BENCHMARK_TEMPLATE(NotInDictionary, Set<std::unordered_set<std::string>>);
BENCHMARK_TEMPLATE(NotInDictionary, OpenAddressingMap);

int main(int argc, char** argv) {
  static const int DICT_SIZE = 100'000;
  std::vector<std::string> words =
    createVectorOfUniqueRandomStrings(DICT_SIZE * 2);
  wordsIn.assign(words.begin(), words.begin() + DICT_SIZE);
  wordsNotIn.assign(words.begin() + DICT_SIZE, words.end());

  benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  benchmark::RunSpecifiedBenchmarks();
}
