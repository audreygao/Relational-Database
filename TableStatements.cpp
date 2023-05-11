//
//  TableStatements.cpp
//  assignment3
//
//  Created by Allen Munk on 4/17/21.
//

#include "TableStatements.hpp"

namespace ECE141 {
  CreateTableStatement::CreateTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}

  //Set the type for a new Attribute by parsing the type token
  StatusResult  parseAttributeType(Attribute& anAttribute, Tokenizer& aTokenizer) {
    Keywords aType =aTokenizer.current().keyword;
    switch (aType) {
      case Keywords::integer_kw:
        anAttribute.setType(DataTypes::int_type);
      break;
      case Keywords::float_kw:
        anAttribute.setType(DataTypes::float_type);
      break;
      case Keywords::boolean_kw:
        anAttribute.setType(DataTypes::bool_type);
      break;
      case Keywords::datetime_kw:
        anAttribute.setType(DataTypes::datetime_type);
      break;
      case Keywords::varchar_kw:
        anAttribute.setType(DataTypes::varchar_type);
        aTokenizer.next(2);
        anAttribute.setSize(std::stoi(aTokenizer.current().data));
        aTokenizer.next();
      break;
      default:
        break;
    }

    return StatusResult();
  }

StatusResult  parseAttributeFlags(Attribute& anAttribute, Tokenizer& aTokenizer) {
  Keywords aType = aTokenizer.current().keyword;
  switch (aType) {
    case Keywords::not_kw:
      if(aTokenizer.next() && aTokenizer.current().keyword == Keywords::null_kw)
        anAttribute.setNoNull();
      break;
    case Keywords::auto_increment_kw:
      anAttribute.setInc();
    break;
    case Keywords::primary_kw:
      if(aTokenizer.next() && aTokenizer.current().keyword == Keywords::key_kw)
        anAttribute.setPrim();
    break;
    default:
      break;
  }

  return StatusResult();
}

  StatusResult  CreateTableStatement::parse(Tokenizer &aTokenizer) {
    Entity newEntity;
    if(!aTokenizer.next(2))
      return StatusResult(Errors::invalidCommand);
    newEntity.setName(aTokenizer.current().data);
    
    aTokenizer.next();
    
    if(!aTokenizer.skipIf('('))
      return StatusResult(Errors::syntaxError);
    
    while(aTokenizer.remaining() && aTokenizer.current().data != ";") {
      Attribute newAttribute = Attribute();
      int fieldCount = 0;
      while(!aTokenizer.skipIf(',') && !aTokenizer.skipIf(')')) {
        if(fieldCount == 0)
          newAttribute.setName(aTokenizer.current().data);
        else if(fieldCount == 1)
          parseAttributeType(newAttribute, aTokenizer);
        else
          parseAttributeFlags(newAttribute, aTokenizer);
        aTokenizer.next();
        fieldCount++;
      }
      newEntity.addAttribute(newAttribute);
    }
    anEntity = newEntity;
    return StatusResult();
  }

  Entity CreateTableStatement::getEntity(){return anEntity;}

  ShowTableStatement::ShowTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}

  StatusResult ShowTableStatement::parse(Tokenizer &aTokenizer) {
    aTokenizer.next();
    return StatusResult();
  }

  /*                DropTableStatement               */
  DropTableStatement::DropTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}
  StatusResult DropTableStatement::parse(Tokenizer &aTokenizer) {
    if(aTokenizer.next(2)) name = aTokenizer.current().data;
    return StatusResult();
  }

  /*                DescribeTableStatement               */
  DescribeTableStatement::DescribeTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}
  StatusResult DescribeTableStatement::parse(Tokenizer &aTokenizer) {
    if(aTokenizer.next(1)) name = aTokenizer.current().data;
    return StatusResult();
  }

  InsertTableStatement::InsertTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}

  
