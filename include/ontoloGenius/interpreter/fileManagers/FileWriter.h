#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "ontoloGenius/interpreter/fileManagers/FileManager.h"

#include <string>

class FileWriter : public FileManager
{
public:
  FileWriter() {}
  ~FileWriter() {}

  void write(const std::string& text);
  void writeLine(const std::string& text) { write(text + "\n"); }

private:
};

#endif
