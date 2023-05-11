//
//  SQLProcessor.hpp
//  RGAssignment3
//
//  Created by rick gessner on 4/1/21.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include <sstream>
#include <string>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "TableStatements.hpp"
#include "Database.hpp"
#include "Filters.hpp"
#include "Timer.hpp"
#include "Index.hpp"

namespace ECE141 {

  //class Statement;

  class SQLProcessor : public CmdProcessor {
  public:
    
    //STUDENT: Declare OCF methods...
    SQLProcessor(std::ostream &anOutput, Database* db);
    ~SQLProcessor();
    
    bool isKnown(Keywords aKeyword);
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer) override;
    StatusResult  run(Statement *aStmt, const Timer& aTimer) override;
    
    StatusResult createTable(std::ostream &anOutput, Entity anEntity);
    StatusResult showTables(std::ostream &anOutput);
    StatusResult dropTable(std::ostream &anOutput, std::string tableName);
    StatusResult describeEntity(std::ostream &anOutput, std::string tableName);
    StatusResult insertRows(std::ostream &anOutput, std::string tableName, StringList attributeNames, RowCollection& newRows);
    StatusResult showQuery(std::ostream &anOutput, std::string tableName, Query& aQuery, Filters& aFilter, JoinList& aJoinList);
    StatusResult updateRows(std::ostream &anOutput, std::string tableName, Filters& aFilter, Filters& anUpdatedValues);
    StatusResult deleteEntries(std::ostream &anOutput, std::string tableName, Filters& aFilter);
    
    StatusResult showIndexes(std::ostream &anOutput);
    StatusResult showIndex(std::ostream &anOutput, std::string aTableName, StringList aFieldsVector);

    StatusResult alterTable(std::ostream &anOutput, Keywords subCmd, std::string aTableName, AttributeList attributes);

    void setDB(Database* aDB);
    
  protected:
    CmdProcessor *dbproc;
    Database* db;
    Timer timer;
  };

}

#endif /* SQLProcessor_hpp */
