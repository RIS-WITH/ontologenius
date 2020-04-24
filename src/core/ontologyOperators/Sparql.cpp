#include "ontologenius/core/ontologyOperators/Sparql.h"

namespace ontologenius
{
  Sparql::Sparql()
  {
    onto_ = nullptr;
    error_ = "";
  }

  std::vector<std::map<std::string, std::string>> Sparql::run(const std::string& query)
  {
    if(onto_ == nullptr)
    {
      error_ = "can not create an ontology manipulator";
      return std::vector<std::map<std::string, std::string>>();
    }

    std::vector<std::string> sub_queries = split(query, ",");
    std::vector<triplet_t> sub_queries_triplet;
    for(auto& q : sub_queries)
      sub_queries_triplet.push_back(getTriplet(q));

    return resolve(sub_queries_triplet);
  }

  std::vector<std::map<std::string, std::string>> Sparql::resolve(std::vector<triplet_t> query, const std::map<std::string, std::string>& accu)
  {
    std::vector<std::map<std::string, std::string>> res;

    triplet_t current = query[0];
    query.erase(query.begin());

    std::unordered_set<std::string> values;
    std::string var_name;
    resolveSubQuery(current, accu, var_name, values);

    for(auto& value : values)
    {
      std::map<std::string, std::string> new_accu = accu;
      if(new_accu.find(var_name) != new_accu.end())
      {
        if(new_accu[var_name] != value)
          continue;
      }
      else
        new_accu[var_name] = value;


      std::vector<std::map<std::string, std::string>> local_res;
      if(query.size())
      {
        local_res = resolve(query, new_accu);
        if(local_res.size() == 0)
          continue;
      }

      if(local_res.size() != 0)
      {
        for(auto lr : local_res)
        {
          lr[var_name] = value;
          res.push_back(lr);
        }
      }
      else
      {
        std::map<std::string, std::string> tmp;
        tmp[var_name] = value;
        res.push_back(tmp);
      }
    }

    return res;
  }

  void Sparql::resolveSubQuery(triplet_t triplet, const std::map<std::string, std::string>& accu, std::string& var_name, std::unordered_set<std::string>& values)
  {
    if(triplet.predicat.variable)
      error_ = "predicat can not be a variable in: " + toString(triplet);
    else if(triplet.subject.variable && !triplet.object.variable && (triplet.predicat.name == "isA"))
    {
      var_name = triplet.subject.name;
      if(accu.find(var_name) != accu.end())
        values = getType(triplet, accu.at(var_name));
      else
        values = getType(triplet);
    }
    else if(!triplet.subject.variable && triplet.object.variable && (triplet.predicat.name == "isA"))
    {
      var_name = triplet.object.name;
      if(accu.find(var_name) != accu.end())
        values = getUp(triplet, accu.at(var_name));
      else
        values = getUp(triplet);
    }
    else if(triplet.subject.variable && triplet.object.variable && (triplet.predicat.name == "isA"))
    {
      var_name = triplet.subject.name;
      if(accu.find(var_name) != accu.end())
      {
        triplet.subject.name = accu.at(triplet.subject.name);
        var_name = triplet.object.name;
        if(accu.find(var_name) != accu.end())
          values = getUp(triplet, accu.at(var_name));
        else
          values = getUp(triplet);
      }
      else if(accu.find(triplet.object.name) != accu.end())
      {
        triplet.object.name = accu.at(triplet.object.name);
        values = getType(triplet);
      }
      else
        error_ = "can not resolve query : " + toString(triplet);
    }
    else if(triplet.subject.variable && !triplet.object.variable && (triplet.predicat.name == "hasLabel"))
    {
      var_name = triplet.subject.name;
      if(accu.find(var_name) != accu.end())
        values = find(triplet, accu.at(var_name));
      else
        values = find(triplet);
    }
    else if(!triplet.subject.variable && triplet.object.variable && (triplet.predicat.name == "hasLabel"))
    {
      var_name = triplet.object.name;
      if(accu.find(var_name) != accu.end())
        values = getName(triplet, accu.at(var_name));
      else
        values = getName(triplet);
    }
    else if(triplet.subject.variable && triplet.object.variable && (triplet.predicat.name == "hasLabel"))
    {
      var_name = triplet.subject.name;
      if(accu.find(var_name) != accu.end())
      {
        triplet.subject.name = accu.at(triplet.subject.name);
        var_name = triplet.object.name;
        if(accu.find(var_name) != accu.end())
          values = getName(triplet, accu.at(var_name));
        else
          values = getName(triplet);
      }
      else if(accu.find(triplet.object.name) != accu.end())
      {
        triplet.object.name = accu.at(triplet.object.name);
        values = find(triplet);
      }
      else
        error_ = "can not resolve query : " + toString(triplet);
    }
    else if(triplet.subject.variable && !triplet.object.variable)
    {
      var_name = triplet.subject.name;
      if(accu.find(var_name) != accu.end())
        values = getFrom(triplet, accu.at(var_name));
      else
        values = getFrom(triplet);
    }
    else if(!triplet.subject.variable && triplet.object.variable)
    {
      var_name = triplet.object.name;
      if(accu.find(var_name) != accu.end())
        values = getOn(triplet, accu.at(var_name));
      else
        values = getOn(triplet);
    }
    else if(triplet.subject.variable && triplet.object.variable)
    {
      var_name = triplet.subject.name;
      if(accu.find(var_name) != accu.end())
      {
        triplet.subject.name = accu.at(triplet.subject.name);
        var_name = triplet.object.name;
        if(accu.find(var_name) != accu.end())
          values = getOn(triplet, accu.at(var_name));
        else
          values = getOn(triplet);
      }
      else if(accu.find(triplet.object.name) != accu.end())
      {
        triplet.object.name = accu.at(triplet.object.name);
        values = getFrom(triplet);
      }
      else
        error_ = "can not resolve query : " + toString(triplet);
    }
    else
      error_ = "can not resolve query : " + toString(triplet);
  }

