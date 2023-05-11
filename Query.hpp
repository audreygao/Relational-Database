//
//  Query.hpp
//  Assignment5
//
//  Created by rick gessner on 4/26/21.
//

#ifndef Query_hpp
#define Query_hpp

#include <stdio.h>
#include <string>
#include "Attribute.hpp"
#include "Row.hpp"
#include "Entity.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  class Query  {
  public:
    
    Query();
    
    bool selectAll() const;
    
    Query& setSelectAll(bool aState);
    Query& setSelect(const StringList &aFields);
        
    Entity* getFrom() const;
    Query& setFrom(Entity *anEntity);

    Query& setOffset(int anOffset);
    Query& setLimit(int aLimit);
    int getLimit();

    Query& orderBy(const std::string &aField, bool ascending=false);
    std::string getOrderField();
    
    bool validateQuery();
    
    StringList getSelect();
    
    void setJoinAttributeList(std::vector<Attribute> theAttributeList);

    //how will you handle where clause?
    //how will you handle group by?

    Query& run(RowCollection &aRows);
    
  protected:
    Entity*   _from;
    bool      all;
    int       offset;
    int       limit;
    StringList selectFields;
    std::string orderField;
    StringList joinAttributes;
  };
}
#endif /* Query_hpp */
