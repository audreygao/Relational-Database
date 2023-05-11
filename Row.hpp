//
//  Row.hpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp


#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include <functional>
#include "Storage.hpp"
#include "Attribute.hpp"

class Database;

namespace ECE141 {

  class Row : public Storable {
  public:

    //STUDENT declare OCF methods...
      Row() {};
    ~Row() {};
      //storable api just in case...
    StatusResult        encode(std::ostream &aWriter) override;
    StatusResult        decode(std::istream &aReader) override;
    void                initBlock(Block &aBlock) ;
    
    void                addValue(std::string identifier, Value aValue);
    void                removeValue(std::string identifier);
    Value               getValue(std::string aKey);
    int getDataSize();
  
    bool                hasKey(std::string aKey);
    
    void                getKeyValues(KeyValues &aCopy);
    void                setKeyValues(KeyValues &aCopy);
    void                addKeyValues(KeyValues &aCopy);
    
    void                setAttributeList(AttributeList att);
    StringList          getAttributeNamesList();
    
    void                setBlockNumber(uint32_t aBlockNumber);
    uint32_t            getBlockNumber();

  protected:
    KeyValues           data;
    uint32_t            blockNumber;
    AttributeList       attributeList;
    //do you need any other data members?
    
  };

  //-------------------------------------------
  using RowCollection = std::vector<std::unique_ptr<Row>>;
  std::ostream& operator<<(std::ostream &aStream, const Value &aValue);

}

#endif /* Row_hpp */
