#ifndef MANAGERCLIENT_H
#define MANAGERCLIENT_H

#include "ontoloGenius/utility/clients/ClientBase.h"

class ManagerClient : public ClientBase
{
public:
  explicit ManagerClient(ros::NodeHandle* n) : ClientBase(n, "manage")
  {
  }

  std::vector<std::string> list();
  bool add(const std::string& name);
  bool del(const std::string& name);
  std::vector<std::string> getDifference(const std::string& onto1, const std::string& onto2, const std::string& concept);

private:

};

#endif //MANAGERCLIENT_H
