#include "ontologenius/core/ontoGraphs/Checkers/IndividualChecker.h"

#include <algorithm>

#include "ontologenius/core/ontoGraphs/Graphs/ClassGraph.h"
#include "ontologenius/core/ontoGraphs/Graphs/ObjectPropertyGraph.h"
#include "ontologenius/core/ontoGraphs/Graphs/DataPropertyGraph.h"

namespace ontologenius {

size_t IndividualChecker::check()
{
  graph_size = graph_vect_.size();
  checkSame();

  checkReflexive();

  checkObectPropertyDomain();
  checkObectPropertyRange();

  checkDataPropertyDomain();
  checkDataPropertyRange();

  checkAssymetric();

  is_analysed = true;
  printStatus();

  return getErrors();
}

void IndividualChecker::checkSame()
{
  for(size_t i = 0; i < graph_size; i++)
  {
    std::unordered_set<std::string> same = individual_graph_->getSame(graph_vect_[i]->value());
    std::unordered_set<std::string> distinct;

    for (std::string it : same)
    {
      std::unordered_set<std::string> tmp = individual_graph_->getDistincts(it);
      distinct.insert(tmp.begin(), tmp.end());
    }

    std::string intersection = findIntersection(same, distinct);
    if(intersection != "")
      print_error("'" + graph_vect_[i]->value() + "' can't be same and distinct with '" + intersection + "'");
  }
}

void IndividualChecker::checkReflexive()
{
  std::shared_lock<std::shared_timed_mutex> lock(individual_graph_->mutex_);
  for(size_t i = 0; i < graph_size; i++)
  {
    for(IndivObjectRelationElement_t& object_relation : graph_vect_[i]->object_relations_)
    {
      if(object_relation.first->properties_.reflexive_property_)
      {
        if(graph_vect_[i]->get() != object_relation.second->get())
          print_error("'" + object_relation.first->value() + "' is reflexive so can't be from '" + graph_vect_[i]->value() + "' to '" + object_relation.second->value() + "'");
      }
      else if(object_relation.first->properties_.irreflexive_property_)
      {
        if(graph_vect_[i]->get() == object_relation.second->get())
          print_error("'" + object_relation.first->value() + "' is irreflexive so can't be from '" + graph_vect_[i]->value() + "' to '" + object_relation.second->value() + "'");
      }
    }
  }
}

void IndividualChecker::checkObectPropertyDomain()
{
  for(size_t i = 0; i < graph_size; i++)
  {
    std::unordered_set<ClassBranch_t*> up;
    individual_graph_->getUpPtr(graph_vect_[i], up);

    std::shared_lock<std::shared_timed_mutex> lock(individual_graph_->mutex_);

    for(IndivObjectRelationElement_t& object_relation : graph_vect_[i]->object_relations_)
    {
      std::unordered_set<ObjectPropertyBranch_t*> prop_up;
      individual_graph_->object_property_graph_->getUpPtr(object_relation.first, prop_up);
      std::unordered_set<ClassBranch_t*> domain;
      for(auto prop : prop_up)
        individual_graph_->object_property_graph_->getDomainPtr(prop, domain);

      if(domain.size() != 0)
      {
        ClassBranch_t* intersection = findIntersection(up, domain);
        if(intersection == nullptr)
        {
          std::unordered_set<ClassBranch_t*> disjoints;
          for(auto dom : domain)
            individual_graph_->class_graph_->getDisjoint(dom, disjoints);
          intersection = findIntersection(up, disjoints);

          if(intersection == nullptr)
          {
            graph_vect_[i]->flags_["domain"].push_back(object_relation.first->value());
            print_warning("'" + graph_vect_[i]->value() + "' is not in domain of '" + object_relation.first->value() + "'");
          }
          else
            print_error("'" + graph_vect_[i]->value() + "' can not be in domain of '" + object_relation.first->value() + "'");
        }
      }
    }
  }
}

void IndividualChecker::checkObectPropertyRange()
{
  for(size_t i = 0; i < graph_size; i++)
  {
    for(IndivObjectRelationElement_t& object_relation : graph_vect_[i]->object_relations_)
    {
      std::unordered_set<ClassBranch_t*> up;
      individual_graph_->getUpPtr(object_relation.second, up);

      std::shared_lock<std::shared_timed_mutex> lock(individual_graph_->mutex_);
      std::unordered_set<ObjectPropertyBranch_t*> prop_up;
      individual_graph_->object_property_graph_->getUpPtr(object_relation.first, prop_up);
      std::unordered_set<ClassBranch_t*> range;
      for(auto prop : prop_up)
        individual_graph_->object_property_graph_->getRangePtr(prop, range);

      if(range.size() != 0)
      {
        ClassBranch_t* intersection = findIntersection(up, range);
        if(intersection == nullptr)
        {
          std::unordered_set<ClassBranch_t*> disjoints;
          for(auto ran : range)
            individual_graph_->class_graph_->getDisjoint(ran, disjoints);
          intersection = findIntersection(up, disjoints);

          if(intersection == nullptr)
          {
            graph_vect_[i]->flags_["range"].push_back(object_relation.first->value());
            print_warning("'" + object_relation.second->value() + "' is not in range of '" + object_relation.first->value() + "'");
          }
          else
            print_error("'" + object_relation.second->value() + "' can not be in range of '" + object_relation.first->value() + "'");
        }
      }
    }
  }
}

void IndividualChecker::checkDataPropertyDomain()
{
  for(size_t i = 0; i < graph_size; i++)
  {
    std::unordered_set<ClassBranch_t*> up;
    individual_graph_->getUpPtr(graph_vect_[i], up);

    std::shared_lock<std::shared_timed_mutex> lock(individual_graph_->mutex_);

    for(IndivDataRelationElement_t& relation : graph_vect_[i]->data_relations_)
    {
      std::unordered_set<DataPropertyBranch_t*> prop_up;
      individual_graph_->data_property_graph_->getUpPtr(relation.first, prop_up);
      std::unordered_set<ClassBranch_t*> domain;
      for(auto prop : prop_up)
        individual_graph_->data_property_graph_->getDomainPtr(prop, domain);

      if(domain.size() != 0)
      {
        ClassBranch_t* intersection = findIntersection(up, domain);
        if(intersection == nullptr)
        {
          std::unordered_set<ClassBranch_t*> disjoints;
          for(auto dom : domain)
            individual_graph_->class_graph_->getDisjoint(dom, disjoints);
          intersection = findIntersection(up, disjoints);

          if(intersection == nullptr)
          {
            graph_vect_[i]->flags_["range"].push_back(relation.first->value());
            print_warning("'" + graph_vect_[i]->value() + "' is not in domain of '" + relation.first->value() + "'");
          }
          else
            print_error("'" + graph_vect_[i]->value() + "' can not be in domain of '" + relation.first->value() + "'");
        }
      }
    }
  }
}

void IndividualChecker::checkDataPropertyRange()
{
  std::shared_lock<std::shared_timed_mutex> lock(individual_graph_->mutex_);
  for(size_t i = 0; i < graph_size; i++)
  {
    for(IndivDataRelationElement_t& relation : graph_vect_[i]->data_relations_)
    {
      std::unordered_set<std::string> range = individual_graph_->data_property_graph_->getRange(relation.first->value());
      if(range.size() != 0)
      {
        std::unordered_set<std::string>::iterator intersection = std::find(range.begin(), range.end(), relation.second.type_);
        if(intersection == range.end())
          print_error("'" + relation.second.type_ + "' is not in range of '" + relation.first->value() + "'");
      }
    }
  }
}

void IndividualChecker::checkAssymetric()
{
  std::shared_lock<std::shared_timed_mutex> lock(individual_graph_->mutex_);
  for(size_t i = 0; i < graph_size; i++)
  {
    for(IndivObjectRelationElement_t& object_relation : graph_vect_[i]->object_relations_)
    {
      if(object_relation.first->properties_.antisymetric_property_)
        if(symetricExist(graph_vect_[i], object_relation.first, object_relation.second))
          print_error("'" + object_relation.first->value() + "' is antisymetric so can't be from '" + graph_vect_[i]->value() + "' to '" + object_relation.second->value() + "' and inverse");
    }
  }
}

bool IndividualChecker::symetricExist(IndividualBranch_t* indiv_on, ObjectPropertyBranch_t* sym_prop, IndividualBranch_t* sym_indiv)
{
  std::shared_lock<std::shared_timed_mutex> lock(individual_graph_->mutex_);
  for(IndivObjectRelationElement_t& object_relation : sym_indiv->object_relations_)
  {
    if(object_relation.first->get() == sym_prop->get())
      if(object_relation.second->get() == indiv_on->get())
        return true;
  }
  return false;
}

} // namespace ontologenius
