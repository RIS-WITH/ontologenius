#include "ontoloGenius/graphical/Drawers/ObjectPropertyDrawer.h"

#include <iostream>

ObjectPropertyDrawer::ObjectPropertyDrawer(ObjectPropertyGraph* graph)
{
  graph_ = graph;
  init();
}

void ObjectPropertyDrawer::putInLayers()
{
  if((graph_ != nullptr) && (graph_->roots_.size() != 0))
  {
    //init markers
    for(uint32_t i = 0; i < branchs_nodes.size(); i++)
      branchs_nodes[i]->marker = false;

    for(uint32_t i = 0; i < roots_nodes.size(); i++)
    {
      roots_nodes[i]->marker = false;
      roots_nodes[i]->pos = i;
    }

    int layer = 0;

    while(!testEnd())
    {
      layer_nodes.push_back(std::vector<node_t*>());
      putLayer(layer);
      layer++;
    }

    if(layer_nodes.size() > 0)
      layer_nodes.pop_back();
  }
}

int ObjectPropertyDrawer::createNode(ObjectPropertyBranch_t* branch, node_t* mother)
{
  int family = branch->family;
  if(!exist(branch->value()))
  {
    node_t* node = new node_t(branch->value());
    branchs_nodes.push_back(node);
    node->prev.push_back(mother);
    node->family = branch->family;
    for(uint32_t i = 0; i < branch->childs_.size(); i++)
      family += createNode(branch->childs_[i], node);

    family = family / (branch->childs_.size() + 1);
  }
  else
  {
    for(uint32_t i = 0; i < branchs_nodes.size(); i++)
      if(branchs_nodes[i]->value == branch->value())
        branchs_nodes[i]->prev.push_back(mother);
  }
  return family;
}

void ObjectPropertyDrawer::init()
{
  std::vector<node_t*> single;
  std::vector<node_t*> couple;
  if(graph_ != nullptr)
  {
    for(auto it : graph_->roots_)
    {
      node_t* node = new node_t(it.second->value(), 0);
      //roots_nodes.push_back(node);
      node->family = it.second->family;
      int family = it.second->family;

      for(uint32_t branch = 0; branch < it.second->childs_.size(); branch++)
        family += createNode(it.second->childs_[branch], node);

      family = family / (it.second->childs_.size() + 1);
      if(family == node->family)
        single.push_back(node);
      else
        couple.push_back(node);
    }

    uint32_t middle = single.size()/2;
    for(uint32_t i = 0; i < middle; i++)
      roots_nodes.push_back(single[i]);

    for(uint32_t i = 0; i < couple.size(); i++)
      roots_nodes.push_back(couple[i]);

    for(uint32_t i = middle; i < single.size(); i++)
      roots_nodes.push_back(single[i]);
  }
}
