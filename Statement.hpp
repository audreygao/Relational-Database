//
//  Statement.hpp
//  Database
//
//  Created by rick gessner on 3/20/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef Statement_hpp
#define Statement_hpp

#include <iostream>
#include <string>
#include "keywords.hpp"

namespace ECE141 {
  
  class Tokenizer;
  class CmdProcessor;
  
  class Statement {
  public:
    Statement(Keywords aStatementType=Keywords::unknown_kw, std::string name = "");
    Statement(const Statement &aCopy);
    
    virtual               ~Statement();
    
    virtual StatusResult  parse(Tokenizer &aTokenizer);
    
    Keywords              getType() const {return stmtType;}
    
    virtual StatusResult  dispatch() {return StatusResult{};}
    
    std::string           getName() const {return name;}

  protected:
    Keywords   stmtType;
    std::string name;
  };
  
}

#endif /* Statement_hpp */
