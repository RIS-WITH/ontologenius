#ifndef NODEWRITER_H
#define NODEWRITER_H

#include <string>

class ValuedNodeData;

class NodeWriter
{
public:
  NodeWriter() {file_ = nullptr; }
  ~NodeWriter() {}

  FILE* file_;

  void writeDictionary(ValuedNodeData* node);

  void writeString(std::string text);
};

#endif
