//
//  ShowView.hpp
//  assignment2
//
//  Created by Allen Munk on 4/10/21.
//

#ifndef ShowView_hpp
#define ShowView_hpp

#include <stdio.h>
#include <vector>
#include <iomanip>
#include "Helpers.hpp"
#include "View.hpp"

namespace ECE141 {
  class ShowView : public View {
  public:
    ShowView(StringList& nameList, double aTime, Keywords aKeyword);

    bool show(std::ostream &aStream) override;
  private:
    StringList names;
    double time;
    Keywords keyword;
  };
}
#endif /* ShowView_hpp */
