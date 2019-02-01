#ifndef ONTOLOGENIUS_CORE_UTILITY_UTILITY_H
#define ONTOLOGENIUS_CORE_UTILITY_UTILITY_H

#include <string>

#include "ontologenius/REST.h"
#include "ros/ros.h"

int send_request(std::string method, std::string url, std::string body, std::string* response)
{
  ros::NodeHandle n;

  ros::ServiceClient client = n.serviceClient<ontologenius::REST>("ontologenius/rest");

  ontologenius::REST srv;
  srv.request.method = method;
  srv.request.URL = url;
  srv.request.body = body;

  if(!client.call(srv))
  {
    ROS_ERROR("REST client call failed");
    return -1;
  }
  else
  {
    (*response) = srv.response.text;
    return 0;
  }
}

#endif //ONTOLOGENIUS_CORE_UTILITY_UTILITY_H
