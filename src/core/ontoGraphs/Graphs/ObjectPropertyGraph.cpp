#include "ontologenius/core/ontoGraphs/Graphs/ObjectPropertyGraph.h"

#include <iostream>

#include "ontologenius/core/ontoGraphs/Graphs/ClassGraph.h"

namespace ontologenius {

ObjectPropertyGraph::ObjectPropertyGraph(ClassGraph* class_graph)
{
  class_graph_ = class_graph;
}

ObjectPropertyGraph::ObjectPropertyGraph(const ObjectPropertyGraph& other, ClassGraph* class_graph)
{
  class_graph_ = class_graph;

  language_ = other.language_;

  for(const auto& root : other.roots_)
  {
    ObjectPropertyBranch_t* prop_branch = new ObjectPropertyBranch_t(root.first);
    roots_[root.first] = prop_branch;
    all_branchs_.push_back(prop_branch);
  }

  for(const auto& branch : other.branchs_)
  {
    ObjectPropertyBranch_t* prop_branch = new ObjectPropertyBranch_t(branch.first);
    branchs_[branch.first] = prop_branch;
    all_branchs_.push_back(prop_branch);
  }

  this->container_.load(all_branchs_);
}

void ObjectPropertyGraph::add(std::string value, ObjectPropertyVectors_t& property_vectors)
{
  std::lock_guard<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);
  /**********************
  ** Mothers
  **********************/
  ObjectPropertyBranch_t* me = nullptr;
  //am I a created mother ?
  amIA(&me, tmp_mothers_, value);

  //am I a created branch ?
  amIA(&me, branchs_, value);

  //am I a created root ?
  amIA(&me, roots_, value);

  //am I created ?
  if(me == nullptr)
    me = new ObjectPropertyBranch_t(value);

  me->nb_mothers_ += property_vectors.mothers_.size();

  //am I a root ?
  if(me->nb_mothers_ == 0)
    roots_[value] = me;
  else
  {
    //for all my mothers
    for(auto& mother : property_vectors.mothers_)
    {
      ObjectPropertyBranch_t* mother_branch = nullptr;
      getInMap(&mother_branch, mother.elem, roots_);
      getInMap(&mother_branch, mother.elem, branchs_);
      getInMap(&mother_branch, mother.elem, tmp_mothers_);
      if(mother_branch == nullptr)
      {
        mother_branch = new struct ObjectPropertyBranch_t(mother.elem);
        tmp_mothers_[mother_branch->value()] = mother_branch;
      }

      conditionalPushBack(mother_branch->childs_, ObjectPropertyElement_t(me, mother.probability, true));
      conditionalPushBack(me->mothers_, ObjectPropertyElement_t(mother_branch, mother.probability));
    }

    //but i am also a branch
    branchs_[me->value()] = me;
  }

  /**********************
  ** Disjoints
  **********************/
  //for all my disjoints
  for(auto& disjoint : property_vectors.disjoints_)
  {
    ObjectPropertyBranch_t* disjoint_branch = nullptr;
    getInMap(&disjoint_branch, disjoint.elem, roots_);
    getInMap(&disjoint_branch, disjoint.elem, branchs_);
    getInMap(&disjoint_branch, disjoint.elem, tmp_mothers_);

    //I create my disjoint
    if(disjoint_branch == nullptr)
    {
      disjoint_branch = new struct ObjectPropertyBranch_t(disjoint.elem);
      tmp_mothers_[disjoint_branch->value()] = disjoint_branch; //I put my disjoint as tmp_mother
    }
    conditionalPushBack(me->disjoints_, ObjectPropertyElement_t(disjoint_branch, disjoint.probability));
    conditionalPushBack(disjoint_branch->disjoints_, ObjectPropertyElement_t(me, disjoint.probability, true));
  }

