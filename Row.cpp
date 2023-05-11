//
//  Row.cpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Row.hpp"
#include "Database.hpp"
#include <sstream>

namespace ECE141 {

  template <typename T>
  char vtype(const T& aVar) {
    static char theChars[]="bids";
    return theChars[aVar.index()];
  }

  std::ostream& operator<<(std::ostream &aStream, const Value &aValue) {
    std::visit([&aStream](auto const & aValue) {
      aStream << " " << aValue;
    }, aValue);
    return aStream;          
  }

  std::istream& operator>>(std::istream &aStream, const Value &aValue) {
    return aStream;
  }

  StatusResult Row::encode(std::ostream &aWriter) {
    for(auto att: attributeList) {
      aWriter << vtype(data[att.getName()]);
      if( vtype(data[att.getName()]) == 's') {
        int size = std::get<std::string>(data[att.getName()]).length();
        aWriter << " " << size << " ";
      }
      aWriter << data[att.getName()] << " ";
    }
    return StatusResult();
  }

  StatusResult Row::decode(std::istream &aReader) {
    for(int i = 0; i < attributeList.size(); i++) {
      char c;
      Value val;
      aReader >> c;
      switch (c) {
        case 'b':
          bool valB;
          aReader >> valB;
          val = Value{valB};
          break;
        case 'i':
          int valI;
          aReader >> valI;
          val = Value{valI};
        break;
        case 'd':
          float valD;
          aReader >> valD;
          val = Value{valD};
        break;
        case 's':
          int length;
          aReader >> length;
          std::string valS;
          aReader >> valS;
          while(valS.length() < length) {
            valS += " ";
            std::string str;
            aReader >> str;
            valS += str;
          }
          val = Value{valS};
        break;
      } 
      addValue(attributeList.at(i).getName(), val);
    }
    return StatusResult();
  }
  
  void Row::initBlock(Block &aBlock) {
    
  }

  void Row::addValue(std::string identifier, Value aValue) {
    data[identifier] = aValue;
  }

  void Row::removeValue(std::string identifier) {
    data.erase(identifier);
  }

  Value Row::getValue(std::string aKey) {
    return data[aKey];
  }

  void  Row::getKeyValues(KeyValues &aCopy) {
    for(KeyValues::iterator it = data.begin(); it != data.end(); it++) {
      aCopy[it->first] = it -> second;
    }
  }

  void Row::setKeyValues(KeyValues &aCopy) {
    for(KeyValues::iterator it = aCopy.begin(); it != aCopy.end(); it++) {
      data[it->first] = it -> second;
    }
  }

void Row::addKeyValues(KeyValues &aCopy) {
  for(KeyValues::iterator it = aCopy.begin(); it != aCopy.end(); it++) {
    addValue(it->first, it->second);
  }
}

bool Row::hasKey(std::string aKey) {
  return data.find(aKey) != data.end();
}


int Row::getDataSize() {
  return data.size();
}

  void Row::setAttributeList(AttributeList att) {
    attributeList = att;
  }

StringList        Row::getAttributeNamesList() {
  StringList vec;
  for(auto& att: attributeList) {
    vec.push_back(att.getName());
  }
  return vec;
}

  void Row::setBlockNumber(uint32_t aBlockNumber){
    blockNumber = aBlockNumber;
  }

  uint32_t Row::getBlockNumber() {
    return blockNumber;
  }

}
