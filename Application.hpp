//
//  AppProcessor.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "DBProcessor.hpp"

namespace ECE141 {

  class Application : public CmdProcessor {
  public:
    
    Application(std::ostream &anOutput);
    virtual ~Application();

    virtual StatusResult  handleInput(std::istream &anInput);
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer) override;
    StatusResult  run(Statement *aStmt, const Timer& theTimer) override;

    
  protected:
    DBProcessor dbProcessor;
  };
  
}

#endif /* Application_hpp */
