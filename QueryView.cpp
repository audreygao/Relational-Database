//
//  QueryView.cpp
//  assignment2
//
//  Created by Allen Munk on 4/10/21.
//

#include "QueryView.hpp"

namespace ECE141 {
  
  QueryView::QueryView(std::string aName, Keywords aKeyword, bool success, double aTime, int rows) : name(aName), command(aKeyword), success(success), time(aTime), rows(rows){}
  
  bool QueryView::show(std::ostream &aStream) {
    if(success) {
      if(command==Keywords::use_kw) {
        aStream << "Database changed\n";
      } else {
        std::string rowCase = (rows == 1) ? " row" : " rows";
        aStream << "Query OK, "<< rows << rowCase <<  " affected " << "(" << std::setprecision(2) << time << ")\n";
      }
      return true;
    }
    switch (command) {
      case Keywords::drop_kw:
        aStream << "ERROR 1008 (HY000): Can't drop database '"<< name <<"'; database doesn't exist\n";
        break;
      case Keywords::use_kw:
        aStream << "ERROR 1008 (HY000): Can't use database '"<< name <<"'; database doesn't exist\n";
        break;
      default:
        break;
    }
    return false;
  }

}



