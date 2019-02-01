#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <stdint.h>

#include "ontoloGenius/core/ontoGraphs/Graphs/OntoGraph.h"

#include "ontoloGenius/core/ontoGraphs/Branchs/DataPropertyBranch.h"
#include "ontoloGenius/core/ontoGraphs/Branchs/ClassBranch.h"

#ifndef DATAPROPERTYGRAPH_H
#define DATAPROPERTYGRAPH_H

struct DataPropertyVectors_t
{
   std::vector<std::string> mothers_;
   std::vector<std::string> disjoints_;
   std::vector<std::string> domains_;
   std::vector<std::string> ranges_;
   Properties_t properties_;
   std::map<std::string, std::vector<std::string>> dictionary_;
};

//for friend
class DataPropertyDrawer;
class IndividualGraph;

//for graphs usage
class ClassGraph;

class DataPropertyGraph : public OntoGraph<DataPropertyBranch_t>
{
  friend DataPropertyDrawer;
  friend IndividualGraph;
  friend ClassGraph;
public:
  explicit DataPropertyGraph(ClassGraph* class_graph) {class_graph_ = class_graph; }
  ~DataPropertyGraph() {}

  void linkGraph(ClassGraph* class_graph) noexcept {class_graph_ = class_graph; }

  void add(const std::string& value, DataPropertyVectors_t& property_vectors) noexcept;
  void add(std::vector<std::string>& disjoints) noexcept;

  std::unordered_set<std::string> getDisjoint(const std::string& value) noexcept;
  std::unordered_set<std::string> getDomain(const std::string& value) noexcept;
  void getDomainPtr(DataPropertyBranch_t* branch, std::unordered_set<ClassBranch_t*>& res, size_t depth = -1) noexcept;
  std::unordered_set<std::string> getRange(const std::string& value) noexcept;
  std::unordered_set<std::string> select(std::unordered_set<std::string>& on, const std::string& selector) noexcept;

  bool add(DataPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept;
  bool addInvert(DataPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept;
  bool remove(DataPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept;

private:
  ClassGraph* class_graph_;

  void isMyDisjoint(DataPropertyBranch_t* me, const std::string& disjoint, std::map<std::string, DataPropertyBranch_t*>& vect, bool& find, bool all = true) noexcept
  {
    if(find)
      return;

    auto it = vect.find(disjoint);
    if(it != vect.end())
    {
      me->setSteady_disjoint(it->second);
      if(all)
        it->second->disjoints_.push_back(me);
      find = true;
    }
  }

  void isMyDomain(DataPropertyBranch_t* me, const std::string& domain, std::map<std::string, ClassBranch_t*>& vect, bool& find) noexcept
  {
    if(find)
      return;

    auto it = vect.find(domain);
    if(it != vect.end())
    {
      me->setSteady_domain(it->second);
      find = true;
    }
  }
};

#endif /* DATAPROPERTYGRAPH_H */
