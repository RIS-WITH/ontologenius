#ifndef OBJECTPROPERTYDRAWER_H
#define OBJECTPROPERTYDRAWER_H

#include "ontoloGenius/core/ontoGraphs/Graphs/ObjectPropertyGraph.h"
#include "ontoloGenius/graphical/Drawers/GraphDrawer.h"

class ObjectPropertyDrawer : public GraphDrawer
{
public:
  explicit ObjectPropertyDrawer(ObjectPropertyGraph* graph = nullptr);
  ~ObjectPropertyDrawer() {}

  void setGraph(ObjectPropertyGraph* graph) {graph_ = graph; init(); }

  void putInLayers();

private:
  ObjectPropertyGraph* graph_;
  int createNode(ObjectPropertyBranch_t* branch, node_t* mother);
  void init();
};

#endif /* OBJECTPROPERTYDRAWER_H */