  /**********************
  ** Inverses
  **********************/
  //for all my inverses
  for(auto& inverse : property_vectors.inverses_)
  {
    ObjectPropertyBranch_t* inverse_branch = nullptr;
    getInMap(&inverse_branch, inverse.elem, roots_);
    getInMap(&inverse_branch, inverse.elem, branchs_);
    getInMap(&inverse_branch, inverse.elem, tmp_mothers_);

    //I create my disjoint
    if(inverse_branch == nullptr)
    {
      inverse_branch = new struct ObjectPropertyBranch_t(inverse.elem);
      tmp_mothers_[inverse_branch->value()] = inverse_branch; //I put my disjoint as tmp_mother
    }
    conditionalPushBack(me->inverses_, ObjectPropertyElement_t(inverse_branch, inverse.probability));
    conditionalPushBack(inverse_branch->inverses_, ObjectPropertyElement_t(me, inverse.probability, true));
  }

  /**********************
  ** Domains
  **********************/
  //for all my domains
  for(auto& domain : property_vectors.domains_)
  {
    ClassBranch_t* domain_branch = nullptr;
    getInMap(&domain_branch, domain.elem, class_graph_->roots_);
    getInMap(&domain_branch, domain.elem, class_graph_->branchs_);
    getInMap(&domain_branch, domain.elem, class_graph_->tmp_mothers_);

    //I create my domain
    if(domain_branch == nullptr)
    {
      ObjectVectors_t empty_vectors;
      class_graph_->add(domain.elem, empty_vectors);
      getInMap(&domain_branch, domain.elem, class_graph_->roots_);
    }
    conditionalPushBack(me->domains_, ClassElement_t(domain_branch, domain.probability));
  }

  /**********************
  ** Ranges
  **********************/
  //for all my ranges
  for(auto& range : property_vectors.ranges_)
  {
    ClassBranch_t* range_branch = nullptr;
    getInMap(&range_branch, range.elem, class_graph_->roots_);
    getInMap(&range_branch, range.elem, class_graph_->branchs_);
    getInMap(&range_branch, range.elem, class_graph_->tmp_mothers_);

    //I create my domain
    if(range_branch == nullptr)
    {
      ObjectVectors_t empty_vectors;
      class_graph_->add(range.elem, empty_vectors);
      getInMap(&range_branch, range.elem, class_graph_->roots_);
    }
    conditionalPushBack(me->ranges_, ClassElement_t(range_branch, range.probability));
  }

  /**********************
  ** Language and properties
  **********************/
  me->properties_ = property_vectors.properties_;
  me->setSteady_dictionary(property_vectors.dictionary_);
  if(me->dictionary_.spoken_.find("en") == me->dictionary_.spoken_.end())
    me->dictionary_.spoken_["en"].push_back(me->value());
  me->setSteady_muted_dictionary(property_vectors.muted_dictionary_);

  /**********************
  ** Chain axiom
  **********************/
  for(size_t chain_i = 0; chain_i < property_vectors.chains_.size(); chain_i++)
  {
    std::vector<ObjectPropertyBranch_t*> chain;
    ObjectPropertyBranch_t* first = nullptr;

    for(size_t i = 0; i < property_vectors.chains_[chain_i].size(); i++)
    {
      ObjectPropertyBranch_t* next = nullptr;

      //is a root my next ?
      getNextChainLink(&next, property_vectors.chains_[chain_i][i], roots_);

      //is a branch my next ?
      getNextChainLink(&next, property_vectors.chains_[chain_i][i], branchs_);

      //is a tmp mother is my next ?
      getNextChainLink(&next, property_vectors.chains_[chain_i][i], tmp_mothers_);

      if(next == nullptr)
      {
        next = new struct ObjectPropertyBranch_t(property_vectors.chains_[chain_i][i]);
        tmp_mothers_[next->value()] = next;
      }

      if(first == nullptr)
        first = next;
      else
        chain.push_back(next);
    }

    chain.push_back(me);
    first->chains_.push_back(chain);
    me->str_chains_.push_back(property_vectors.chains_[chain_i]);
  }

