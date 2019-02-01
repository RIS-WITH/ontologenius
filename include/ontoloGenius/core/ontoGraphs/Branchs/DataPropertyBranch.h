#ifndef DATAPROPERTYBRANCH_T
#define DATAPROPERTYBRANCH_T

#include <string>
#include <vector>

#include "ontoloGenius/core/ontoGraphs/Branchs/PropertyBranch.h"
#include "ontoloGenius/core/ontoGraphs/Branchs/Data.h"

//for branch type usage
class ClassBranch_t;

template <typename T>
class DataPropertyBranchData_t : public PropertyBranchData_t<T>
{
public:
  std::vector<ClassBranch_t*> domains_;
  std::vector<data_t> ranges_;
};

//for template usage
class DataPropertyBranch_t;
class DataPropertySteady_t :  public BranchSteady_t<DataPropertyBranch_t>,
                              public DataPropertyBranchData_t<DataPropertyBranch_t>
{
public:
  DataPropertySteady_t() {}
};

class DataPropertyBranch_t :  public Branch_t<DataPropertyBranch_t>,
                              public DataPropertyBranchData_t<DataPropertyBranch_t>
{
public:
  DataPropertySteady_t steady_;

  explicit DataPropertyBranch_t(std::string value = "") : Branch_t(value) {}

  void setFullSteady() noexcept;
  void setSteady_disjoint(DataPropertyBranch_t* disjoint) noexcept;
  void setSteady_properties(Properties_t properties) noexcept;
  void setSteady_domain(ClassBranch_t* domain) noexcept;
  void setSteady_range(std::string range) noexcept;
  void setSteady_child(DataPropertyBranch_t* child) noexcept;
  void setSteady_mother(DataPropertyBranch_t* mother) noexcept;
  void setSteady_dictionary(std::string lang, std::string word) noexcept;
  void setSteady_dictionary(std::map<std::string, std::vector<std::string>> dictionary) noexcept;
};


#endif
