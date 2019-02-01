#ifndef BRANCH_H
#define BRANCH_H

#include <string>
#include <vector>

#include "ontoloGenius/core/ontoGraphs/Branchs/ValuedNode.h"

/*
This file use CRTP (curiously recurring template pattern)
be really carreful of how you use it
*/

template <typename B>
class BranchData_t
{
public:
  std::vector<B*> childs_;
  std::vector<B*> mothers_;
};

template <typename T>
class Branch_t : public BranchData_t<T>, public ValuedNode
{
public:
  uint8_t family;
  uint8_t nb_mothers_;

  explicit Branch_t(std::string value) : ValuedNode(value), family(0), nb_mothers_(0)
    {};
};

template <typename S>
class BranchSteady_t : public ValuedNodeData, public BranchData_t<S>
{
public:
  BranchSteady_t() {}
};

#endif
