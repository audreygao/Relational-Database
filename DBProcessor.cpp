//
//  DBPRocessor.cpp
//  assignment2
//
//  Created by Allen Munk on 4/9/21.
//

#include <stdio.h>
#include "DBProcessor.hpp"
#include "Database.hpp"
#include "Timer.hpp"
#include "iomanip"
#include <filesystem>
#include "Config.hpp"
#include <algorithm>
#include "QueryView.hpp"
#include "ShowView.hpp"
#include "DumpView.hpp"

namespace ECE141 {
DBProcessor::DBProcessor(std::ostream &anOutput) : CmdProcessor(anOutput), db(nullptr), sqlProcessor(anOutput, nullptr){}

DBProcessor::~DBProcessor() {
    //if(database) delete database;
  }

// USE: -----------------------------------------------------

bool DBProcessor::isKnown(Keywords aKeyword) {
  static Keywords theKnown[]={Keywords::create_kw, Keywords::drop_kw, Keywords::show_kw, Keywords::use_kw, Keywords::dump_kw};
  auto theIt = std::find(std::begin(theKnown),
                         std::end(theKnown), aKeyword);
  return theIt!=std::end(theKnown);
}

bool isCreate(Tokenizer& aTokenizer){
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::create_kw).then(Keywords::database_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isDrop(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::drop_kw).then(Keywords::database_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isShow(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::show_kw).then(Keywords::databases_kw);
  aTokenizer.restart();
  return returnBool;
}

bool isUse(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::use_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}

bool isDump(Tokenizer& aTokenizer) {
  TokenSequence theTokenSequence(aTokenizer);
  std::string theName;
  bool returnBool = TokenSequence::Term(theTokenSequence).is(Keywords::dump_kw).then(Keywords::database_kw).thenId(theName);
  aTokenizer.restart();
  return returnBool;
}


CmdProcessor* DBProcessor::recognizes(Tokenizer &aTokenizer) {
  if(isKnown(aTokenizer.current().keyword)) {
    if(isCreate(aTokenizer) || isDrop(aTokenizer) || isShow(aTokenizer) || isUse(aTokenizer) || isDump(aTokenizer))
      return this;
    aTokenizer.restart();
  }
  return sqlProcessor.recognizes(aTokenizer);
}


StatusResult DBProcessor::create(std::ostream &anOutput, const std::string aName) {

  try {
    //if a db is open, close it
      if (db) {
        delete db;
        db = nullptr;
      }
    db = new Database(aName, CreateDB());
    sqlProcessor.setDB(db);
  } catch(int e) {
    return StatusResult(Errors::databaseCreationError);
  }
  timer.stop();
  
  QueryView theView(aName, Keywords::create_kw, true, timer.elapsed(), 1);
  theView.show(anOutput);
  return StatusResult();
}

StatusResult DBProcessor::drop(std::ostream &anOutput, const std::string aName) {
  bool success  = false;
  int numTables = 0;

  try {
    if(std::filesystem::exists(Config::getDBPath(aName))) {
      
      //change the database in use to the database to drop
      if (db) {
        delete db;
        db = nullptr;
      }
      db = new Database(aName, OpenDB());
      sqlProcessor.setDB(db);      
      success = true;

      // load the table names in this database
      EntityCollection entities;
      StringList tableNames;
      db -> getAllEntities(entities, tableNames);
      numTables = tableNames.size();

      // drop each table in this database
      for(auto table: tableNames) {
        int rowsDeleted;
        db -> dropTable (table, rowsDeleted);
      }

      //remove the database file
      delete db;
      db = nullptr;
      std::filesystem::remove(Config::getDBPath(aName));
    }
  } catch(int e) {
    return StatusResult(Errors::unknownError);
  }

  timer.stop();
  
  QueryView theView(aName, Keywords::drop_kw, success, timer.elapsed(), numTables);
  theView.show(anOutput);
  return success ? StatusResult(): StatusResult(Errors::unknownDatabase);
}

StatusResult DBProcessor::show(std::ostream &anOutput) {
  StringList dbNames;

  try {
    //iterate through all databases with .db ext and record the database name
    for(const auto & file : std::filesystem::directory_iterator(Config::getStoragePath())) {
      if(std::string(file.path()).find(".db") != std::string::npos) {
        std::string fileName = std::string(file.path());
        fileName.erase(0, strlen(Config::getStoragePath()) + 1);
        fileName.erase(fileName.length() - strlen(Config::getDBExtension()), strlen(Config::getDBExtension()));
        dbNames.push_back(fileName);
      }
    }
  } catch(int e) {
    return StatusResult(Errors::unknownError);
  }

  timer.stop();

  ShowView theView(dbNames, timer.elapsed(), Keywords::database_kw);
  theView.show(anOutput);
  return StatusResult();
}

StatusResult DBProcessor::use(std::ostream &anOutput, const std::string aName) {
  bool success  = false;
  
  try {

    //check if the database exists
    success = std::filesystem::exists(Config::getDBPath(aName));
    if(success) {

      //if a db is open, close it
      if (db) {
        delete db;
        db = nullptr;
      }
       db = new Database(aName, OpenDB());
       sqlProcessor.setDB(db);
    }
  } catch(int e) {
    return StatusResult(Errors::unknownError);
  }
  QueryView theView(aName, Keywords::use_kw, success);
  theView.show(anOutput);
  return success ? StatusResult(): StatusResult(Errors::unknownDatabase);
}

StatusResult DBProcessor::dump(std::ostream &anOutput, const std::string aName ) {
  bool success = false;
  std::vector<char> types;

  try {
    //see if the file aName exists
    success = std::filesystem::exists(Config::getDBPath(aName));
    if(success) {
      //if a db is open, close it
      if (db) {
        delete db;
        db = nullptr;
      }
       db = new Database(aName, OpenDB());
       db->dump(types);
    }
  } catch(int e) {
    return StatusResult(Errors::unknownError);
  }
  timer.stop();

  //dump the type of each block in the database
  DumpView dumpView(types, success, timer.elapsed());
  dumpView.show(anOutput);
  return success ? StatusResult(): StatusResult(Errors::unknownDatabase);
}

StatusResult DBProcessor::run(Statement* aStatement,  const Timer& theTimer) {
  timer = theTimer;
  switch(aStatement->getType()) {
    case Keywords::create_kw: return create(output, aStatement -> getName());
    case Keywords::drop_kw: return drop(output, aStatement -> getName());
    case Keywords::show_kw: return show(output);
    case Keywords::use_kw: return use(output, aStatement -> getName());
    case Keywords::dump_kw: return dump(output, aStatement -> getName());
    default: break;
  }
  return StatusResult{Errors::noError};
}
  
// USE: retrieve a statement based on given text input...
Statement* DBProcessor::makeStatement(Tokenizer &aTokenizer) {
  Token theToken=aTokenizer.current();
  if (isKnown(theToken.keyword)) {
    aTokenizer.next(); //skip ahead...

    //create/drop/use/dump DATABASE databaseName
    if(aTokenizer.current().keyword == Keywords::database_kw) {
      if(aTokenizer.next())
        return new Statement(theToken.keyword, aTokenizer.current().data);
    }

    //use databaseName
    else {
      return new Statement(theToken.keyword, aTokenizer.current().data);
    }
  }

  return nullptr;
}


}
