#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <map>
#include <unordered_set>

#include "ontoloGenius/interpreter/codeDescription/Code.h"
#include "ontoloGenius/interpreter/Error.h"

class Parser
{
public:
  explicit Parser(Code* code, size_t current_line = 1, bool subparser = false);
  ~Parser();

  Error& getError() {return error_; }

private:
  void checkCode();

  void checkReserved();
  void checkReservedWord(std::string symbol);
  void checkBraquets();
  void checkStringAndComment();
  void getSubsections();
  void getIfBlock();
  size_t getNextIfBlock(int& nb_block, size_t pose);
  void getFromNamespace();
  void replaceOperator();
  std::map<size_t, std::string> splitBySemicolon();
  int splitIfBlock(std::map<size_t, std::string>& splited, std::string ifelse_id);
  void checkInstructionValidity(std::map<size_t, std::string>& splited);
  void checkInstructionValidity(size_t pose, std::string code, bool onFunction = false);
  void checkArgumentValidity(size_t pose, std::string code);

  Code* code_;
  Error error_;
  bool subparser_;
};

#endif
