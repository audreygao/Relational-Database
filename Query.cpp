//
//  Query.cpp
//  Assignment5
//
//  Created by rick gessner on 4/26/21.
//

#include "Query.hpp"
#
namespace ECE141 {
Query::Query(): _from(nullptr), all(false){}
  //implement your query class here...
  Query& Query::setSelectAll(bool aState) {
    all = aState;
    return *this;
  }
  
  Query& Query::setSelect(const StringList &aFields) {
    selectFields = aFields;
    return *this;
  }
      
  Entity* Query::getFrom() const {
    return _from;
  }

  Query& Query::setFrom(Entity *anEntity) {
    _from = anEntity;
    return *this;
  }

  Query& Query::setOffset(int anOffset) {
    return *this;
  }
  Query& Query::setLimit(int aLimit) {
    limit = aLimit;
    return *this;
  }
  int Query::getLimit() {
    return limit; 
  }

  Query& Query::orderBy(const std::string &aField, bool ascending) {
    orderField = aField;
    return *this;
  }

  std::string Query::getOrderField() {
    return orderField;
  }

  bool Query::validateQuery() {
    if(_from != nullptr) {
      //If query is all fields, return true
      if(all)
        return true;
      //Else, have Entity validate the selectFields list and orderField
      return (_from -> validateFields(selectFields,orderField, joinAttributes));
    }
    return false;
  }

  bool Query::selectAll() const {
    return all;
  }

  StringList Query::getSelect() {
    return selectFields;
  }

void Query::setJoinAttributeList(std::vector<Attribute> theAttributeList) {
  for(auto attribute : theAttributeList){
    joinAttributes.push_back(attribute.getName());
  }
}


}
