//
//  Entity.hpp
//
//  Created by rick gessner on 4/03/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//


#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>

#include "Attribute.hpp"
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "Row.hpp"
//#include "Storable.hpp"

namespace ECE141 {

  //------------------------------------------------

  class Entity : public Storable  {
  public:

    static uint32_t       hashString(const char *str);
    static uint32_t       hashString(const std::string &aStr);

      //declare ocf methods...
      
      //declare methods you think are useful for entity...
    Entity();
    void            setName(std::string aName);
    std::string     getName();
    
    void            addAttribute(Attribute anAttribute);
    bool            removeAttribute(std::string attributeName);
    void            getAttributeList(AttributeList & aCopy);
    StringList      getRequiredAttributes(AttributeList attributeList);
    StringList      getAttributeNamesList(AttributeList attributeList);
    Attribute       getAttributeAt(int anIndex);
    Attribute*      getAttribute(std::string aString);
    
    bool            validateRows(StringList attributeNames, RowCollection& rows);
    bool            validateFields(StringList attributeNames, std::string orderName, StringList joinAttributes);
    bool            validateIndexFields(StringList attributeNames);
    
    void            setBlockNum(uint32_t aBlockNum);
    uint32_t        getBlockNum();
    uint32_t        getIndexBlockNum();
    std::string     getPrimaryKey();

    
      //this is the storable interface...
    StatusResult          encode(std::ostream &aWriter) override;
    StatusResult          decode(std::istream &aReader) override;
           
  protected:
    bool hasIncId(std::string& anAttributeName);
    void setId(RowCollection& rows, const std::string anAttributeName);
    AttributeList   attributes;
    std::string     name;
    uint32_t        blockNum;
    uint32_t        indexBlock;
    int             incId;
    
    //surely there must be other data members?
  };
  using EntityCollection = std::vector<std::unique_ptr<Entity>>;
  
}
#endif /* Entity_hpp */
