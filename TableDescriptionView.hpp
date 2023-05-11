//
//  TableDescriptionView.hpp
//  assignment3
//
//  Created by Ya Gao
//

#ifndef TableDescriptionView_hpp
#define TableDescriptionView_hpp

#include <stdio.h>
#include <vector>
#include <iomanip>
#include "Helpers.hpp"
#include "View.hpp"
#include "Entity.hpp"

namespace ECE141 {
  class TableDescriptionView : public View {
  public:
    TableDescriptionView(double aTime, Keywords aKeyword, bool status);
    void setEntity(Entity const & anEntity);
    bool show(std::ostream &aStream) override;
    const char* nullStr[2] = {"NO", "YES"};
    const char* keyStr[2] = {"NO", "YES"};
  private:
    StringList dbNames;
    double time;
    bool success;
    Keywords keyword;
    Entity theEntity;
  };
}
#endif /* ShowView_hpp */
