#include "indexesView.hpp"

namespace ECE141 {

indexesView::indexesView(double aTime, Keywords aKeyword):
    time(aTime), keyword(aKeyword){}

//indexesView::indexesView(double aTime, Keywords aKeyword, std::map<IndexKey, uint32_t>& indexes):
//    time(aTime), keyword(aKeyword){
//        for(auto thePair : indexes) {
//            data[std::get<uint32_t>(thePair.first)] = thePair.second;
//        }
//    }

bool indexesView::setEntities(EntityCollection& entityList){
    entities = std::move(entityList);
    return true;
}

bool indexesView::setIndexes(std::map<IndexKey, uint32_t>& indexes){
    for(auto thePair : indexes) {
        data[std::get<uint32_t>(thePair.first)] = thePair.second;
    }
    return true;
}

bool indexesView::show(std::ostream &aStream){
      if(keyword == Keywords::indexes_kw) {
          aStream << "+-----------------------------------------+\n";
          aStream << "|" << std::left << std::setw(20) << "table";
          aStream << "|" << std::left << std::setw(20) << "fields(s)";
          aStream << "|\n";
          aStream << "+-----------------------------------------+\n";
          
          for(auto& entity: entities) {
              aStream << "|" << std::left << std::setw(20) << entity->getName();
              aStream << "|" << std::left << std::setw(20) << entity->getPrimaryKey();
              aStream << "|\n";
              aStream << "+-----------------------------------------+\n";
          }
          
          aStream << entities.size() << "rows in set " << "(" << std::setprecision(2) << time << " sec)\n";
      } else if(keyword == Keywords::index_kw) {
          aStream << "+-----------------------------------------+\n";
          aStream << "|" << std::left << std::setw(20) << "key";
          aStream << "|" << std::left << std::setw(20) << "block#";
          aStream << "|\n";
          aStream << "+-----------------------------------------+\n";
          
          for(auto pair: data) {
              aStream << "|" << std::left << std::setw(20) << pair.first;
              aStream << "|" << std::left << std::setw(20) << pair.second;
              aStream << "|\n";
              aStream << "+-----------------------------------------+\n";
          }
          aStream << data.size() << " rows in set " << "(" << std::setprecision(2) << time << " sec)\n";
      } else {
          return false;
      }
      return true;
  }

}
