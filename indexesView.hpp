//
//  indexesView.hpp
//  
//

#ifndef indexesView_hpp
#define indexesView_hpp

#include <stdio.h>
#include <vector>
#include <iomanip>
#include "Helpers.hpp"
#include "View.hpp"
#include "Entity.hpp"
#include "Index.hpp"

namespace ECE141 {
  class indexesView : public View {
  public:
    indexesView(double aTime, Keywords aKeyword);
    //indexesView(double aTime, Keywords aKeyword, std::map<IndexKey, uint32_t>& indexes);
      bool setEntities(EntityCollection& entityList);
      bool setIndexes(std::map<IndexKey, uint32_t>& indexes);
    bool show(std::ostream &aStream) override;
  private:
    EntityCollection entities;
    std::map<uint32_t, uint32_t>  data;
    double time;
    Keywords keyword;
  };
}
#endif /* ShowView_hpp */