Value assignValueType(Token aValueString) {
  Value returnValue;
  if(aValueString.type == TokenType::identifier || aValueString.type == TokenType::timedate)
    if(aValueString.data == "true")
      returnValue = true;
    else if(aValueString.data == "false")
      returnValue = false;
    else
      returnValue = aValueString.data;
  else {
    if(aValueString.data.find(".") != std::string::npos)
      returnValue = std::stof(aValueString.data);
    else
      returnValue = std::stoi(aValueString.data);
  }
  return returnValue;
}
  StatusResult InsertTableStatement::parse(Tokenizer &aTokenizer) {
    if(!aTokenizer.next(2))
      return StatusResult(Errors::invalidCommand);
    name = (aTokenizer.current().data);
    aTokenizer.next();
    
    if(!aTokenizer.skipIf('('))
      return StatusResult(Errors::syntaxError);
    
    while(aTokenizer.remaining() && !aTokenizer.skipIf(')')) {
      aTokenizer.skipIf(',');
      attributeNames.push_back(aTokenizer.current().data);
      aTokenizer.next();
    }
    
    if(!aTokenizer.skipIf(')') && !aTokenizer.skipIf(Keywords::values_kw))
      return StatusResult(Errors::syntaxError);
    
    aTokenizer.next();
    while(aTokenizer.more() && !aTokenizer.skipIf(';')) {
      Row* theRow = new Row();
      size_t attributeIndex = 0;
      while(aTokenizer.current().data != ")") {
        if(aTokenizer.current().data != ",") {
          if(attributeIndex >= attributeNames.size())
            return StatusResult(Errors::unexpectedValue);
          theRow -> addValue(attributeNames.at(attributeIndex), assignValueType(aTokenizer.current()));
          attributeIndex += 1;
        }
        aTokenizer.next();
      }
      rows.push_back(std::unique_ptr<Row>(theRow));
      aTokenizer.skipIf(')');
      aTokenizer.skipIf(',');
      aTokenizer.skipIf('(');
    }
    return StatusResult();
  }

  RowCollection& InsertTableStatement::getRows() {
    return rows;
  }
  std::vector<std::string> InsertTableStatement::getAttributes() {
    return attributeNames;
  }

SelectTableStatement::SelectTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}



std::vector<Keywords> preprocessKeywords(Tokenizer &aTokenizer) {
  std::vector<Keywords> returnVector;
  static Keywords filterWords[] = {Keywords::order_kw, Keywords::where_kw, Keywords::limit_kw, Keywords::set_kw, Keywords::left_kw, Keywords::right_kw, Keywords::join_kw};
  while(aTokenizer.remaining()) {
    auto theIt = std::find(std::begin(filterWords),
                             std::end(filterWords), aTokenizer.current().keyword);
    if(theIt != std::end(filterWords)) {
      returnVector.push_back(*theIt);
      if(*theIt == Keywords::left_kw || *theIt == Keywords::right_kw)
        aTokenizer.next();
    }
    aTokenizer.next();
  }
  
  aTokenizer.restart();
  return returnVector;
  
}

StatusResult parseTableFields(Tokenizer &aTokenizer, std::string& aTableName) {
  StatusResult theResult = StatusResult();
  if(!aTokenizer.remaining())
    theResult.error = Errors::invalidArguments;
  else {
    if(aTokenizer.skipIf(TokenType::identifier) && aTokenizer.skipIf('.')) {
      aTableName = aTokenizer.current().data;
      aTokenizer.next();
    }
    else
      theResult.error = Errors::invalidArguments;
  }
  return theResult;
}
  StatusResult parseTableName(Tokenizer &aTokenizer, std::string& aFieldName) {
    StatusResult theResult = StatusResult();
    if(!aTokenizer.remaining())
      theResult.error = Errors::invalidArguments;
    else {
      aFieldName = aTokenizer.current().data;
      aTokenizer.next();
    }
    return theResult;
  }

  StatusResult SelectTableStatement::parseJoin(Tokenizer &aTokenizer) {
    Token &theToken = aTokenizer.current();
    StatusResult theResult{joinTypeExpected};

    Keywords theJoinType{Keywords::join_kw}; //could just be a plain join
    if(in_array<Keywords>(gJoinTypes, theToken.keyword)) {
      theJoinType=theToken.keyword;
      aTokenizer.next(1); //yank the 'join-type' token (e.g. left, right)
      if(aTokenizer.skipIf(Keywords::join_kw)) {
        std::string theTable;
        if((theResult=parseTableName(aTokenizer, theTable))) {
          //Join(const std::string &aTable, Keywords aType)
          Join theJoin(theTable, theJoinType, Operand(), Operand());
          theResult.error=keywordExpected; //on...
          if(aTokenizer.skipIf(Keywords::on_kw)) { //LHS field = RHS field
            //TableField LHS("");
            if((theResult=parseTableFields(aTokenizer, theJoin.expr.lhs.name))) {
              if(aTokenizer.skipIf('=')) {
                if((theResult=parseTableFields(aTokenizer, theJoin.expr.rhs.name))) {
                  joins.push_back(theJoin);
                }
              }
            }
          }
        }
      }
    }
    return theResult;
  }
