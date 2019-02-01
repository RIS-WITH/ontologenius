#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <map>

#include <stdint.h>

#include "ontoloGenius/interpreter/codeDescription/Code.h"

class Error
{
public:
  explicit Error(Code* code = nullptr);
  ~Error() {}

  void printError(size_t pose, const std::string& message);
  void printWarning(size_t pose, const std::string& message);
  void printNote(size_t pose, const std::string& message);

  void printStatus();

  bool isOnError();

  void cpy(Error& error);
private:
  Code* code_;

  uint16_t nb_error;
  uint16_t nb_wrng;

  size_t getBeginOfLine(size_t line_nb);
  void printCursor(size_t pose);
  void printMessage(size_t pose, const std::string& message);
};

#endif
