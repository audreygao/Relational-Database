//
//  Filters.cpp
//  Datatabase5
//
//  Created by rick gessner on 3/5/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include <string>
#include <limits>
#include <map>
#include "keywords.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Compare.hpp"

namespace ECE141 {
  
  using Comparitor = bool (*)(Value &aLHS, Value &aRHS);

  bool equals(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isEqual(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool lessThan(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isLessThan(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool greaterThan(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isGreaterThan(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  static std::map<Operators, Comparitor> comparitors {
    {Operators::equal_op, equals},
    {Operators::lt_op, lessThan},
    {Operators::gt_op, greaterThan},
    //STUDENT: Add more for other operators...
  };

  bool Expression::operator()(KeyValues &aList) {
    Value theLHS{lhs.value};
    Value theRHS{rhs.value};
    if(TokenType::identifier==lhs.ttype) {
      theLHS=aList[lhs.name]; //get row value
    }

    if(TokenType::identifier==rhs.ttype) {
      theRHS=aList[rhs.name]; //get row value
    }

    return comparitors.count(op)
      ? comparitors[op](theLHS, theRHS) : false;
  }

//  bool Expression::operator()(KeyValues &leftList, KeyValues &rightList) {
//    Value theLHS{""};
//    Value theRHS{""};
//
//    theLHS=leftList[lhs.name]; //get row value
//    theRHS=rightList[rhs.name]; //get row value
//
//    return comparitors.count(op)
//      ? comparitors[op](theLHS, theRHS) : false;
//  }
  
bool Expression::updateValue(KeyValues &aList) {
    std::string attributeName;
    Value newValue;
    if(op == Operators::equal_op) {
        if(TokenType::identifier==lhs.ttype) {
            attributeName = lhs.name;
            newValue = Value{rhs.value};
        }

        if(TokenType::identifier==rhs.ttype) {
            attributeName = rhs.name;
            newValue = Value{lhs.value};
        }
        aList[attributeName] = newValue;
        return true;
    }
    return false;
}

  //--------------------------------------------------------------
  
  Filters::Filters()  {}
  
  Filters::Filters(const Filters &aCopy)  {
  
  }
  
  Filters::~Filters() {
    //no need to delete expressions, they're unique_ptrs!
  }

  Filters& Filters::add(Expression *anExpression) {
    expressions.push_back(std::unique_ptr<Expression>(anExpression));
    return *this;
  }
    
  //compare expressions to row; return true if matches
  bool Filters::matches(KeyValues &aList) const {
    
    //STUDENT: You'll need to add code here to deal with
    //         logical combinations (AND, OR, NOT):
    //         like:  WHERE zipcode=92127 AND age>20
    bool match = true;
    for(auto &theExpr : expressions) {
        switch (theExpr->logic) {
          case Logical::and_op :
            match = match && (*theExpr)(aList);
            break;
          case Logical::or_op :
          match = match || (*theExpr)(aList);
            break;
          case Logical::not_op :
            match = !(*theExpr)(aList);
            break;
          default: match = (*theExpr)(aList);
        }
    }
    return match;
  }
 
bool Filters::updateValuesInList(KeyValues &aList) {
    for(auto &theExpr : expressions) {
        theExpr->updateValue(aList);
    }
    return true;
}

  //where operand is field, number, string...
  StatusResult parseOperand(Tokenizer &aTokenizer,
                            Operand &anOperand) {
    StatusResult theResult{noError};
    Token &theToken = aTokenizer.current();
    if(TokenType::identifier==theToken.type) {
      //Validation is seperated for parsing; all identifiers + strings are treated as strings until validation
      anOperand.name=theToken.data;
      anOperand.ttype=TokenType::string;
      anOperand.dtype=DataTypes::varchar_type;
      anOperand.value=theToken.data;
    }
    else if(TokenType::number==theToken.type) {
      anOperand.ttype=TokenType::number;
      anOperand.dtype=DataTypes::int_type;
      if (theToken.data.find('.')!=std::string::npos) {
        anOperand.dtype=DataTypes::float_type;
        anOperand.value=std::stof(theToken.data);
      }
      else anOperand.value=std::stoi(theToken.data);
    }
    else theResult.error=syntaxError;
    if(theResult) aTokenizer.next();
    return theResult;
  }
    
  
  bool validateOperands(Operand &aLHS, Operand &aRHS, Entity &anEntity) {
    bool theStatus = false;
    if(TokenType::string == aLHS.ttype) {
      if (auto *theAttr=anEntity.getAttribute(aLHS.name)) {
        aLHS.ttype=TokenType::identifier;
        aLHS.entityId=Entity::hashString(aLHS.name);
        aLHS.dtype=theAttr->getType();
        theStatus = true;
      }
    }
    else if(TokenType::string == aLHS.ttype) {
      if (auto *theAttr=anEntity.getAttribute(aRHS.name)) {
        aRHS.ttype=TokenType::identifier;
        aRHS.entityId=Entity::hashString(aRHS.name);
        aRHS.dtype=theAttr->getType();
        theStatus = true;
      }
    }
    return theStatus;
  }

  StatusResult Filters::validateAll(Entity& anEntity) {
    StatusResult  theResult{noError};
    for(Expressions::iterator it = expressions.begin(); it != expressions.end(); it++) {
      if(!validateOperands((*it)->lhs, (*it)->rhs, anEntity))
        theResult.error = Errors::invalidArguments;
    }
    
    return theResult;
    
  }

  StatusResult Filters::parse(Tokenizer &aTokenizer) {
    StatusResult  theResult{noError};

    while(theResult && (2<aTokenizer.remaining())) {
      Operand theLHS,theRHS;
      Token &theToken=aTokenizer.current();
      if(theToken.type!=TokenType::identifier) return theResult;
      if((theResult=parseOperand(aTokenizer,theLHS))) {
        Token &theToken=aTokenizer.current();
        if(theToken.type==TokenType::operators) {
          Operators theOp=Helpers::toOperator(theToken.data);
          aTokenizer.next();
          if((theResult=parseOperand(aTokenizer,theRHS))) {
            add(new Expression(theLHS, theOp, theRHS));
              if(aTokenizer.skipIf(semicolon)) {
                break;
              }
          }
        }
      }
      else theResult.error=syntaxError;
    }
    return theResult;
  }

}

