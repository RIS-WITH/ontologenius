#include "ros/ros.h"
#include <thread>

#include "ontoloGenius/RosInterface.h"
#include "ontoloGenius/core/utility/error_code.h"

void removeUselessSpace(std::string& text)
{
  while((text[0] == ' ') && (text.size() != 0))
    text.erase(0,1);

  while((text[text.size() - 1] == ' ') && (text.size() != 0))
    text.erase(text.size() - 1,1);
}

ros::NodeHandle* n_;
std::map<std::string, RosInterface*> interfaces_;
std::map<std::string, std::atomic<bool>*> interfaces_run_;
std::map<std::string, std::thread> interfaces_threads_;

std::string language;
std::string intern_file = "none";
std::vector<std::string> files;

bool managerHandle(ontologenius::OntologeniusService::Request &req,
                   ontologenius::OntologeniusService::Response &res)
{
  res.code = 0;

  removeUselessSpace(req.action);
  removeUselessSpace(req.param);

  if(req.action == "add")
  {
    auto it = interfaces_.find(req.param);
    if(it != interfaces_.end())
      res.code = NO_EFFECT;
    else
    {
      RosInterface* tmp = new RosInterface(n_, req.param);
      interfaces_[req.param] = tmp;
      tmp->init(language, intern_file, files);
      interfaces_run_[req.param] = tmp->getAtomicRun();
      std::thread th(&RosInterface::run, tmp);
      interfaces_threads_[req.param] = std::move(th);

      std::cout << req.param << " STARTED" << std::endl;
    }
  }
  else if(req.action == "delete")
  {
    auto it = interfaces_.find(req.param);
    if(it == interfaces_.end())
      res.code = NO_EFFECT;
    else
    {
      *(interfaces_run_[req.param]) = false;
      interfaces_threads_[req.param].join();

      interfaces_threads_.erase(req.param);
      interfaces_run_.erase(req.param);
      delete interfaces_[req.param];
      interfaces_.erase(req.param);

      std::cout << req.param << " STOPED" << std::endl;
    }
  }
  else if(req.action == "list")
  {
    for(auto it : interfaces_)
      res.values.push_back(it.first);
  }
  else
    res.code = UNKNOW_ACTION;

  return true;
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "ontologenius_multi");

  ros::NodeHandle n;
  n_ = &n;

  ros::service::waitForService("ontologenius/rest", -1);

  language = std::string(argv[1]);
  std::cout << "language " << language << std::endl;

  intern_file = std::string(argv[2]);
  std::cout << "intern_file " << intern_file << std::endl;

  for(int i = 3; i < argc; i++)
    files.push_back(std::string(argv[i]));

  /*interface.run();*/

  ros::ServiceServer service = n_->advertiseService("ontologenius/manage", managerHandle);

  ros::spin();

  for(auto intreface : interfaces_)
    delete intreface.second;

  ROS_DEBUG("KILL ontoloGenius");

  return 0;
}
