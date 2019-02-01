#ifndef WORDTABLE_H
#define WORDTABLE_H

#include <vector>
#include <string>
#include <unordered_set>
#include <cstdint>

class WordTable
{
public:
  WordTable() {}
  ~WordTable() {}

  inline uint32_t add(const std::string& value) noexcept
  {
    table_.push_back(value);
    return table_.size() - 1;
  }

  inline std::string& get(uint32_t index) noexcept
  {
    return table_[index];
  }

  inline std::string& operator[] (uint32_t index) noexcept
  {
    return table_[index];
  }

  inline const std::string& operator[] (uint32_t index) const noexcept
  {
    return table_[index];
  }

  inline void index2string(std::unordered_set<std::string>& res, std::unordered_set<uint32_t>& base) noexcept
  {
    for(uint32_t i : base)
      res.insert(table_[i]);
  }
private:
  std::vector<std::string> table_;
};

#endif
