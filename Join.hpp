//
//  Join.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/05/21.
//

#ifndef Join_h
#define Join_h

#include <string>
#include <vector>
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "keywords.hpp"
#include "Filters.hpp"


namespace ECE141 {

  class Join  {
  public:
    Join(const std::string &aTable, Keywords aType, Operand anOp1, Operand anOp2)
      : table(aTable), joinType(aType), expr(anOp1, Operators::equal_op, anOp2) {}
            
    std::string table;
    Keywords    joinType;
    Expression  expr;
  };

  using JoinList = std::vector<Join>;

}

#endif /* Join_h */
