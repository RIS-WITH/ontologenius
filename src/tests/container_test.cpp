#include "ontoloGenius/core/ontoGraphs/Branchs/ValuedNode.h"
#include "ontoloGenius/core/ontoGraphs/BranchContainer/BranchContainerDyn.h"
#include "ontoloGenius/core/ontoGraphs/BranchContainer/BranchContainerMap.h"

#include "ros/ros.h"

#include <chrono>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <unordered_set>
#include <stdio.h>
#include <math.h>

class FileReader
{
public:
  explicit FileReader(std::string path)
  {
    len = cpt = 0;
    file_ = NULL;
    init(path, "r");
  }
  ~FileReader()
  {
    if(file_ != NULL)
      fclose(file_);
  }

  std::string readLine()
  {
    char * line = NULL;
    if(getline(&line, &len, file_) != -1)
    {
        cpt++;
        return std::string(line);
    }
    else
      return "";
  }

  size_t getNbLine() {return cpt; }
private:
  size_t len;
  size_t cpt;

  void init(std::string file_name, std::string option)
  {
    file_ = fopen(file_name.c_str(), option.c_str());
    if(file_ == NULL)
      std::cout << "Fail to open file " << file_name << " with option '" << option << "'" << std::endl;
  }

  void reset(std::string file_name)
  {
    file_ = fopen(file_name.c_str(), "w");
    if(file_ == NULL)
      std::cout << "Fail to reset file " << file_name << std::endl;
    else
      fclose(file_);
    file_ = NULL;
  }

  FILE* file_;
};

std::vector<ValuedNode*> full_words;

void readFullWords()
{
  FileReader reader("/home/gsarthou/Desktop/words.txt");
  bool oef = false;
  do
  {
    std::string res = reader.readLine();
    if(res == "")
      oef = true;
    else
    {
      ValuedNode* tmp = new ValuedNode(res);
      full_words.push_back(tmp);
    }
  }
  while(oef == false);
  std::cout << reader.getNbLine() << std::endl;
}

double findAll(BranchContainerBase<ValuedNode>* container, std::vector<ValuedNode*> words)
{
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

  for(size_t i = 0; i < words.size(); i++)
    ValuedNode* none = container->find(words[i]->value());

  std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  return time_span.count() / words.size();
}

double findAllNTime(BranchContainerBase<ValuedNode>* container, std::vector<ValuedNode*> words, size_t n)
{
  double sum = 0;
  for(size_t i = 0; i < n; i++)
  {
    sum += findAll(container, words);
  }

  return sum / n;
}

std::vector<ValuedNode*> getPartOfWords(float percent)
{
  std::vector<ValuedNode*> tmp;
  for(size_t i = 0; i < full_words.size() * percent / 100; i++)
  {
    size_t myIndex = rand() % full_words.size();
    tmp.push_back(full_words[myIndex]);
  }

  return tmp;
}

std::vector<ValuedNode*> creatTestVector(float percent, std::vector<ValuedNode*> words)
{
  std::vector<ValuedNode*> tmp;
  for(size_t i = 0; i < words.size() * percent / 100; i++)
  {
    size_t myIndex = rand() % words.size();
    tmp.push_back(words[myIndex]);
  }

  if(tmp.size() < full_words.size() / 2)
  {
    std::vector<ValuedNode*> res;
    while(res.size() < full_words.size())
    {
      res.insert(res.end(), tmp.begin(), tmp.end());
    }
    return res;
  }
  else
    return tmp;
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "ontologenius_container_tester");

  readFullWords();

  size_t N = 100;

  for(size_t i = 1; i <= 5; i++)
  {
    std::cout << "########" << std::endl << "#  " << i <<"  #" << std::endl << "########" << std::endl << std::endl;
    BranchContainerDyn<ValuedNode> container_dyn;
    BranchContainerMap<ValuedNode> container_map;

    std::vector<ValuedNode*> part_of_words = getPartOfWords(i);
    container_dyn.load(part_of_words);
    container_map.load(part_of_words);

    for(float j = 0.1; j <= 2; j = j + 0.1)
    {
      std::vector<ValuedNode*> vect = creatTestVector(j, part_of_words);
      double time_span_dyn = findAllNTime(&container_dyn, vect, N);
      double time_span_map = findAllNTime(&container_map, vect, N);

      std::cout << i << " " << j << " " << time_span_dyn << " " << time_span_map << " " << vect.size()*N << " " << time_span_dyn*vect.size()*N << " " << time_span_map*vect.size()*N << std::endl;
    }

    size_t size = full_words.size() * i / 100;
    std::cout << log2(size) << " : " << size << std::endl;
  }

  for(size_t i = 0; i < full_words.size(); i++)
    delete full_words[i];

  ROS_DEBUG("test done");

  return 0;
}
