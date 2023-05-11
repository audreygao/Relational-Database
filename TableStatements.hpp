//
//  TableStatements.hpp
//  assignment3
//
//  Created by Allen Munk on 4/17/21.
//

#ifndef TableStatements_hpp
#define TableStatements_hpp

#include <stdio.h>
#include <locale>
#include "Statement.hpp"
#include "Entity.hpp"
#include "Tokenizer.hpp"
#include "Row.hpp"
#include "Query.hpp"
#include "Filters.hpp"
#include "Join.hpp"
#include "Helpers.hpp"

namespace ECE141 {

class TableStatement: public Statement{
public:
  TableStatement(Keywords aKeyword, std::string name="") : Statement(aKeyword, name){}
  StatusResult  parse(Tokenizer &aTokenizer) override {return StatusResult();}
private:
  
};

class CreateTableStatement: public TableStatement{
public:
  CreateTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  Entity getEntity();
private:
  Entity anEntity;
};

class ShowTableStatement: public TableStatement{
public:
  ShowTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
};

class DropTableStatement: public TableStatement{
public:
  DropTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
};

class DescribeTableStatement: public TableStatement{
public:
  DescribeTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
};

class InsertTableStatement: public TableStatement{
public:
  InsertTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  RowCollection& getRows();
  std::vector<std::string> getAttributes();
  
private:
  RowCollection rows;
  std::vector<std::string> attributeNames;
};

class SelectTableStatement: public TableStatement{
public:
  SelectTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  Query& getQuery();
  Filters& getFilter();
  JoinList& getJoins();
  StatusResult parseJoin(Tokenizer &aTokenizer);
  
private:
  Query query;
  Filters filter;
  JoinList joins;
  
};

class UpdateTableStatement: public TableStatement {
public:
  UpdateTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  Filters& getFilter();
  Filters& getUpdatedValues();
  
private:
  Filters filter;
  Filters updatedValues;
  
};

class DeleteTableStatement: public TableStatement {
public:
  DeleteTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  Filters& getFilter();
  
private:
  Filters filter;

  
};

class DumpTableStatement: public TableStatement {
public:
  DumpTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  
private:

  
};

class ShowIndexesStatement: public TableStatement {
public:
  ShowIndexesStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  
private:

  
};

class ShowIndexStatement: public TableStatement {
public:
  ShowIndexStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  std::vector<std::string> getFields();
private:
  std::vector<std::string> fields;
  
};

class AlterTableStatement: public TableStatement {
public:
  AlterTableStatement(Keywords aKeyword);
  StatusResult  parse(Tokenizer &aTokenizer) override;
  Keywords getSubCmd();
  AttributeList getAttributes();
private:
  Keywords subCmd;
  AttributeList attributes;
  
};

}

#endif /* TableStatements_hpp */
