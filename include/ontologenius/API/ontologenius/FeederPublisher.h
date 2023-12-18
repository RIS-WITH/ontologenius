#ifndef ONTOLOGENIUS_FEEDERPUBLISHER_H
#define ONTOLOGENIUS_FEEDERPUBLISHER_H

#include <atomic>
#include <stdlib.h>

#include "ontologenius/compat/ros.h"

namespace onto {

/// @brief The FeederPublisher class provides an abstraction ontologenius feeder(insert) ROS topic.
/// Working in a closed world can be interesting, but with ontologenius, you can also choose to work in an open world by adding and modifying the agent's knowledge base during its operation.
/// The feeder publisher is used to insert and delete knowledge dynamically.
/// The feeding process is asynchronous and therefore does not guarantee any response time.
/// It still provides functions to synchronize if you have applications where you have to query the ontology right after having modified it.
/// All modifications can be time-stamped for advanced uses using the republication mechanism.
class FeederPublisher
{
public:
  /// @brief Constructs a FeederPublisher.
  /// Can be used in a multi-ontology mode by specifying the name of the ontology name.
  /// @param name is the instance to be connected to. For classic use, name should be defined as "".
  explicit FeederPublisher(const std::string& name) :
          name_(name),
          notification_callback_([](auto& msg){ (void) msg; }),
          commit_nb_((time(nullptr), rand() % 100000 + 1)),
          updated_(false),
          pub_(name.empty() ? "ontologenius/insert" : "ontologenius/insert/" + name, 1000),
          stamped_pub_((name == "") ? "ontologenius/insert_stamped" : "ontologenius/insert_stamped/" + name, 1000),
          commit_sub_(name_ == "" ? "ontologenius/end" : "ontologenius/end/" + name_, 1000, &FeederPublisher::commitCallback, this),
          notif_sub_(name_ == "" ? "ontologenius/feeder_notifications" : "ontologenius/feeder_notifications/" + name_, 1000, &FeederPublisher::notificationCallback, this)
  {

  }

  FeederPublisher(FeederPublisher& other) = delete;

  /*FeederPublisher(FeederPublisher& other) :
          name_(other.name_),
          pub_(n_.advertise<std_msgs_compat::String>((other.name_ == "") ? "ontologenius/insert" : "ontologenius/insert/" + other.name_, 1000)),
          stamped_pub_(n_.advertise<ontologenius::compat::OntologeniusStampedString>((other.name_ == "") ? "ontologenius/insert_stamped" : "ontologenius/insert_stamped/" + other.name_, 1000))
  {
    commit_sub_ = n_.subscribe(name_ == "" ? "ontologenius/end" : "ontologenius/end/" + name_, 1000, &FeederPublisher::commitCallback, this);
    notif_sub_ = n_.subscribe(name_ == "" ? "ontologenius/feeder_notifications" : "ontologenius/feeder_notifications/" + name_, 1000, &FeederPublisher::notificationCallback, this);
    commit_nb_ = other.commit_nb_;
    updated_ = false;
  }*/

  /// @brief Adds the fact that "from" is linked with "on" by the property "property".
  /// At least "from" or "on" must be already known to the system. If one of them is unknown, it will be automatically created.
  /// The property can be unknown before calling this function.
  /// @param from is the subject of the triplet to add.
  /// @param property is the predicat of the triplet to add.
  /// @param on is the object of the triplet to add.
  /// @param stamp is the time at which the added relation become true.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void addProperty(const std::string& from, const std::string& property, const std::string& on, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Adds the fact that "from" is linked to the data "value" of type "type" by the property "property".
  /// At least "from" must be already known to the system.
  /// The property can be unknown before calling this function.
  /// @param from is the subject of the triplet to add.
  /// @param property is the predicat of the triplet to add.
  /// @param type is triplet object type.
  /// @param value is the value of the triplet object.
  /// @param stamp is the time at which the added relation become true.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void addProperty(const std::string& from, const std::string& property, const std::string& type, const std::string& value, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Adds the inheratage: "from" is a "on". "from" and "on" could by a class, an individual or a property.
  /// At least from or on must be already known to the system. If one of them is unknown, it will be automatically created.
  /// @param from is the parent concept. It could by a class, an individual or a property.
  /// @param on is the child concept. It could by a class, an individual or a property.
  /// @param stamp is the time at which the added relation become true.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void addInheritage(const std::string& from, const std::string& on, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Adds the label "name" in the language "lang" to the class, individual, or property "from".
  /// "from" must be already known to the system.
  /// @param from is the concept (individual, class, property) to which add a name in natural language.
  /// @param lang is the language indentifier (en, fr, de, ...).
  /// @param name is the concept name in natural language.
  /// @param stamp is the time at which the added relation become true.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void addLanguage(const std::string& from, const std::string& lang, const std::string& name, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Adds a class or individual.
  /// @param from the individual or class to add.
  /// @param stamp is the time at which the added relation become true.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void addConcept(const std::string& from, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Adds an object property inverse axiom.
  /// @param property is the object property for which its inverse is define.
  /// @param inverse_property is the inverse property of the previous object property.
  /// @param stamp is the time at which the added relation become true.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void addInverseOf(const std::string& property, const std::string& inverse_property, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());

