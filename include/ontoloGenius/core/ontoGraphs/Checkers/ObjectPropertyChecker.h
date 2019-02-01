#ifndef OBJECTPROPERTYCHECKER_H
#define OBJECTPROPERTYCHECKER_H

#include "ontoloGenius/core/ontoGraphs/Graphs/ObjectPropertyGraph.h"
#include "ontoloGenius/core/ontoGraphs/Checkers/ValidityChecker.h"

class ObjectPropertyChecker : public ValidityChecker<ObjectPropertyBranch_t>
{
public:
  explicit ObjectPropertyChecker(ObjectPropertyGraph* graph) : ValidityChecker(graph) {property_graph_ = graph;}
  ~ObjectPropertyChecker() {}

  size_t check();
  void printStatus(){ValidityChecker<ObjectPropertyBranch_t>::printStatus(std::string("object property"), std::string("object properties"), graph_vect_.size());}
private:
  void checkDisjoint();
  void checkCharacteristics();

  ObjectPropertyGraph* property_graph_;

  ObjectPropertyBranch_t* findIntersection(std::unordered_set<ObjectPropertyBranch_t*>& base, std::unordered_set<ObjectPropertyBranch_t*>& comp)
  {
    for (ObjectPropertyBranch_t* it : comp)
    {
      if(base.find(it) != base.end())
        return it;
    }
    return nullptr;
  }
};

#endif
