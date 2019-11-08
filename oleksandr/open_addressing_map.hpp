#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

struct OpenAddressingMap
{
  template <typename Iterator>
  OpenAddressingMap(Iterator begin, Iterator end) {
    data.resize(2 * std::distance(begin, end));
    std::for_each(begin, end, [this](auto value) { insert(value); });
  }

  template <typename Container>
  OpenAddressingMap(const Container& c)
    : OpenAddressingMap(std::begin(c), std::end(c)) {}

  void insert(const std::string& value) {
    auto start = getPosition(data, value);
    for (auto it = start; it != start - 1;) {
      auto& optionalValue = *it;
      if (!optionalValue.has_value()) {
        optionalValue = value;
        ++dataSize;
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
    auto start = getPosition(data, value);
    for (auto it = start; it != start - 1;) {
      auto& optionalValue = *it;
      if (!optionalValue.has_value()) {
        return false;
      }

      if (optionalValue.value() == value) {
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
    return data.size();
  }

private:
  template <typename T>
  static auto getPosition(T&& t, std::string_view value) -> decltype(t.begin()) {
    auto hash = std::hash<std::string_view>()(value);
    auto position = hash % t.size();

    return t.begin() + position;
  }

  std::vector<std::optional<std::string>> data;
  size_t dataSize = 0;
};
