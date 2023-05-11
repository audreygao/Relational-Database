//
//  Attribute.hpp
//
//  Created by rick gessner on 4/02/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include <iostream>
#include "Attribute.hpp"

namespace ECE141 {

Attribute::Attribute() : name(""), type(DataTypes::no_type), autoIncrement(false), primaryKey(false), notNull(false), size(0){}

Attribute::~Attribute(){}

//Attribute::Attribute(const Attribute& aCopy) : name(aCopy.name), type(aCopy.type), autoIncrement(aCopy.autoIncrement), primaryKey(aCopy.primaryKey), notNull(aCopy.notNull), size(aCopy.size){}

void Attribute::setName(std::string aName) {
  name = aName;
}
  
void Attribute::setType(DataTypes aType) {
  type = aType;
}

void Attribute::setSize(int size) {
  this -> size = size;
}

void Attribute::setInc() {autoIncrement = true;}

void Attribute::setPrim() {primaryKey = true;}

void Attribute::setNoNull() {notNull = true;}

//getter functions
std::string Attribute::getName() {return name;}
DataTypes Attribute::getType() {return type;}
int Attribute::getSize() {return size;}
bool Attribute::getInc() {return autoIncrement;}
bool Attribute::getPrim() {return primaryKey;}
bool Attribute::getNotNull() {return notNull;}

StatusResult        Attribute::encode(std::ostream &aWriter){
  aWriter << " " << name << " " << static_cast<char>(type) << "  ";
  if(type == DataTypes::varchar_type)
    aWriter << size << " ";
  aWriter << autoIncrement << " " << primaryKey << " " << notNull << " "; 
  return StatusResult();
  
} 

StatusResult        Attribute::decode(std::istream &aReader){
  char typeChar;
  aReader >> name >> typeChar;
  type = (DataTypes)typeChar;
  if(type == DataTypes::varchar_type)
    aReader >> size;
  aReader >> autoIncrement >> primaryKey >> notNull;

  return StatusResult();
}

}