StatusResult SelectTableStatement::parse(Tokenizer &aTokenizer) {
  Query theQuery;
//Filters theFilter;

  std::vector theFilterKeywords = preprocessKeywords(aTokenizer);
  
  //Skip first token (already validated they will be SELECT)
  if(aTokenizer.next(1)) {
    //wild card case
    if(aTokenizer.current().data == "*") {
      theQuery.setSelectAll(true);
      aTokenizer.next();
    }
    //parse the attribute arguements
    else {
      StringList theSelectList;
      while(aTokenizer.current().keyword != Keywords::from_kw && aTokenizer.more()) {
        theSelectList.push_back(aTokenizer.current().data);
        aTokenizer.next();
        aTokenizer.skipIf(',');
      }
      if(!aTokenizer.more())
        return StatusResult(Errors::invalidArguments);
      theQuery.setSelect(theSelectList);
    }
    
    //know the current keyword is from; set table name
    if(aTokenizer.next())
      name = (aTokenizer.current().data);
    else
      return StatusResult(Errors::invalidArguments);
    
    aTokenizer.next();
    
    //Check for any time of filtering keywrods
    for(std::vector<Keywords>::iterator filterIterator = theFilterKeywords.begin(); filterIterator != theFilterKeywords.end(); filterIterator++) {
      Keywords theKeyword = *filterIterator;
      //If ORDER BY
      if(Keywords::order_kw == theKeyword) {
        if(aTokenizer.skipIf(Keywords::order_kw) && aTokenizer.skipIf(Keywords::by_kw)) {
          theQuery.orderBy(aTokenizer.current().data);
          aTokenizer.next();
        }
      }
      //If WHERE
      else if(Keywords::where_kw == theKeyword) {
        aTokenizer.next();
        filter.parse(aTokenizer);
      }
      //If LIMIT
      else if(Keywords::limit_kw == theKeyword) {
        if(aTokenizer.skipIf(Keywords::limit_kw) && TokenType::number == aTokenizer.current().type) {
          theQuery.setLimit(stoi(aTokenizer.current().data));
          aTokenizer.next();
        }
      }
      else if(in_array<Keywords>(gJoinTypes, theKeyword)) {
        parseJoin(aTokenizer);
      }
    }
  }
  query = theQuery;
  return StatusResult();
}

  Query& SelectTableStatement::getQuery() {
    return query;
  }

  Filters& SelectTableStatement::getFilter() {
    return filter;
  }

  JoinList& SelectTableStatement::getJoins() {
    return joins;
  }


UpdateTableStatement::UpdateTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}

StatusResult UpdateTableStatement::parse(Tokenizer &aTokenizer) {


  std::vector theFilterKeywords = preprocessKeywords(aTokenizer);
  
  if(aTokenizer.next(1)) {
    name = aTokenizer.current().data;
    aTokenizer.next();
    
    //Check for any time of filtering keywrods
    for(std::vector<Keywords>::iterator filterIterator = theFilterKeywords.begin(); filterIterator != theFilterKeywords.end(); filterIterator++) {
      Keywords theKeyword = *filterIterator;
      //If SET
      if(Keywords::set_kw == theKeyword) {
        aTokenizer.next();
        updatedValues.parse(aTokenizer);
      }
      //If WHERE
      else if(Keywords::where_kw == theKeyword) {
        aTokenizer.next();
        filter.parse(aTokenizer);
      }
      else {
        return StatusResult(Errors::invalidArguments);
      }
    }
  }
  return StatusResult();
}

Filters& UpdateTableStatement::getFilter(){
  return filter;
}
Filters& UpdateTableStatement::getUpdatedValues() {
  return updatedValues;
}

