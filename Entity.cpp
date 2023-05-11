//
//  Entity.cpp
//
//  Created by rick gessner on 4/03/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include <sstream>
#include "Entity.hpp"


namespace ECE141 {


Entity::Entity() : incId(0), indexBlock(0){}

  const int gMultiplier = 37;

  //hash given string to numeric quantity...
  uint32_t Entity::hashString(const char *str) {
    uint32_t h{0};
    unsigned char *p;
    for (p = (unsigned char*)str; *p != '\0'; p++)
      h = gMultiplier * h + *p;
    return h;
  }

  uint32_t Entity::hashString(const std::string &aStr) {
    return hashString(aStr.c_str());
  }
  
  void Entity::setName(std::string aName) {
    name = aName;
  }

  std::string Entity::getName(){
    return name;
  }

  void Entity::addAttribute(Attribute anAttribute) {
    attributes.push_back(anAttribute);
  }

bool Entity::removeAttribute(std::string attributeName) {
  for(int i = 0; i < attributes.size(); i++) {
    if( attributes.at(i).getName() == attributeName ) {
      attributes.erase(attributes.begin() + i);
      return true;
    }
  }
  return false;
}

  void Entity::getAttributeList(AttributeList & aCopy) {
    aCopy = attributes;
  }

  Attribute Entity::getAttributeAt(int anIndex){
    return attributes[anIndex];
  }

  Attribute* Entity::getAttribute(std::string aString) {
    Attribute* returnAttribute = nullptr;
    for(AttributeList::iterator it = attributes.begin(); it != attributes.end(); it++) {
      if((*it).getName() == aString) {
        returnAttribute = &(*it);
      }
    }
    return returnAttribute;
  }


StringList Entity::getRequiredAttributes(AttributeList attributes) {
  StringList returnVector;
  for(AttributeList::iterator attributeIt = attributes.begin(); attributeIt != attributes.end(); attributeIt++) {
    if((*attributeIt).getNotNull() && !(*attributeIt).getInc())
      returnVector.push_back((*attributeIt).getName());
  }
  return returnVector;
}

StringList Entity::getAttributeNamesList(AttributeList attributeList) {
  StringList returnVector;
  for(AttributeList::iterator attributeIt = attributeList.begin(); attributeIt != attributeList.end(); attributeIt++) {
    returnVector.push_back((*attributeIt).getName());
  }
  return returnVector;
}

std::map<std::string,DataTypes> getAttributeTypes(AttributeList attributeList) {
  std::map<std::string,DataTypes> returnMap;
  for(AttributeList::iterator attributeIt = attributeList.begin(); attributeIt != attributeList.end(); attributeIt++) {
    returnMap[(*attributeIt).getName()] = ((*attributeIt).getType());
  }
  return returnMap;
}

  bool Entity::validateRows(StringList attributeNames, RowCollection& rows) {
    static std::map<DataTypes,int> valueTypes= {{ DataTypes::bool_type,0},{DataTypes::datetime_type,3},{DataTypes::float_type,2}, {DataTypes::varchar_type, 3}, {DataTypes::int_type, 1}};
    StringList allAttributes, requiredAttributes;
    allAttributes = getAttributeNamesList(attributes);

    requiredAttributes = getRequiredAttributes(attributes);

    //Make sure the attributes from command 1. Exist 2. All non null elements are present
    for(StringList::iterator nameIt = attributeNames.begin(); nameIt != attributeNames.end(); nameIt++) {
      //If Attribute is valid
      if(std::find(allAttributes.begin(),allAttributes.end(),*nameIt) != allAttributes.end()) {
        StringList::iterator requiredIndex = std::find(requiredAttributes.begin(), requiredAttributes.end(), *nameIt);
        //If Attribute is non null, remove from list of non null attributes
        if(requiredIndex != requiredAttributes.end()) {
          requiredAttributes.erase(requiredIndex);
        }

      }
      else
        return false;
    }

    //return false if there are non null attributes missing
    if(requiredAttributes.size())
      return false;
    
    std::map<std::string,DataTypes> attributeTypes = getAttributeTypes(attributes);
    //Check to make sure each attribute in a new row is the correct type
    for(RowCollection::iterator rowIt = rows.begin(); rowIt != rows.end(); rowIt++) {
      size_t rowAttributeIndex = 0;
      while(rowAttributeIndex < attributeNames.size()) {
        std::string currentAttribute = attributeNames[rowAttributeIndex];
        //If row value is wrong type then invalid insertion
        if(valueTypes[attributeTypes[currentAttribute]] != (*rowIt) -> getValue(currentAttribute).index())
          return false;
        rowAttributeIndex += 1;
      }
    }
    
    std::string theAttributeName;
    if(hasIncId(theAttributeName)) {
      setId(rows, theAttributeName);
    }
    
    return true;
  }

void Entity::setId(RowCollection& rows, const std::string anAttributeName){
  for(auto& row : rows) {
    incId += 1;
    row -> addValue(anAttributeName, incId);
  }
  
}


  bool Entity::validateFields(StringList attributeNames, std::string orderName, StringList joinAttributes){
    StringList allAttributes = getAttributeNamesList(attributes);
    if(joinAttributes.size())
      allAttributes.insert(allAttributes.end(), joinAttributes.begin(), joinAttributes.end());
    for(StringList::iterator nameIt = attributeNames.begin(); nameIt != attributeNames.end(); nameIt++) {
      //If an attribute name does not exist, query is not valid
      if(std::find(allAttributes.begin(),allAttributes.end(),*nameIt) == allAttributes.end())
        return false;
      }
      if(orderName.size() && std::find(allAttributes.begin(),allAttributes.end(),orderName) == allAttributes.end())
        return false;
      
    return true;
  }

  bool Entity::hasIncId(std::string& anAttributeName) {
    bool theResult = false;
    for(auto attribute : attributes) {
      if(attribute.getPrim() && attribute.getInc()) {
        anAttributeName = attribute.getName();
        theResult = true;
      }
    }
    return theResult;
  }

std::string Entity::getPrimaryKey() {
  std::string theKey = "";
  for(auto attribute : attributes) {
    if(attribute.getPrim()) {
      theKey = attribute.getName();
    }
  }
  return theKey;
}



  
  StatusResult Entity::encode(std::ostream &aWriter) {
    StatusResult status;
    aWriter << name << " " <<attributes.size();
    for(AttributeList::iterator it = attributes.begin(); it != attributes.end(); it++)
      status = (*it).encode(aWriter);
    aWriter << " " << incId;
    if(indexBlock)
      aWriter << " " << indexBlock;
    return status;
  }

  StatusResult Entity::decode(std::istream &aReader) {
    StatusResult status;
    int size;
    aReader >> name >> size;
    Attribute anAttribute;
    for(int i = 0; i < size; i++) {
      status = anAttribute.decode(aReader);
      attributes.push_back(anAttribute);
    }
    aReader >> incId;
    aReader >> indexBlock;
    return status;
  }

  void Entity::setBlockNum(uint32_t aBlockNum) {
    blockNum = aBlockNum;
  }

  uint32_t Entity::getBlockNum() {
    return blockNum;
  }
  uint32_t Entity::getIndexBlockNum() {
    return indexBlock;
  }

  //Validate a vector of attribute names to make sure they are indexable
bool Entity::validateIndexFields(StringList attributeNames) {
  bool theResult = true;
  for(auto attributeName : attributeNames) {
    Attribute* theAttribute = getAttribute(attributeName);
    if(!theAttribute -> getPrim())
      theResult = false;
  }
  return theResult;
}




  
}