  std::unordered_set<std::string> Sparql::getOn(const triplet_t& triplet, const std::string& selector)
  {
    auto res = onto_->individual_graph_.getOn(triplet.subject.name, triplet.predicat.name);
    if(selector == "")
      return res;
    else if(std::find(res.begin(), res.end(), selector) != res.end())
      return std::unordered_set<std::string>({selector});
    else
      return std::unordered_set<std::string>();
  }

  std::unordered_set<std::string> Sparql::getFrom(const triplet_t& triplet, const std::string& selector)
  {
    auto res = onto_->individual_graph_.getFrom(triplet.object.name, triplet.predicat.name);
    if(selector == "")
      return res;
    else if(std::find(res.begin(), res.end(), selector) != res.end())
      return std::unordered_set<std::string>({selector});
    else
      return std::unordered_set<std::string>();
  }

  std::unordered_set<std::string> Sparql::getUp(const triplet_t& triplet, const std::string& selector)
  {
    if(selector == "")
      return onto_->individual_graph_.getUp(triplet.subject.name);
    else
    {
      auto is = onto_->individual_graph_.getUp(triplet.subject.name);
      if(std::find(is.begin(), is.end(), selector) != is.end())
        return std::unordered_set<std::string>({selector});
      else
        return std::unordered_set<std::string>();
    }
  }

  std::unordered_set<std::string> Sparql::getType(const triplet_t& triplet, const std::string& selector)
  {
    auto res = onto_->individual_graph_.getType(triplet.object.name);
    if(selector == "")
      return res;
    else if(std::find(res.begin(), res.end(), selector) != res.end())
      return std::unordered_set<std::string>({selector});
    else
      return std::unordered_set<std::string>();
  }

  std::unordered_set<std::string> Sparql::find(const triplet_t& triplet, const std::string& selector)
  {
    auto res = onto_->individual_graph_.find(triplet.object.name);
    if(selector == "")
      return res;
    else if(std::find(res.begin(), res.end(), selector) != res.end())
      return std::unordered_set<std::string>({selector});
    else
      return std::unordered_set<std::string>();
  }

  std::unordered_set<std::string> Sparql::getName(const triplet_t& triplet, const std::string& selector)
  {
    auto res = onto_->individual_graph_.getNames(triplet.subject.name);
    if(selector == "")
    {
      std::unordered_set<std::string> set_res;
      for(auto& r : res)
        set_res.insert(r);
      return set_res;
    }
    else if(std::find(res.begin(), res.end(), selector) != res.end())
      return std::unordered_set<std::string>({selector});
    else
      return std::unordered_set<std::string>();
  }

  triplet_t Sparql::getTriplet(const std::string& subquery)
  {
    std::vector<std::string> resources = split(subquery, " ");

    size_t cpt = 0;
    triplet_t res;
    for(const auto& x : resources)
    {
      if(x != "")
      {
        resource_t resource = getResource(x);
        switch (cpt)
        {
          case 0: res.subject = resource; break;
          case 1: res.predicat = resource; break;
          case 2: res.object = resource; break;
          default: error_ = "invalid subquery format in : " + subquery;
        }
        cpt++;
      }
    }

    return res;
  }

  resource_t Sparql::getResource(const std::string& resource)
  {
    std::string text = resource;
    removeUselessSpace(text);

    resource_t res;

    if(text[0] == '?')
    {
      res.variable = true;
      text = text.substr(1);
    }
    else
      res.variable = false;
    res.name = text;

    return res;
  }

  std::string Sparql::toString(const triplet_t& triplet)
  {
    return (triplet.subject.variable ? "?" : "") + triplet.subject.name + " " +
            (triplet.predicat.variable ? "?" : "") + triplet.predicat.name + " " +
            (triplet.object.variable ? "?" : "") + triplet.object.name;
  }

  std::vector<std::string> Sparql::split(const std::string& str, const std::string& delim)
  {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
      pos = str.find(delim, prev);
      if (pos == std::string::npos)
        pos = str.length();

      std::string token = str.substr(prev, pos-prev);

      if (!token.empty())
        tokens.push_back(token);
      prev = pos + delim.length();
    }
    while ((pos < str.length()) && (prev < str.length()));

    return tokens;
  }

  void Sparql::removeUselessSpace(std::string& text)
  {
    while((text[0] == ' ') && (text.size() != 0))
      text.erase(0,1);

    while((text[text.size() - 1] == ' ') && (text.size() != 0))
      text.erase(text.size() - 1,1);
  }

} // namespace ontologenius
