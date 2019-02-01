#ifndef INDIVIDUALBRANCH_H
#define INDIVIDUALBRANCH_H

#include <string>
#include <vector>

#include "ontoloGenius/core/ontoGraphs/Branchs/ValuedNode.h"

#include "ontoloGenius/core/ontoGraphs/Branchs/ClassBranch.h"
#include "ontoloGenius/core/ontoGraphs/Branchs/ObjectPropertyBranch.h"
#include "ontoloGenius/core/ontoGraphs/Branchs/DataPropertyBranch.h"

class Triplet;

template <typename T>
class IndividualBranchData_t
{
public:
  std::vector<ClassBranch_t*> is_a_;

  std::vector<ObjectPropertyBranch_t*> object_properties_name_;
  std::vector<T*> object_properties_on_;
  std::vector<bool> object_properties_deduced_;
  std::vector<Triplet> object_properties_has_induced_;

  std::vector<DataPropertyBranch_t*> data_properties_name_;
  std::vector<data_t> data_properties_data_;
  std::vector<bool> data_properties_deduced_;

  std::vector<T*> same_as_;
  std::vector<T*> distinct_;
};

//for template usage
class IndividualBranch_t;
class IndividualSteady_t : public ValuedNodeData, public IndividualBranchData_t<IndividualBranch_t>
{
public:
  IndividualSteady_t() {}
};

class IndividualBranch_t : public ValuedNode, public IndividualBranchData_t<IndividualBranch_t>
{
public:
  bool mark;
  IndividualSteady_t steady_;

  explicit IndividualBranch_t(std::string value = "") : ValuedNode(value) {mark = false; }

  void setFullSteady() noexcept;
  void setSteady_is_a(ClassBranch_t* is_a) noexcept;
  void setSteady_object_properties_name(ObjectPropertyBranch_t* object_properties_name) noexcept;
  void setSteady_object_properties_on(IndividualBranch_t* object_properties_on) noexcept;
  void setSteady_data_properties_name(DataPropertyBranch_t* data_properties_name) noexcept;
  void setSteady_data_properties_data(data_t data_properties_data) noexcept;
  void setSteady_same_as(IndividualBranch_t* same_as) noexcept;
  void setSteady_distinct(IndividualBranch_t* distinct) noexcept;
  void setSteady_dictionary(std::string lang, std::string word) noexcept;
  void setSteady_dictionary(std::map<std::string, std::vector<std::string>> dictionary) noexcept;
};

class Triplet
{
public:
  void push(IndividualBranch_t* from,
            ObjectPropertyBranch_t* prop,
            IndividualBranch_t* on) noexcept
  {
    from_.push_back(from);
    prop_.push_back(prop);
    on_.push_back(on);
  }
  bool exist(IndividualBranch_t* from,
            ObjectPropertyBranch_t* prop,
            IndividualBranch_t* on) noexcept
  {
    for(auto from_i : from_)
      if(from_i == from)
        for(auto prop_i : prop_)
          if(prop_i == prop)
            for(auto on_i : on_)
              if(on_i == on)
                return true;
    return false;
  }
  std::vector<IndividualBranch_t*> from_;
  std::vector<ObjectPropertyBranch_t*> prop_;
  std::vector<IndividualBranch_t*> on_;

};

#endif
