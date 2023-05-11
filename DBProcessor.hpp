//
//  DBProccessor.hpp
//  assignment2
//
//  Created by Allen Munk on 4/9/21.
//

#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Database.hpp"
#include "SQLProcessor.hpp"
#include "TokenSequence.hpp"

namespace ECE141 {

  class DBProcessor : public CmdProcessor {
  public:
    
    DBProcessor(std::ostream &anOutput);
    virtual ~DBProcessor();

    bool isKnown(Keywords aKeyword);

    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer) override;
    StatusResult  run(Statement *aStmt, const Timer& theTimer) override;

    StatusResult create(std::ostream &anOutput, const std::string aName);
    StatusResult drop(std::ostream &anOutput, const std::string aName);
    StatusResult show(std::ostream &anOutput);
    StatusResult use(std::ostream &anOutput, const std::string aName);
    StatusResult dump(std::ostream &anOutput, const std::string aName);

  protected:
    Database* db;
    SQLProcessor sqlProcessor;
    Timer timer;
  };
  
}

#endif /* DBProccessor_h */
