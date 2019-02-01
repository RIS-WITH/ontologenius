#include "ontoloGenius/core/ontoGraphs/Graphs/DataPropertyGraph.h"
#include <iostream>

#include "ontoloGenius/core/ontoGraphs/Graphs/ClassGraph.h"

void DataPropertyGraph::add(const std::string& value, DataPropertyVectors_t& property_vectors) noexcept
{
  std::lock_guard<std::shared_timed_mutex> lock(Graph<DataPropertyBranch_t>::mutex_);
  /**********************
  ** Mothers
  **********************/
  DataPropertyBranch_t* me = nullptr;
  //am I a created mother ?
  amIA(&me, tmp_mothers_, value);

  //am I a created branch ?
  amIA(&me, branchs_, value);

  //am I a created root ?
  amIA(&me, roots_, value);

  //am I created ?
  if(me == nullptr)
  {
    me = new DataPropertyBranch_t(value);
  }

  me->nb_mothers_ += property_vectors.mothers_.size();

  //am I a root ?
  if(me->nb_mothers_ == 0)
    roots_[me->value()] = me;
  else
  {
    //for all my mothers
    for(size_t mothers_i = 0; mothers_i < property_vectors.mothers_.size(); mothers_i++)
    {
      bool i_find_my_mother = false;

      //is a root my mother ?
      isMyMother(me, property_vectors.mothers_[mothers_i], roots_, i_find_my_mother);

      //is a branch my mother ?
      isMyMother(me, property_vectors.mothers_[mothers_i], branchs_, i_find_my_mother);

      //is a tmp mother is mine ?
      isMyMother(me, property_vectors.mothers_[mothers_i], tmp_mothers_, i_find_my_mother);

      //I create my mother
      if(!i_find_my_mother)
      {
        DataPropertyBranch_t* my_mother = new struct DataPropertyBranch_t(property_vectors.mothers_[mothers_i]);
        my_mother->childs_.push_back(me);
        me->setSteady_mother(my_mother);
        tmp_mothers_[my_mother->value()] = my_mother;
      }
    }

    //but i am also a branch
    branchs_[me->value()] = me;
  }

  /**********************
  ** Disjoints
  **********************/
  //for all my disjoints
  for(size_t disjoints_i = 0; disjoints_i < property_vectors.disjoints_.size(); disjoints_i++)
  {
    bool i_find_my_disjoint = false;

    //is a root my disjoint ?
    isMyDisjoint(me, property_vectors.disjoints_[disjoints_i], roots_, i_find_my_disjoint);

    //is a branch my disjoint ?
    isMyDisjoint(me, property_vectors.disjoints_[disjoints_i], branchs_, i_find_my_disjoint);

    //is a tmp mother is my disjoint ?
    isMyDisjoint(me, property_vectors.disjoints_[disjoints_i], tmp_mothers_, i_find_my_disjoint);

    //I create my disjoint
    if(!i_find_my_disjoint)
    {
      DataPropertyBranch_t* my_disjoint = new struct DataPropertyBranch_t(property_vectors.disjoints_[disjoints_i]);
      me->setSteady_disjoint(my_disjoint);
      my_disjoint->disjoints_.push_back(me);
      tmp_mothers_[my_disjoint->value()] = my_disjoint; //I put my disjoint as tmp_mother
    }
  }

  /**********************
  ** Domains
  **********************/
  //for all my domains
  for(size_t domains_i = 0; domains_i < property_vectors.domains_.size(); domains_i++)
  {
    bool i_find_my_domain = false;

    //is a root my domain ?
    isMyDomain(me, property_vectors.domains_[domains_i], class_graph_->roots_, i_find_my_domain);

    //is a branch my domain ?
    isMyDomain(me, property_vectors.domains_[domains_i], class_graph_->branchs_, i_find_my_domain);

    //is a tmp mother is my domain ?
    isMyDomain(me, property_vectors.domains_[domains_i], class_graph_->tmp_mothers_, i_find_my_domain);

    //I create my domain
    if(!i_find_my_domain)
    {
      ObjectVectors_t empty_vectors;
      class_graph_->add(property_vectors.domains_[domains_i], empty_vectors);
      auto it = class_graph_->roots_.find(property_vectors.domains_[domains_i]);
      if(it != class_graph_->roots_.end())
      {
        me->setSteady_domain(it->second);
        i_find_my_domain = true;
      }
    }
  }

  /**********************
  ** Ranges
  **********************/
  //for all my ranges
  for(size_t ranges_i = 0; ranges_i < property_vectors.ranges_.size(); ranges_i++)
    me->setSteady_range(property_vectors.ranges_[ranges_i]);

  /**********************
  ** Language and properties
  **********************/
  me->setSteady_properties(property_vectors.properties_);
  me->setSteady_dictionary(property_vectors.dictionary_);
  if(me->dictionary_.find("en") == me->dictionary_.end())
    me->dictionary_["en"].push_back(me->value());
}

