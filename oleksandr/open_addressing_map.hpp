#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

struct OpenAddressingMap
{
  template <typename Iterator>
  OpenAddressingMap(Iterator begin, Iterator end) {
    auto size = std::distance(begin, end);
    data.resize(2 * size);
    strings.reserve(size);

    std::for_each(begin, end, [this](auto value) { insert(value); });
  }

  template <typename Container>
  OpenAddressingMap(const Container& c)
    : OpenAddressingMap(std::begin(c), std::end(c)) {}

  void insert(const std::string& value) {
    auto [start, hash] = getPosition(data, value);

    for (auto it = start; it != start - 1;) {
      auto& optionalValue = *it;

      if (!optionalValue.data) {
        strings.emplace_back(value);
        optionalValue.data = &strings.back();
        optionalValue.hash = hash;
        return;
      }

      ++it;
      if (it == data.end()) {
        it = data.begin();
      }
    }

    throw std::runtime_error("No space");
  }

  bool has(std::string_view value) const {
    auto [start, hash] = getPosition(data, value);
    for (auto it = start; it != start - 1;) {
      auto& optionalValue = *it;
      if (!optionalValue.data) {
        return false;
      }

      if (optionalValue.hash == hash && *optionalValue.data == value) {
        return true;
      }

      ++it;
      if (it == data.end()) {
        it = data.begin();
      }
    }

    return false;
  }

  size_t size() const {
    return strings.size();
  }

private:
  template <typename T>
  static auto getPosition(T&& t, std::string_view value) -> std::pair<decltype(t.begin()), size_t> {
    auto hash = std::hash<std::string_view>()(value);
    auto position = hash % t.size();

    return {t.begin() + position, hash};
  }

  struct Bucket
  {
    std::string* data = nullptr;
    size_t hash = 0;
  };

  std::vector<Bucket> data;
  std::vector<std::string> strings;
};
