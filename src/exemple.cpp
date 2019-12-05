#include <ros/ros.h>

#include "ontologenius/core/ontoGraphs/Ontology.h"
#include "ontologenius/graphical/Drawers/OntologyDrawer.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "ontologeniusExemple");

{
  ontologenius::Ontology onto;

  //modify ontoGraph.h to have colors (link function in close one)

  onto.readFromUri("https://raw.githubusercontent.com/sarthou/ontologenius/master/files/attribute.owl");
  onto.readFromUri("https://raw.githubusercontent.com/sarthou/ontologenius/master/files/measure.owl");
  onto.readFromUri("https://raw.githubusercontent.com/sarthou/ontologenius/master/files/positionProperty.owl");
  onto.readFromUri("https://raw.githubusercontent.com/sarthou/ontologenius/master/files/property.owl");
  onto.readFromUri("https://raw.githubusercontent.com/sarthou/ontologenius/master/files/testIndividuals.owl");

  onto.close();

  ontologenius::OntologyDrawer drawer(&onto);
  drawer.draw("ontologenius_exemple");
}

  ROS_DEBUG("Drawing done");

  return 0;
}
