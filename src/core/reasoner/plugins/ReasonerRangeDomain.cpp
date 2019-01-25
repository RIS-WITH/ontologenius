#include "ontoloGenius/core/reasoner/plugins/ReasonerRangeDomain.h"
#include <pluginlib/class_list_macros.h>

void ReasonerRangeDomain::postReason()
{
  std::lock_guard<std::shared_timed_mutex> lock(ontology_->individual_graph_.mutex_);
  std::vector<IndividualBranch_t*> indiv = ontology_->individual_graph_.get();
  size_t indiv_size = indiv.size();

  std::map<std::string, std::vector<std::string>>::iterator it_range;
  std::map<std::string, std::vector<std::string>>::iterator it_domain;

  for(size_t indiv_i = 0; indiv_i < indiv_size; indiv_i++)
    if(indiv[indiv_i]->updated_ == true)
    {
      it_range = indiv[indiv_i]->flags_.find("range");
      if(it_range != indiv[indiv_i]->flags_.end())
      {
        for(std::string prop : it_range->second)
          deduceRange(indiv[indiv_i], prop);
      }

      it_domain = indiv[indiv_i]->flags_.find("domain");
      if(it_domain != indiv[indiv_i]->flags_.end())
      {
        for(std::string prop : it_domain->second)
          deduceDomain(indiv[indiv_i], prop);
      }
    }
}

void ReasonerRangeDomain::deduceRange(IndividualBranch_t* branch, std::string& prop)
{
  std::cout << "deduceRange on " << branch->value() << std::endl;
  for(size_t i = 0; i < branch->object_properties_name_.size(); i++)
    if(branch->object_properties_name_[i]->value() == prop)
      deduceObjRange(branch, i);
}

void ReasonerRangeDomain::deduceDomain(IndividualBranch_t* branch, std::string& prop)
{
  std::cout << "deduceDomain on " << branch->value() << std::endl;
  for(size_t i = 0; i < branch->object_properties_name_.size(); i++)
    if(branch->object_properties_name_[i]->value() == prop)
      deduceObjDomain(branch, i);

  for(size_t i = 0; i < branch->data_properties_name_.size(); i++)
    if(branch->data_properties_name_[i]->value() == prop)
      deduceDatDomain(branch, i);
}

void ReasonerRangeDomain::deduceObjRange(IndividualBranch_t* branch, size_t index)
{
  std::unordered_set<ClassBranch_t*> ranges;
  std::unordered_set<ObjectPropertyBranch_t*> props;
  props.insert(branch->object_properties_name_[index]);
  while(ranges.size() == 0)
  {
    for(auto prop : props)
      ontology_->object_property_graph_.getRangePtr(prop, ranges);

    if(ranges.size() == 0)
    {
      std::unordered_set<ObjectPropertyBranch_t*> prop_up;
      for(auto prop : props)
      {
        ontology_->object_property_graph_.getUpPtr(prop, prop_up, 1);
        prop_up.erase(prop);
      }

      if(prop_up.size() == 0)
        break;
      else
        props = prop_up;
    }
  }

  for(auto range : ranges)
  {
    std::unordered_set<ClassBranch_t*> up;
    ontology_->individual_graph_.getUpPtr(branch->object_properties_on_[index], up);
    if(up.find(range) == up.end())
    {
      branch->object_properties_on_[index]->is_a_.push_back(range);
      range->individual_childs_.push_back(branch->object_properties_on_[index]);

      branch->object_properties_on_[index]->updated_ = true;
      range->updated_ = true;
    }
  }
}

void ReasonerRangeDomain::deduceObjDomain(IndividualBranch_t* branch, size_t index)
{
  std::cout << "deduceObjDomain" << std::endl;
}

void ReasonerRangeDomain::deduceDatDomain(IndividualBranch_t* branch, size_t index)
{
  std::cout << "deduceDatDomain" << std::endl;
}

std::string ReasonerRangeDomain::getName()
{
  return "reasoner range and domain";
}

std::string ReasonerRangeDomain::getDesciption()
{
  return "This is an reasoner to deduce new inheritances based on range and domain of properties.";
}

PLUGINLIB_EXPORT_CLASS(ReasonerRangeDomain, ReasonerInterface)