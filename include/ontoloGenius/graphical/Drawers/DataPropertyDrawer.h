#ifndef DATAPROPERTYDRAWER_H
#define DATAPROPERTYDRAWER_H

#include "ontoloGenius/core/ontoGraphs/Graphs/DataPropertyGraph.h"
#include "ontoloGenius/graphical/Drawers/GraphDrawer.h"

class DataPropertyDrawer : public GraphDrawer
{
public:
  explicit DataPropertyDrawer(DataPropertyGraph* graph = nullptr);
  ~DataPropertyDrawer() {}

  void setGraph(DataPropertyGraph* graph) {graph_ = graph; init(); }

  void putInLayers();

private:
  DataPropertyGraph* graph_;
  int createNode(DataPropertyBranch_t* branch, node_t* mother);
  void init();
};

#endif /* DATAPROPERTYDRAWER_H */
