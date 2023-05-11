//
//  DumpView.hpp
//  assignment2
//
//  Created by Ya Gao
//

#ifndef DumpView_hpp
#define DumpView_hpp

#include <stdio.h>
#include <vector>
#include <iomanip>
//#include "Helpers.hpp"
#include "View.hpp"

namespace ECE141 {
  class DumpView : public View {
  public:
    DumpView(std::vector<char> types, bool success, double aTime);
    bool show(std::ostream &aStream) override;
  private:
    std::vector<char> types;
    bool success;
    double time;
  
  };
}
#endif 
