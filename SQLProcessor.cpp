//
//  SQLProcessor.cpp
//  RGAssignment3
//
//  Created by rick gessner on 4/1/21.
//

#include "SQLProcessor.hpp"
#include "Application.hpp"
#include "Database.hpp"
#include "Helpers.hpp"
#include "TableView.hpp"

#include "Timer.hpp"
#include "QueryView.hpp"
#include "ShowView.hpp"
#include "TableDescriptionView.hpp"
#include "indexesView.hpp"
namespace ECE141 {
  
using StmtFactory = Statement* (*)(Tokenizer &aTokenizer);

SQLProcessor::SQLProcessor(std::ostream &anOutput, Database* db): CmdProcessor(anOutput), db(db){}
SQLProcessor::~SQLProcessor(){}

void SQLProcessor::setDB(Database* aDB) {
  this -> db = aDB;
}


bool isTableCreate(Tokenizer& aTokenizer){
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::create_kw).then(Keywords::table_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isTableDrop(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::drop_kw).then(Keywords::table_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isTableShow(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::show_kw).then(Keywords::tables_kw);
  aTokenizer.restart();
  return returnBool;
}

bool isDescribe(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::describe_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isInsert(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::insert_kw).then(Keywords::into_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isSelect(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::select_kw);
  aTokenizer.restart();
  return returnBool;
}

bool isUpdate(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::update_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isDelete(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::delete_kw).then(Keywords::from_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isShowIndexes(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::show_kw).then(Keywords::indexes_kw);
  aTokenizer.restart();
  return returnBool;
}

bool isShowIndex(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::show_kw).then(Keywords::index_kw);
  aTokenizer.restart();
  return returnBool;
}

bool isAlter(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::alter_kw).then(Keywords::table_kw)
      .thenId(theName).then(Keywords::add_kw);
  aTokenizer.restart();
  if(!returnBool) {
    returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::alter_kw).then(Keywords::table_kw)
        .thenId(theName).then(Keywords::drop_kw);
  }
  return returnBool;
}

bool SQLProcessor::isKnown(Keywords aKeyword) {
  static Keywords theKnown[]={Keywords::create_kw, Keywords::drop_kw, Keywords::show_kw, Keywords::describe_kw, Keywords::insert_kw,Keywords::select_kw, Keywords::update_kw, Keywords::delete_kw, Keywords::alter_kw};
  auto theIt = std::find(std::begin(theKnown),
                           std::end(theKnown), aKeyword);
  return theIt!=std::end(theKnown);
}



CmdProcessor* SQLProcessor::recognizes(Tokenizer &aTokenizer) {
  if(isKnown(aTokenizer.current().keyword)) {
    if(isTableCreate(aTokenizer) || isTableDrop(aTokenizer) || isTableShow(aTokenizer) || isDescribe(aTokenizer)|| isInsert(aTokenizer) || isSelect(aTokenizer)||isUpdate(aTokenizer)||isDelete(aTokenizer)||isShowIndexes(aTokenizer)||isShowIndex(aTokenizer)
        || isAlter(aTokenizer)){
      if(!db) {
        output << "Error, no database selected\n";
        return nullptr;
      }
      aTokenizer.restart();
      return this;
    }
  }
    return nullptr;
}

StatusResult SQLProcessor::createTable(std::ostream &anOutput, Entity anEntity) {
  //encode the entity into stream and save the encoded entity to database
  StatusResult status = db -> saveEntity(anEntity);
  
  timer.stop();
  
  QueryView theView(anEntity.getName(), Keywords::create_kw, status, timer.elapsed(), 0);
  theView.show(anOutput);
  return status;
}

StatusResult SQLProcessor::showTables(std::ostream &anOutput) {

  //load the names of the all entities/tables in the current database
  EntityCollection entities;
  StringList tableNames;
  StatusResult status = db -> getAllEntities(entities, tableNames);
  
  timer.stop();
  
  //Ask view to show the lists of tables
  ShowView theView(tableNames, timer.elapsed(), Keywords::table_kw);
  theView.show(anOutput);
  return status;
}

StatusResult SQLProcessor::dropTable(std::ostream &anOutput, std::string tableName) {

  //ask database to drop the table {tableName}
  int rowsDeleted = 0;
  StatusResult status = db -> dropTable(tableName, rowsDeleted);
  
  timer.stop();
  
  QueryView theView(tableName, Keywords::drop_kw, status, timer.elapsed(), 0);
  theView.show(anOutput);
  return status;
}

StatusResult SQLProcessor::describeEntity(std::ostream &anOutput, std::string tableName) {

  //retrieve the entity of a table with name tableNmame
  Entity anEntity;
  StatusResult status = db -> getEntity(anEntity, tableName);
  timer.stop();
  
  //Ask View to display info of the entity
  TableDescriptionView theView(timer.elapsed(), Keywords::describe_kw, status);
  theView.setEntity(anEntity);
  theView.show(anOutput);

  return status;
}

StatusResult SQLProcessor::insertRows(std::ostream &anOutput, std::string tableName,StringList attributeNames, RowCollection& newRows) {
  //retrieve the entity of table tableName in the current database
  Entity anEntity;
  StatusResult status;
  if(db -> getEntity(anEntity, tableName)) {
    std::cout << std::endl;
    //validate the rows with the entity
    if(!anEntity.validateRows(attributeNames, newRows))
      return StatusResult(Errors::invalidAttribute);
        
    //use the hash of the tableName as the refId
    uint32_t hash = anEntity.hashString(tableName);
    AttributeList aList;
    anEntity.getAttributeList(aList);
    
    for(auto& row : newRows) {
      row -> setAttributeList(aList);
    }

    status = db -> encodeAndSaveRows(newRows, hash, anEntity.getPrimaryKey(), anEntity.getIndexBlockNum());
    if(status)
      status = db -> updateEntity(anEntity);
  }

  timer.stop();

  QueryView theView(anEntity.getName(), Keywords::insert_kw, status, timer.elapsed(), newRows.size());
  theView.show(anOutput);
  return status;
}

StatusResult SQLProcessor::showQuery(std::ostream &anOutput, std::string tableName, Query& aQuery, Filters& aFilter, JoinList& aJoinList){

  //retrieve the entity of table tableName in the current database
  Entity anEntity;
  std::vector<Entity> theJoinEntityList;
  if(db -> getEntity(anEntity, tableName)) {
    std::vector<Attribute> theJoinAttributesList;
    if(aJoinList.size()) {
      for(auto join : aJoinList) {
        Entity theJoinEnity;
        if(!(db -> getEntity(theJoinEnity, join.table)))
          return StatusResult(Errors::unknownTable);
        else {
          std::vector<Attribute> theSingleJoinAttributesList;
          theJoinEnity.getAttributeList(theSingleJoinAttributesList);
          theJoinAttributesList.insert(theJoinAttributesList.end(), theSingleJoinAttributesList.begin(), theSingleJoinAttributesList.end());
          theJoinEntityList.push_back(theJoinEnity);
        }
      }
      aQuery.setJoinAttributeList(theJoinAttributesList);
    }
    aQuery.setFrom(&anEntity);
    //validation
    if(!aQuery.validateQuery() || !aFilter.validateAll(anEntity))
      return StatusResult(Errors::invalidAttribute);
  }
  else
    return StatusResult(Errors::unknownTable);
  
  RowCollection theRows;
  StatusResult status = db->selectRows(aQuery, aFilter, theRows);

  if(aJoinList.size()) {
    RowCollection rightRows;
    switch(aJoinList.at(0).joinType) {
      case(Keywords::left_kw) :
        db -> leftJoin(theRows, rightRows, aJoinList);
        break;
      case(Keywords::right_kw) :
        db -> rightJoin(aQuery, aFilter, aJoinList, rightRows);
        break;
      default: break;
    }
    timer.stop();
    TableView theView(rightRows, aQuery, timer.elapsed());
    theView.show(anOutput);
  } else {
    timer.stop();
    TableView theView(theRows, aQuery, timer.elapsed());
    theView.show(anOutput);
  }
  
  return status;
}

StatusResult SQLProcessor::updateRows(std::ostream &anOutput, std::string tableName, Filters& aFilter, Filters& anUpdatedValues) {
  Entity anEntity;
  if(db -> getEntity(anEntity, tableName)) {
    if(!aFilter.validateAll(anEntity) || !anUpdatedValues.validateAll(anEntity))
      return StatusResult(Errors::invalidAttribute);
  }
  else
    return StatusResult(Errors::unknownTable);
  
  int rowsUpdated = 0;
  StatusResult status = db -> updateRows(anEntity, aFilter, anUpdatedValues, rowsUpdated);
  timer.stop();
  
  QueryView theView(anEntity.getName(), Keywords::update_kw, status, timer.elapsed(), rowsUpdated);
  theView.show(anOutput);
  return status;
  
}

StatusResult SQLProcessor::deleteEntries(std::ostream &anOutput, std::string tableName, Filters& aFilter) {
  Entity anEntity;
  if(db -> getEntity(anEntity, tableName)) {
    if(!aFilter.validateAll(anEntity))
      return StatusResult(Errors::invalidAttribute);
  }
  else
    return StatusResult(Errors::unknownTable);
  
  int rowsDeleted = 0;
  StatusResult status = db -> deleteRows(anEntity, aFilter, rowsDeleted);
  timer.stop();
  
  QueryView theView(anEntity.getName(), Keywords::delete_kw, status, timer.elapsed(), rowsDeleted);
  theView.show(anOutput);
  return status;
}

StatusResult SQLProcessor::showIndexes(std::ostream &anOutput) {
  //Load all entites, make a table with their name and their primary key
  EntityCollection entities;
  StringList tableNames;
  if(db -> getAllEntities(entities, tableNames)) {
    timer.stop();
    indexesView theView(timer.elapsed(), Keywords::indexes_kw);
    theView.setEntities(entities);
    theView.show(anOutput);
    return StatusResult();
  }
  return StatusResult(Errors::unknownError);
}

StatusResult SQLProcessor::showIndex(std::ostream &anOutput, std::string aTableName, StringList aFieldsVector){
  Entity theEntity;
  if(nullptr == db)
    return StatusResult(Errors::noDatabaseSpecified);
  if(db -> getEntity(theEntity, aTableName)) {
    if(!theEntity.validateIndexFields(aFieldsVector))
      return StatusResult(Errors::invalidAttribute);
  }
  else
    return StatusResult(Errors::unknownTable);
  
  //Load index data from table and create view (index location is in Entity object)
  std::map<IndexKey, uint32_t> data;
  StatusResult status = db -> getIndexData(data, theEntity);
  timer.stop();
  indexesView theView(timer.elapsed(), Keywords::index_kw);
  theView.setIndexes(data);
  theView.show(anOutput);
  return status;
}

StatusResult SQLProcessor::alterTable(std::ostream &anOutput, Keywords subCmd, std::string aTableName, AttributeList attributes) {
  Entity anEntity;
  StatusResult status;
  if( db -> getEntity(anEntity, aTableName)) {
    status = db -> changeEntity(subCmd, anEntity, attributes);
  }
  
  QueryView theView(anEntity.getName(), Keywords::alter_kw, status, timer.elapsed(), 0);
  theView.show(anOutput);
  return StatusResult();
}



StatusResult SQLProcessor::run(Statement* aStatement, const Timer& theTimer) {
  timer = theTimer;

  switch(aStatement->getType()) {
    case Keywords::create_kw: return createTable(output, static_cast<CreateTableStatement*>(aStatement) -> getEntity());
    case Keywords::drop_kw: return dropTable(output, aStatement->getName());
    case Keywords::show_kw: return showTables(output);
    case Keywords::describe_kw: return describeEntity(output, aStatement->getName());
    case Keywords::insert_kw: return insertRows(output, aStatement -> getName(), static_cast<InsertTableStatement*>(aStatement) -> getAttributes(), static_cast<InsertTableStatement*>(aStatement) -> getRows());
    case Keywords::select_kw: return showQuery(output,aStatement -> getName(), static_cast<SelectTableStatement*>(aStatement) -> getQuery() , static_cast<SelectTableStatement*>(aStatement) -> getFilter(),
        static_cast<SelectTableStatement*>(aStatement) -> getJoins());
    case Keywords::update_kw: return updateRows(output, aStatement -> getName(), static_cast<UpdateTableStatement*>(aStatement) -> getFilter(), static_cast<UpdateTableStatement*>(aStatement) -> getUpdatedValues());
    case Keywords::delete_kw: return deleteEntries(output, aStatement -> getName(), static_cast<DeleteTableStatement*>(aStatement) -> getFilter());
    case Keywords::indexes_kw: return showIndexes(output);
    case Keywords::index_kw: return showIndex(output, aStatement -> getName(), static_cast<ShowIndexStatement*>(aStatement) -> getFields());
    case Keywords::alter_kw: return alterTable(output, static_cast<AlterTableStatement*>(aStatement) -> getSubCmd(), aStatement -> getName(), static_cast<AlterTableStatement*>(aStatement) -> getAttributes());
    default: break;
  }
  return StatusResult{Errors::noError};
}
Statement* createStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new CreateTableStatement(theKeyword);
}