DeleteTableStatement::DeleteTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}

StatusResult DeleteTableStatement::parse(Tokenizer &aTokenizer) {


  std::vector theFilterKeywords = preprocessKeywords(aTokenizer);
  
  if(aTokenizer.next(2)) {
    name = aTokenizer.current().data;
    aTokenizer.next();
    
    //Check for any time of filtering keywrods
    for(std::vector<Keywords>::iterator filterIterator = theFilterKeywords.begin(); filterIterator != theFilterKeywords.end(); filterIterator++) {
      Keywords theKeyword = *filterIterator;
      
      //If WHERE
      if(Keywords::where_kw == theKeyword) {
        aTokenizer.next();
        filter.parse(aTokenizer);
      }
      else {
        return StatusResult(Errors::invalidArguments);
      }
    }
  }
  return StatusResult();
}

Filters& DeleteTableStatement::getFilter(){
  return filter;
}

DumpTableStatement::DumpTableStatement(Keywords aKeyword) : TableStatement(aKeyword) {}
StatusResult  DumpTableStatement::parse(Tokenizer &aTokenizer){
  if(aTokenizer.next(2)) {
    name = aTokenizer.current().data;
  }
  return StatusResult();
}


ShowIndexesStatement::ShowIndexesStatement(Keywords aKeyword) : TableStatement(aKeyword){}
StatusResult  ShowIndexesStatement::parse(Tokenizer &aTokenizer) {
  return StatusResult();
}

ShowIndexStatement::ShowIndexStatement(Keywords aKeyword) : TableStatement(aKeyword){}
StatusResult  ShowIndexStatement::parse(Tokenizer &aTokenizer) {
  
  aTokenizer.next(2);
  std::vector<std::string> theFields;
  while(aTokenizer.current().keyword != Keywords::from_kw && aTokenizer.remaining()) {
    theFields.push_back(aTokenizer.current().data);
    if(!aTokenizer.remaining())
      return StatusResult(Errors::syntaxError);
    aTokenizer.next();
    aTokenizer.skipIf(',');
  }
  
  aTokenizer.skipIf(Keywords::from_kw);
  if(!aTokenizer.remaining())
    return StatusResult(Errors::syntaxError);
  name = aTokenizer.current().data;
  
  fields = theFields;
  
  return StatusResult();
}

std::vector<std::string> ShowIndexStatement::getFields() {
  return fields;
}

AlterTableStatement::AlterTableStatement(Keywords aKeyword) : TableStatement(aKeyword){}
StatusResult  AlterTableStatement::parse(Tokenizer &aTokenizer) {
    
  //skip ALTER TABLE go get the table name
  if(!aTokenizer.next(2))
    return StatusResult(Errors::invalidCommand);
  name = (aTokenizer.current().data);
  aTokenizer.next();
  
  //get the add or drop sub command
  subCmd = aTokenizer.current().keyword;
  aTokenizer.next();
  
  switch(subCmd) {
      //add have to parse the attribute types and flags
    case Keywords::add_kw:
      while(aTokenizer.remaining() && aTokenizer.current().data != ";") {
        Attribute newAttribute = Attribute();
        int fieldCount = 0;
        while(!aTokenizer.skipIf(',')  && !aTokenizer.skipIf(';')) {
          if(fieldCount == 0)
            newAttribute.setName(aTokenizer.current().data);
          else if(fieldCount == 1)
            parseAttributeType(newAttribute, aTokenizer);
          else
            parseAttributeFlags(newAttribute, aTokenizer);
          aTokenizer.next();
          fieldCount++;
        }
        attributes.push_back(newAttribute);
      }
      break;
    case Keywords::drop_kw:
      while(aTokenizer.remaining() && aTokenizer.current().data != ";") {
        Attribute newAttribute = Attribute();
        while(!aTokenizer.skipIf(',') && !aTokenizer.skipIf(';')) {
          newAttribute.setName(aTokenizer.current().data);
          aTokenizer.next();
        }
        attributes.push_back(newAttribute);
      }
      break;
    default: break;
  }

  return StatusResult();
}
Keywords AlterTableStatement::getSubCmd() {return subCmd;}
AttributeList AlterTableStatement::getAttributes() {return attributes;}

}
