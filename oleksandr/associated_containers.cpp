#include <benchmark/benchmark.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include <cstring>

const auto DICT = "/etc/dictionaries-common/words";

struct Vector : std::vector<std::string>
{
  Vector(std::vector<std::string>&& data)
    : std::vector<std::string>(std::move(data)) {
    std::sort(begin(), end());
  }

  bool has(std::string_view word) const {
    return std::binary_search(begin(), end(), word);
  }
};

template <typename Underlying>
struct Set : Underlying
{
  Set(Underlying&& data)
    : Underlying(std::move(data)) {}

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
class Dictionary
{
public:
  Dictionary(std::vector<std::string>&& words)
    : t(std::move(words)) {}

  bool isInDictionary(std::string_view word) const {
    return t.has(word);
  }

  size_t size() const {
    return t.size();
  }

private:
  Table t;
};

std::vector<std::string> loadWords(const std::filesystem::path& path = DICT) {
  std::vector<std::string> result;

  std::ifstream stream(path);
  if (!stream) {
    throw std::runtime_error("Failed to open: " + path.string() + ": " + std::strerror(errno));
  }

  std::string line;
  while (std::getline(stream, line)) {
    result.push_back(line);
  }

  return result;
}

template <typename Table>
static void search(benchmark::State& state) {
  auto size = state.range(0);

  auto words = loadWords();

  auto searched = words;
  std::random_shuffle(searched.begin(), searched.end());

  Dictionary<Table> dict(std::move(words));

  for (auto _ : state) {
    for (auto& word : searched) {
      bool has = dict.isInDictionary(word);
      benchmark::DoNotOptimize(has);
    }
  }

  state.counters["lookup_size"] = searched.size();
  state.counters["dict_size"] = dict.size();
}

BENCHMARK_TEMPLATE(search, Vector);
BENCHMARK_TEMPLATE(search, Set<std::set<std::string_view>>);
BENCHMARK_TEMPLATE(search, Set<std::unordered_set<std::string_view>>);
BENCHMARK_TEMPLATE(search, Set<std::set<std::string>>);
BENCHMARK_TEMPLATE(search, Set<std::unordered_set<std::string>>);
BENCHMARK_MAIN();
