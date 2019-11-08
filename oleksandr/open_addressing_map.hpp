#pragma once

#include <algorithm>
#include <optional>
#include <vector>

template <typename Value>
struct OpenAddressingMap
{
  template <typename Iterator>
  OpenAddressingMap(Iterator begin, Iterator end) {
    data.resize(2*std::distance(begin, end));
    std::for_each(begin, end, [this](auto value) { insert(value); });
  }

  template <typename Container>
  OpenAddressingMap(const Container& c)
    : OpenAddressingMap(std::begin(c), std::end(c)) {}

  void insert(const Value& value) {
    auto hash = std::hash<Value>()(value);
    auto position = hash % data.size();

    for (auto it = data.begin() + position; it != data.end(); ++it) {
      auto& optionalValue = *it;
      if (!optionalValue.has_value()) {
        optionalValue = value;
        return;
      }
    }

    throw std::runtime_error("No space");
  }

  template <typename T>
  bool has(const T& value) const {
    auto hash = std::hash<T>()(value);
    auto position = hash % data.size();

    for (auto it = data.begin() + position; it != data.end(); ++it) {
      auto& optionalValue = *it;
      if (!optionalValue.has_value()) {
        return false;
      }

      if (optionalValue.value() == value) {
        return true;
      }
    }

    return false;
  }

  size_t size() const {
    return data.size();
  }

private:
  std::vector<std::optional<Value>> data;
};
