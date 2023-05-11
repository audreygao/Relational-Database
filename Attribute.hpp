//
//  Attribute.hpp
//
//  Created by rick gessner on 4/02/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include "Storage.hpp"
#include "keywords.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {

  class Attribute : public Storable {
  protected:
    std::string   name;
    DataTypes     type;
    bool          autoIncrement;
    bool          primaryKey;
    bool          notNull;
    int           size;
    
    //what other data members do attributes required?
    
  public:
          
      //STUDENT: declare ocf methods...
    Attribute();
    ~Attribute();
    //Attribute(const Attribute &);
    void setName(std::string aName);
    void setType(DataTypes aType);
    void setSize(int size);
    void setInc();
    void setPrim();
    void setNoNull();
      //What methods do you need to interact with Attributes?
    std::string getName();
    DataTypes getType();
    int getSize();
    bool getInc();
    bool getPrim();
    bool getNotNull();
      
    //Added so t;hat the attribute is a storable...
    StatusResult        encode(std::ostream &aWriter);
    StatusResult        decode(std::istream &aReader);
    
  };
  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;
}


#endif /* Attribute_hpp */
