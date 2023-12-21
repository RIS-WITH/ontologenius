#ifndef ONTOLOGENIUS_RELATIONSWITHINDUCTIONS_H
#define ONTOLOGENIUS_RELATIONSWITHINDUCTIONS_H

#include <vector>

#include "ontologenius/core/ontoGraphs/Branchs/Triplet.h"

namespace ontologenius
{

class IndividualBranch_t;
class ObjectPropertyBranch_t;
class ClassBranch_t;

typedef Triplet_t<IndividualBranch_t, ObjectPropertyBranch_t, IndividualBranch_t> ObjectRelationTriplet_t;
typedef Triplets<IndividualBranch_t, ObjectPropertyBranch_t, IndividualBranch_t> ObjectRelationTriplets;
typedef Triplet_t<IndividualBranch_t, void, ClassBranch_t> InheritedRelationTriplet_t;
typedef Triplets<IndividualBranch_t, void, ClassBranch_t> InheritedRelationTriplets;

template <typename T>
class RelationsWithInductions
{
public:
	std::vector<T> relations;
	std::vector<ObjectRelationTriplets*> has_induced_object_relations;
	std::vector<InheritedRelationTriplets*> has_induced_inheritance_relations;

	size_t size() { return relations.size(); }
	T& operator[](size_t index) { return relations[index]; }

	size_t push_back(T& relation)
	{
		relations.emplace_back(relation);
		has_induced_object_relations.emplace_back(new ObjectRelationTriplets);
		has_induced_inheritance_relations.emplace_back(new InheritedRelationTriplets);
		return relations.size() -1;
	}

	template< class... Args >
	T& emplace_back(Args&&... args)
	{
		has_induced_object_relations.emplace_back(new ObjectRelationTriplets);
		has_induced_inheritance_relations.emplace_back(new InheritedRelationTriplets);
		return relations.emplace_back(std::forward<Args>(args)...);
	}

	void erase(size_t index)
	{
		relations.erase(relations.begin() + index);
		delete has_induced_object_relations.at(index);
		has_induced_object_relations.erase(has_induced_object_relations.begin() + index);
		delete has_induced_inheritance_relations.at(index);
		has_induced_inheritance_relations.erase(has_induced_inheritance_relations.begin() + index);
	}

	typename std::vector<T>::iterator begin() { return relations.begin(); }
	typename std::vector<T>::iterator end() { return relations.end(); }
	typename std::vector<T>::const_iterator cbegin() const { return relations.cbegin(); }
	typename std::vector<T>::const_iterator cend() const { return relations.cend(); } 
};

} // namespace ontologenius

#endif // ONTOLOGENIUS_RELATIONSWITHINDUCTIONS_H