  /// @brief Removes the fact that "from" is linked to any object by the property "property".
  /// After this action, knowledge of the property is not removed.
  /// @param from is the subject of the triplet to remove.
  /// @param property is the predicat of the triplet to remove.
  /// @param stamp is the time at which the added relation become false.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void removeProperty(const std::string& from, const std::string& property, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Removes the fact that "from" is linked with "on" by the property "property".
  /// After this action, knowledge of the property is not removed.
  /// @param from is the subject of the triplet to remove.
  /// @param property is the predicat of the triplet to remove.
  /// @param on is the object of the triplet to remove.
  /// @param stamp is the time at which the added relation become false.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void removeProperty(const std::string& from, const std::string& property, const std::string& on, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Removes the fact that "from" is linked to the data "value" of type "type" by the property "property".
  /// After this action, knowledge of the property is not removed.
  /// @param from is the subject of the triplet to remove.
  /// @param property is the predicat of the triplet to remove.
  /// @param type is triplet object type.
  /// @param value is the value of the triplet object.
  /// @param stamp is the time at which the added relation become false.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void removeProperty(const std::string& from, const std::string& property, const std::string& type, const std::string& value, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Removes the inheratage: "from" is a "on". "from" and "on" could by a class, an individual or a property.
  /// @param from is the parent concept. It could by a class, an individual or a property.
  /// @param on is the child concept. It could by a class, an individual or a property.
  /// @param stamp is the time at which the added relation become false.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void removeInheritage(const std::string& from, const std::string& on, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Removes the label "name" in the language "lang" to the class, individual, or property "from".
  /// @param from is the concept (individual, class, property) to which remove a name in natural language.
  /// @param lang is the language indentifier (en, fr, de, ...).
  /// @param name is the concept name in natural language.
  /// @param stamp is the time at which the added relation become false.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void removeLanguage(const std::string& from, const std::string& lang, const std::string& name, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Removes a class or individual.
  /// All inheritance, properties, and labels applied to "from" are also removed
  /// @param from the individual or class to remove.
  /// @param stamp is the time at which the added relation become false.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void removeConcept(const std::string& from, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());
  /// @brief Removes an object property inverse axiom.
  /// @param property is the object property for which its inverse has to be removed.
  /// @param inverse_property is the inverse property of the previous object property.
  /// @param stamp is the time at which the added relation become false.
  /// If the time stamp stamp is not defined, the function takes the current ROS time as the time stamp.
  void removeInverseOf(const std::string& property, const std::string& inverse_property, const ontologenius::compat::onto_ros::Time& stamp = ontologenius::compat::onto_ros::Node::get().current_time());

  /// @brief Returns the number of subscribers that are currently connected to the internal ROS publisher.
  size_t getNumSubscribers() { /*return stamped_pub_.getNumSubscribers();*/ return 0; } // todo
  /// @brief Blocks while no subscribers are currently connected to the internal ROS publisher.
  void waitConnected()
  {
    ontologenius::compat::onto_ros::Rate loop_rate(100);

    while(ontologenius::compat::onto_ros::Node::get().ok() && (getNumSubscribers() == 0))
      loop_rate.sleep();
  }

  /// @brief Waits until all changes have been applied.
  /// @param timeout is the expiration time in milliseconds. The default value -1 represents an infinite wait.
  /// @return Returns false if the function returns on a timeout.
  bool waitUpdate(int32_t timeout = -1);
  /// @brief Saves all the modifications from the previous commit and waits until all changes have been applied.
  /// This function can only be used on a copied ontology.
  /// @param timeout is the expiration time in milliseconds. The default value -1 represents an infinite wait.
  /// @return Returns the commit id and an empty string if the function returns on a timeout.
  std::string commit(int32_t timeout = -1);
  /// @brief Saves all the modifications from the previous commit with a specific id commit_name and waits until all changes have been applied.
  /// This function can only be used on a copied ontology.
  /// @param commit_name is the identifier of the commit.
  /// @param timeout is the expiration time in milliseconds. The default value -1 represents an infinite wait.
  /// @return Returns false if the function returns on a timeout.
  bool commit(const std::string& commit_name, int32_t timeout = -1);
  /// @brief Apply the necessary changes to return to the specified commit_name and waits until all changes have been applied.
  /// This function can only be used on a copied ontology.
  /// @param commit_name is the identifier of the commit to checkout.
  /// @param timeout is the expiration time in milliseconds. The default value -1 represents an infinite wait.
  /// @return Returns false if the function returns on a timeout.
  bool checkout(const std::string& commit_name, int32_t timeout = -1);

  /// @brief Register a callback function to get notifications from the feeder.
  /// @param callback is the callback function taking a string.
  void registerNotificationCallback(const std::function<void(const std::string&)>& callback) { notification_callback_ = callback; }

private:
  std::string name_;
  std::function<void(const std::string&)> notification_callback_;

  size_t commit_nb_;
  std::atomic<bool> updated_;

  ontologenius::compat::onto_ros::Publisher<std_msgs_compat::String> pub_;
  ontologenius::compat::onto_ros::Publisher<ontologenius::compat::OntologeniusStampedString> stamped_pub_;
  ontologenius::compat::onto_ros::Subscriber<std_msgs_compat::String> commit_sub_;
  ontologenius::compat::onto_ros::Subscriber<std_msgs_compat::String> notif_sub_;




  void sendNop();

  void publish(const std::string& str);
  void publishStamped(const std::string& str, const ontologenius::compat::onto_ros::Time& stamp);

  void commitCallback(const std_msgs_compat::String::ConstPtr& msg);
  void notificationCallback(const std_msgs_compat::String::ConstPtr& msg);
};

} // namespace onto

#endif // ONTOLOGENIUS_FEEDERPUBLISHER_H
