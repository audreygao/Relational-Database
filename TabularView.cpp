//
//  TabularView.cpp
//  hw9
//
//  Created by Gracya Gao on 6/4/21.
//

#include "TabularView.hpp"

namespace ECE141 {

  void TabularView::printDivider(StringList fields, std::ostream &anOutput) {
    std::string str (25 * fields.size() - 1, '-');
    anOutput << "+" << str << "+\n";
  }


}

