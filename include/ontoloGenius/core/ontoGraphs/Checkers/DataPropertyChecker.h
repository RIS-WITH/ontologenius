#ifndef DATAPROPERTYCHECKER_H
#define DATAPROPERTYCHECKER_H

#include "ontoloGenius/core/ontoGraphs/Graphs/DataPropertyGraph.h"
#include "ontoloGenius/core/ontoGraphs/Checkers/ValidityChecker.h"

class DataPropertyChecker : public ValidityChecker<DataPropertyBranch_t>
{
public:
  explicit DataPropertyChecker(DataPropertyGraph* graph) : ValidityChecker(graph) {property_graph_ = graph;}
  ~DataPropertyChecker() {}

  size_t check() noexcept;
  void printStatus() noexcept {ValidityChecker<DataPropertyBranch_t>::printStatus(std::string("data property"), std::string("data properties"), graph_vect_.size());}
private:
  void checkDisjoint() noexcept;

  DataPropertyGraph* property_graph_;
};

#endif
