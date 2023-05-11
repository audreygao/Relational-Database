//
//  QueryView.hpp
//  assignment2
//
//  Created by Allen Munk on 4/10/21.
//

#ifndef QueryView_hpp
#define QueryView_hpp

#include <stdio.h>
#include "View.hpp"
#include "Helpers.hpp"
#include <iostream>
#include "iomanip"

namespace ECE141 {
  class QueryView : public View{
  public:
    QueryView( std::string aName, Keywords aKeyword, bool success, double aTime=0, int rows=0);
    bool show(std::ostream &aStream) override;
  private:
    std::string name;
    Keywords command;
    bool success;
    double time;
    int rows;
  };
}

#endif /* QueryView_hpp */