Statement* showStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  Keywords theNextKeyword = aTokenizer.peek(1).keyword;
  if(theNextKeyword == Keywords::indexes_kw)
    return new ShowIndexesStatement(theNextKeyword);
  else if(theNextKeyword == Keywords::index_kw)
    return new ShowIndexStatement(theNextKeyword);
  return new ShowTableStatement(theKeyword);
}

Statement* dropStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new DropTableStatement(theKeyword);
}

Statement* describeStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new DescribeTableStatement(theKeyword);
}

Statement* insertStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new InsertTableStatement(theKeyword);
}

Statement* selectStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new SelectTableStatement(theKeyword);
}

Statement* updateStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new UpdateTableStatement(theKeyword);
}

Statement* deleteStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new DeleteTableStatement(theKeyword);
}

Statement* dumpStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new DumpTableStatement(theKeyword);
}

Statement* alterStatementFactory(Tokenizer& aTokenizer) {
  Keywords theKeyword = aTokenizer.peek(0).keyword;
  return new AlterTableStatement(theKeyword);
}


// USE: retrieve a statement based on given text input...
Statement* SQLProcessor::makeStatement(Tokenizer &aTokenizer) {
  static std::map<Keywords, StmtFactory> factories ={
    {Keywords::create_kw, createStatementFactory},
    {Keywords::show_kw, showStatementFactory},
    {Keywords::drop_kw, dropStatementFactory},
    {Keywords::describe_kw, describeStatementFactory},
    {Keywords::insert_kw, insertStatementFactory},
    {Keywords::select_kw, selectStatementFactory},
    {Keywords::update_kw, updateStatementFactory},
    {Keywords::delete_kw, deleteStatementFactory},
    {Keywords::dump_kw, dumpStatementFactory},
    {Keywords::alter_kw, alterStatementFactory}
  };
  
  if(aTokenizer.size()) {
    if(factories.count(aTokenizer.current().keyword))
      if(Statement *newStatement = factories[aTokenizer.current().keyword](aTokenizer))
        if(StatusResult parseResult = newStatement -> parse(aTokenizer))
          return newStatement;
  }
  return nullptr;
}


}