void DataPropertyGraph::add(std::vector<std::string>& disjoints) noexcept
{
  std::lock_guard<std::shared_timed_mutex> lock(Graph<DataPropertyBranch_t>::mutex_);

  for(size_t disjoints_i = 0; disjoints_i < disjoints.size(); disjoints_i++)
  {
    //I need to find myself
    DataPropertyBranch_t* me = nullptr;
    //Am I a root ?
    amIA(&me, roots_, disjoints[disjoints_i], false);

    //Am I a branch ?
    amIA(&me, branchs_, disjoints[disjoints_i], false);

    //Am I a tmp_mother ?
    amIA(&me, tmp_mothers_, disjoints[disjoints_i], false);

    // I don't exist ? so I will be a tmp_mother
    if(me == nullptr)
    {
      me = new struct DataPropertyBranch_t(disjoints[disjoints_i]);
      tmp_mothers_[me->value()] = me;
    }

    //for all my disjoints ...
    for(size_t disjoints_j = 0; disjoints_j < disjoints.size(); disjoints_j++)
    {
      //... excepted me
      if(disjoints_i != disjoints_j)
      {
        bool i_find_my_disjoint = false;

        //is a root my disjoint ?
        isMyDisjoint(me, disjoints[disjoints_j], roots_, i_find_my_disjoint, false);

        //is a branch my disjoint ?
        isMyDisjoint(me, disjoints[disjoints_j], branchs_, i_find_my_disjoint, false);

        //is a tmp mother is my disjoint ?
        isMyDisjoint(me, disjoints[disjoints_j], tmp_mothers_, i_find_my_disjoint, false);

        //I create my disjoint
        if(!i_find_my_disjoint)
        {
          DataPropertyBranch_t* my_disjoint = new struct DataPropertyBranch_t(disjoints[disjoints_j]);
          me->setSteady_disjoint(my_disjoint);
          tmp_mothers_[my_disjoint->value()] = my_disjoint; //I put my disjoint as tmp_mother
        }
      }
    }
  }
}


std::unordered_set<std::string> DataPropertyGraph::getDisjoint(const std::string& value) noexcept
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<DataPropertyBranch_t>::mutex_);

  DataPropertyBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(unsigned disjoint_i = 0; disjoint_i < branch->disjoints_.size(); disjoint_i++)
      getDown(branch->disjoints_[disjoint_i], res);

  return res;
}

std::unordered_set<std::string> DataPropertyGraph::getDomain(const std::string& value) noexcept
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<DataPropertyBranch_t>::mutex_);

  DataPropertyBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(unsigned domain_i = 0; domain_i < branch->domains_.size(); domain_i++)
      class_graph_->getDown(branch->domains_[domain_i], res);

  return res;
}

void DataPropertyGraph::getDomainPtr(DataPropertyBranch_t* branch, std::unordered_set<ClassBranch_t*>& res, size_t depth) noexcept
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<DataPropertyBranch_t>::mutex_);

  if(branch != nullptr)
    for(unsigned domain_i = 0; domain_i < branch->domains_.size(); domain_i++)
      class_graph_->getDownPtr(branch->domains_[domain_i], res, depth);
}

std::unordered_set<std::string> DataPropertyGraph::getRange(const std::string& value) noexcept
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<DataPropertyBranch_t>::mutex_);

  DataPropertyBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(unsigned range_i = 0; range_i < branch->ranges_.size(); range_i++)
      res.insert(branch->ranges_[range_i].type_);

  return res;
}

std::unordered_set<std::string> DataPropertyGraph::select(std::unordered_set<std::string>& on, const std::string& selector) noexcept
{
  std::unordered_set<std::string> res;
  for(const std::string& it : on)
  {
    std::unordered_set<std::string> tmp = getUp(it);
    if(tmp.find(selector) != tmp.end())
      res.insert(it);
  }
  return res;
}

bool DataPropertyGraph::add(DataPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept
{
  if(relation != "")
  {
    if(relation[0] == '@')
    {
      relation = relation.substr(1);
      std::lock_guard<std::shared_timed_mutex> lock(mutex_);
      prop->setSteady_dictionary(relation, data);
      prop->updated_ = true;
    }
    else if((relation == "+") || (relation == "isA"))
    {
      DataPropertyBranch_t* tmp = create(data);
      std::lock_guard<std::shared_timed_mutex> lock(mutex_);
      prop->setSteady_mother(tmp);
      tmp->setSteady_child(prop);
      prop->updated_ = true;
      tmp->updated_ = true;
    }
    else
      return false;
  }
  else
    return false;
  return true;
}

bool DataPropertyGraph::addInvert(DataPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept
{
  if(relation != "")
  {
    if((relation == "+") || (relation == "isA"))
    {
      DataPropertyBranch_t* tmp = create(data);
      std::lock_guard<std::shared_timed_mutex> lock(mutex_);
      tmp->setSteady_mother(prop);
      prop->setSteady_child(tmp);
      prop->updated_ = true;
      tmp->updated_ = true;
    }
    else
      return false;
  }
  else
    return false;
  return true;
}

bool DataPropertyGraph::remove(DataPropertyBranch_t* prop, std::string& relation, std::string& data) noexcept
{
  return false;
}
