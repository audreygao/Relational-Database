//
//  TableDescriptionView.cpp
//  assignment3
//
//  Created by Ya Gao
//

#include "TableDescriptionView.hpp"

namespace ECE141 {

void TableDescriptionView::setEntity(Entity const & anEntity) {theEntity = anEntity;}
TableDescriptionView::TableDescriptionView(double aTime, Keywords aKeyword, bool status) :
  time(aTime), keyword(aKeyword), success(status){}

  bool TableDescriptionView::show(std::ostream &aStream){
    if( success ) {
      AttributeList attributes;
      theEntity.getAttributeList(attributes);
      std::string rowCase = (attributes.size() == 1) ? " row" : " rows";
      aStream << "+---------------+--------------+----------+-------+---------+----------------------+\n";
      aStream << "|     Field     |     Type     | Not Null |  Key  | Default | Extra                |\n";
      aStream << "+---------------+--------------+----------+-------+---------+----------------------+\n";
      
      for(auto row:attributes) {
        aStream << "| " << std::left << std::setw(14) << row.getName();
        std::string size = "";
        
        std::string str = Helpers::dataTypeToString(row.getType());
        if(row.getType()==DataTypes::varchar_type) {
          str = std::string(Helpers::dataTypeToString(row.getType())) + "(" + std::to_string(row.getSize()) + ")";
        }
        aStream << "| " << std::left << std::setw(13) << str;

        aStream << "| " << std::left << std::setw(9) << nullStr[row.getNotNull()];
        aStream << "| " << std::left << std::setw(6) << keyStr[row.getPrim()];
        aStream << "| " << std::left << std::setw(8) << "NULL";
        aStream << "|                      |\n";
      }
      aStream << "+---------------+--------------+----------+-------+---------+----------------------+\n";
      aStream << attributes.size() << rowCase << " in set " << "(" << std::setprecision(2) << time << " sec)\n";
    } else {
      aStream << "ERROR: Table does not exist.\n";
    }
    return true;
  }

}