  mitigate(me);
}

void ObjectPropertyGraph::add(std::vector<std::string>& disjoints)
{
  std::lock_guard<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);

  for(size_t disjoints_i = 0; disjoints_i < disjoints.size(); disjoints_i++)
  {
    //I need to find myself
    ObjectPropertyBranch_t* me = nullptr;
    //Am I a root ?
    amIA(&me, roots_, disjoints[disjoints_i], false);

    //Am I a branch ?
    amIA(&me, branchs_, disjoints[disjoints_i], false);

    //Am I a tmp_mother ?
    amIA(&me, tmp_mothers_, disjoints[disjoints_i], false);

    // I don't exist ? so I will be a tmp_mother
    if(me == nullptr)
    {
      me = new struct ObjectPropertyBranch_t(disjoints[disjoints_i]);
      tmp_mothers_[me->value()] = me;
    }

    //for all my disjoints ...
    for(size_t disjoints_j = 0; disjoints_j < disjoints.size(); disjoints_j++)
    {
      //... excepted me
      if(disjoints_i != disjoints_j)
      {
        ObjectPropertyBranch_t* disjoint_branch = nullptr;
        getInMap(&disjoint_branch, disjoints[disjoints_j], roots_);
        getInMap(&disjoint_branch, disjoints[disjoints_j], branchs_);
        getInMap(&disjoint_branch, disjoints[disjoints_j], tmp_mothers_);

        //I create my disjoint
        if(disjoint_branch == nullptr)
        {
          disjoint_branch = new struct ObjectPropertyBranch_t(disjoints[disjoints_j]);
          tmp_mothers_[disjoint_branch->value()] = disjoint_branch; //I put my disjoint as tmp_mother
        }
        conditionalPushBack(me->disjoints_, ObjectPropertyElement_t(disjoint_branch));
        conditionalPushBack(disjoint_branch->disjoints_, ObjectPropertyElement_t(me, 1.0, true));
      }
    }
  }
}


std::unordered_set<std::string> ObjectPropertyGraph::getDisjoint(const std::string& value)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);

  ObjectPropertyBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(auto& disjoint : branch->disjoints_)
      getDown(disjoint.elem, res);

  return res;
}

void ObjectPropertyGraph::getDisjoint(ObjectPropertyBranch_t* branch, std::unordered_set<ObjectPropertyBranch_t*>& res)
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);

  if(branch != nullptr)
    for(auto& disjoint : branch->disjoints_)
      getDownPtr(disjoint.elem, res);
}

std::unordered_set<std::string> ObjectPropertyGraph::getInverse(const std::string& value)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);

  ObjectPropertyBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(auto& inverse : branch->inverses_)
      getDown(inverse.elem, res);

  return res;
}

std::unordered_set<std::string> ObjectPropertyGraph::getDomain(const std::string& value)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);

  ObjectPropertyBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(auto& domain : branch->domains_)
      class_graph_->getDown(domain.elem, res);

  return res;
}

void ObjectPropertyGraph::getDomainPtr(ObjectPropertyBranch_t* branch, std::unordered_set<ClassBranch_t*>& res, size_t depth)
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);

  if(branch != nullptr)
    for(auto& domain : branch->domains_)
      class_graph_->getDownPtr(domain.elem, res, depth);
}

std::unordered_set<std::string> ObjectPropertyGraph::getRange(const std::string& value)
{
  std::unordered_set<std::string> res;
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);

  ObjectPropertyBranch_t* branch = container_.find(value);
  if(branch != nullptr)
    for(auto& range : branch->ranges_)
      class_graph_->getDown(range.elem, res);

  return res;
}

