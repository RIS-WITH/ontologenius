#ifndef ONTOLOGENIUS_ONTOLOGYOWLREADER_H
#define ONTOLOGENIUS_ONTOLOGYOWLREADER_H

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include <tinyxml.h>

#include "ontologenius/core/ontologyIO/OntologyReader.h"

#include "ontologenius/core/ontoGraphs/Graphs/ClassGraph.h"
#include "ontologenius/core/ontoGraphs/Graphs/ObjectPropertyGraph.h"
#include "ontologenius/core/ontoGraphs/Graphs/DataPropertyGraph.h"
#include "ontologenius/core/ontoGraphs/Graphs/IndividualGraph.h"

namespace ontologenius {

class Ontology;

class OntologyOwlReader : public OntologyReader
{
public:
  OntologyOwlReader(ClassGraph* class_graph, ObjectPropertyGraph* object_property_graph, DataPropertyGraph* data_property_graph, IndividualGraph* individual_graph) :
                    OntologyReader(class_graph, object_property_graph, data_property_graph, individual_graph) {}
  explicit OntologyOwlReader(Ontology& onto) : OntologyReader(onto) {}
  ~OntologyOwlReader() {}

  int readFromUri(std::string content, const std::string& uri, bool individual = false);
  int readFromFile(const std::string& fileName, bool individual = false);

  std::vector<std::string> getImportsFromRaw(std::string content);
  std::vector<std::string> getImportsFromFile(const std::string& file_name);

  void setDisplay(bool display) { display_ = display; }
  void displayIndividualRules();
  bool empty() {return (nb_loaded_elem_ == 0); }

private:
  int read(TiXmlElement* rdf, const std::string& name);
  int readIndividual(TiXmlElement* rdf, const std::string& name);

  void readClass(TiXmlElement* elem);
  void readIndividual(TiXmlElement* elem);
  void readDescription(TiXmlElement* elem);
  void readIndividualDescription(TiXmlElement* elem);
  void readObjectProperty(TiXmlElement* elem);
  void readDataProperty(TiXmlElement* elem);
  void readAnnotationProperty(TiXmlElement* elem);
  void readCollection(std::vector<std::string>& vect, TiXmlElement* elem, const std::string& symbol, size_t level = 1);
  std::string readSomeValuesFrom(TiXmlElement* elem);

  inline void push(std::vector<std::string>& vect, TiXmlElement* subElem, const std::string& symbole = "", const std::string& attribute = "rdf:resource");
  inline void push(std::vector<std::string>& vect, const std::string& elem, const std::string& symbole = "");
  inline void push(std::vector<Single_t<std::string>>& vect, TiXmlElement* subElem, float probability, const std::string& symbole = "", const std::string& attribute = "rdf:resource");
  inline void push(std::vector<bool>& vect, bool elem, const std::string& symbole = "");
  void push(Properties_t& properties, TiXmlElement* subElem, const std::string& symbole = "", const std::string& attribute = "rdf:resource");
  void pushLang(std::map<std::string, std::vector<std::string>>& dictionary, TiXmlElement* subElem);
  inline std::string getName(const std::string& uri);
  inline float getProbability(TiXmlElement* elem);
  inline std::string getAttribute(TiXmlElement* elem, const std::string& attribute);
  inline bool testAttribute(TiXmlElement* subElem, const std::string& attribute);

  std::string toString(TiXmlElement* subElem, std::string attribute = "rdf:resource")
  {
    const char* subAttr;
    subAttr = subElem->Attribute(attribute.c_str());
    if(subAttr != NULL)
      return getName(std::string(subAttr));
    return "";
  }

  void removeDocType(std::string& txt);
};

void OntologyOwlReader::push(std::vector<std::string>& vect, TiXmlElement* subElem, const std::string& symbole, const std::string& attribute)
{
  std::string data = getAttribute(subElem, attribute);
  if(data != "")
  {
    vect.push_back(data);
    if(symbole != "" && display_)
      std::cout << "│   │   ├── " << symbole << data << std::endl;
  }
}

void OntologyOwlReader::push(std::vector<Single_t<std::string>>& vect, TiXmlElement* subElem, float probability, const std::string& symbole, const std::string& attribute)
{
  std::string data = getAttribute(subElem, attribute);
  if(data != "")
  {
    vect.push_back(Single_t<std::string>(data, probability));
    if(symbole != "" && display_)
      std::cout << "│   │   ├── " << symbole << data << std::endl;
  }
}

std::string OntologyOwlReader::getName(const std::string& uri)
{
  size_t pos = uri.find("#");
  std::string result = uri.substr(pos+1);
  return result;
}

float OntologyOwlReader::getProbability(TiXmlElement* elem)
{
  float proba = 1.0;

  const char* subAttr;
  subAttr = elem->Attribute("onto:probability");
  if(subAttr != NULL)
    proba = std::stof(std::string(subAttr));

  return proba;
}

inline std::string OntologyOwlReader::getAttribute(TiXmlElement* elem, const std::string& attribute)
{
  const char* subAttr;
  subAttr = elem->Attribute(attribute.c_str());
  if(subAttr != NULL)
    return getName(std::string(subAttr));
  else
    return "";
}

bool OntologyOwlReader::testAttribute(TiXmlElement* subElem, const std::string& attribute)
{
  const char* subAttr;
  subAttr = subElem->Attribute(attribute.c_str());
  if(subAttr != NULL)
    return true;
  else
    return false;
}

} // namespace ontologenius

#endif // ONTOLOGENIUS_ONTOLOGYOWLREADER_H
