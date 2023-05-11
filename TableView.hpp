
//
//  TableView.hpp
//

#ifndef TableView_h
#define TableView_h

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include "TabularView.hpp"
#include "Row.hpp"
#include "Query.hpp"

namespace ECE141 {

  // USE: general tabular view (with columns)
  class TableView : public TabularView {
  public:
    
    //STUDENT: Add OCF methods...
    TableView(RowCollection &aRows, Query& aQuery, double aTime);
    bool show(std::ostream &anOutput);
    
  protected:
    //what data members do you require?

    RowCollection rows;
    Query query;
    StringList fields;
    double time;
  };

}

#endif /* TableView_h */
