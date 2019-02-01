#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <map>
#include <mutex>  // For std::unique_lock
#include <shared_mutex>

#include "ontoloGenius/core/ontoGraphs/BranchContainer/BranchContainerMap.h"
#include "ontoloGenius/core/ontoGraphs/BranchContainer/BranchContainerDyn.h"

#include "ontoloGenius/core/ontoGraphs/Branchs/ValuedNode.h"

template <typename B>
class Graph
{
  static_assert(std::is_base_of<ValuedNode,B>::value, "B must be derived from ValuedNode");
public:
  Graph() { language_ = "en"; }
  virtual ~Graph() {}

  void setLanguage(std::string language) noexcept {language_ = language; }
  std::string getLanguage() noexcept {return language_; }

  virtual void close() = 0;

  virtual std::vector<B*> get() = 0;
  virtual B* findBranch(std::string name) noexcept;
  virtual B* findBranchUnsafe(std::string name) noexcept;
  virtual B* create(std::string name) noexcept;

  BranchContainerMap<B> container_;

  std::string language_;

  mutable std::shared_timed_mutex mutex_;
  //use std::lock_guard<std::shared_timed_mutex> lock(Graph<B>::mutex_); to WRITE A DATA
  //use std::shared_lock<std::shared_timed_mutex> lock(Graph<B>::mutex_); to READ A DATA
};

template <typename B>
B* Graph<B>::findBranch(std::string name) noexcept
{
  B* indiv = nullptr;
  std::shared_lock<std::shared_timed_mutex> lock(mutex_);
  indiv = container_.find(name);

  return indiv;
}

template <typename B>
B* Graph<B>::findBranchUnsafe(std::string name) noexcept
{
  B* indiv = nullptr;
  indiv = container_.find(name);

  return indiv;
}

template <typename B>
B* Graph<B>::create(std::string name) noexcept
{
  B* indiv = nullptr;
  {
    std::shared_lock<std::shared_timed_mutex> lock(mutex_);
    indiv = container_.find(name);
  }

  if(indiv == nullptr)
  {
    std::lock_guard<std::shared_timed_mutex> lock(mutex_);
    indiv = new B(name);
    container_.insert(indiv);
  }
  return indiv;
}

#endif
