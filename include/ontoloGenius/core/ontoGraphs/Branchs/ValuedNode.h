#ifndef VALUEDNODE_H
#define VALUEDNODE_H

#include <string>
#include <vector>
#include <map>

#include "ontoloGenius/core/ontoGraphs/Branchs/WordTable.h"

class UpdatableNode
{
public:
  unsigned int nb_updates_;
  bool updated_;
  std::map<std::string, std::vector<std::string>> flags_;
  UpdatableNode() {updated_ = true; nb_updates_ = 0; }
};

class ValuedNodeData
{
public:
  std::map<std::string, std::vector<std::string>> dictionary_;
};

class ValuedNode : public UpdatableNode, public ValuedNodeData
{
public:
  explicit ValuedNode(const std::string& value) : UpdatableNode() {index_ = table_.add(value); }

  uint32_t get() noexcept {return index_; }
  std::string value() noexcept {return table_[index_]; }

  static WordTable table_;
private:

  uint32_t index_;
};

#endif