void ObjectPropertyGraph::getRangePtr(ObjectPropertyBranch_t* branch, std::unordered_set<ClassBranch_t*>& res, size_t depth)
{
  std::shared_lock<std::shared_timed_mutex> lock(Graph<ObjectPropertyBranch_t>::mutex_);
  if(branch != nullptr)
    for(auto& range : branch->ranges_)
      class_graph_->getDownPtr(range.elem, res, depth);
}

std::unordered_set<std::string> ObjectPropertyGraph::select(std::unordered_set<std::string>& on, const std::string& selector)
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

bool ObjectPropertyGraph::add(ObjectPropertyBranch_t* prop, std::string& relation, std::string& data)
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
      ObjectPropertyBranch_t* tmp = create(data);
      std::lock_guard<std::shared_timed_mutex> lock(mutex_);
      conditionalPushBack(prop->mothers_, ObjectPropertyElement_t(tmp));
      conditionalPushBack(tmp->childs_, ObjectPropertyElement_t(prop));
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

bool ObjectPropertyGraph::addInvert(ObjectPropertyBranch_t* prop, std::string& relation, std::string& data)
{
  if(relation != "")
  {
    if((relation == "+") || (relation == "isA"))
    {
      ObjectPropertyBranch_t* tmp = create(data);
      std::lock_guard<std::shared_timed_mutex> lock(mutex_);
      conditionalPushBack(tmp->mothers_, ObjectPropertyElement_t(prop));
      conditionalPushBack(prop->childs_, ObjectPropertyElement_t(tmp));
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

bool ObjectPropertyGraph::remove(ObjectPropertyBranch_t* prop, std::string& relation, std::string& data)
{
  (void)prop;
  (void)relation;
  (void)data;
  return false;
}

void ObjectPropertyGraph::deepCopy(const ObjectPropertyGraph& other)
{
  for(const auto& root : other.roots_)
    cpyBranch(root.second, roots_[root.first]);

  for(const auto& branch : other.branchs_)
    cpyBranch(branch.second, branchs_[branch.first]);
}

void ObjectPropertyGraph::cpyBranch(ObjectPropertyBranch_t* old_branch, ObjectPropertyBranch_t* new_branch)
{
  new_branch->family = old_branch->family;
  new_branch->nb_mothers_ = old_branch->nb_mothers_;

  new_branch->nb_updates_ = old_branch->nb_updates_;
  new_branch->updated_ = old_branch->updated_;
  new_branch->flags_ = old_branch->flags_;

  new_branch->dictionary_ = old_branch->dictionary_;
  new_branch->steady_dictionary_ = old_branch->steady_dictionary_;

  for(const auto& child : old_branch->childs_)
    new_branch->childs_.push_back(ObjectPropertyElement_t(child, container_.find(child.elem->value())));

  for(const auto& mother : old_branch->mothers_)
    new_branch->mothers_.push_back(ObjectPropertyElement_t(mother, container_.find(mother.elem->value())));

  for(const auto& range : old_branch->ranges_)
    new_branch->ranges_.push_back(ClassElement_t(range, class_graph_->container_.find(range.elem->value())));

  for(const auto& domain : old_branch->domains_)
    new_branch->domains_.push_back(ClassElement_t(domain, class_graph_->container_.find(domain.elem->value())));

  new_branch->properties_ = old_branch->properties_;

  for(const auto& disjoint : old_branch->disjoints_)
    new_branch->disjoints_.push_back(ObjectPropertyElement_t(disjoint, container_.find(disjoint.elem->value())));

  for(const auto& inverse : old_branch->inverses_)
    new_branch->inverses_.push_back(ObjectPropertyElement_t(inverse, container_.find(inverse.elem->value())));

  for(const auto& chain : old_branch->chains_)
  {
    std::vector<ObjectPropertyBranch_t*> tmp;
    for(const auto& link : chain)
      tmp.push_back(container_.find(link->value()));
    new_branch->chains_.push_back(tmp);
  }

  new_branch->str_chains_ = old_branch->str_chains_;
}

} // namespace ontologenius
