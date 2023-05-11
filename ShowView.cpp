//
//  ShowView.cpp
//  assignment2
//
//  Created by Allen Munk on 4/10/21.
//

#include "ShowView.hpp"

namespace ECE141 {

ShowView::ShowView(StringList& nameList, double aTime, Keywords aKeyword): names(nameList), time(aTime), keyword(aKeyword){}

  bool ShowView::show(std::ostream &aStream){
    std::string rowCase = (names.size() == 1) ? " row" : " rows";
    if(keyword == Keywords::database_kw)
      aStream << "+--------------------+\n| Database           |\n+--------------------+\n";
    else if(keyword == Keywords::table_kw)
      aStream << "+--------------------+\n| Tables_in_mydb     |\n+--------------------+\n";

    for(auto name:names) {
      aStream << "| " << std::left << std::setw(18) << name << " |\n";
    }
    aStream << "+--------------------+\n";
    aStream << names.size() << rowCase << " in set " << "(" << std::setprecision(2) << time << " sec)\n";
    return true;
  }

}
