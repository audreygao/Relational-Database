//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <fstream> 
#include "Storage.hpp"
#include "Entity.hpp"
#include "Query.hpp"
#include "Filters.hpp"
#include <vector>
#include <deque>
#include "Index.hpp"
#include "Join.hpp"

namespace ECE141 {

  class Database : public Storable {
  public:
    
            Database(const std::string aPath, CreateDB);
            Database(const std::string aPath, OpenDB);
    virtual ~Database();

    //storable...
    StatusResult    encode(std::ostream &anOutput) override;
    StatusResult    decode(std::istream &anInput) override;

    void dump(std::vector<char>& typeList);
    
    std::string     getName() const;
    
    StatusResult    encodeAndSaveRows(RowCollection& aRowCollection, uint32_t aHashValue,
                                      std::string aPrimaryKey = "", uint32_t anIndexLocation = 0);
    
    StatusResult    getAllEntities(EntityCollection& entities, StringList& tableNames);
    StatusResult    getEntity(Entity &theEntity, std::string entityName);
    StatusResult    decodeEntity(Entity& anEntity, const char * payload);
    StatusResult    saveEntity(Entity& anEntity);
    StatusResult    updateEntity(Entity& anEntity);
    StatusResult    dropTable(std::string entityName, int& rowsDeleted);

    StatusResult    getIndexAtBlockNum(Index& anIndex, uint32_t indexBlockNum);
    
    StatusResult    leftJoin(RowCollection &leftRows, RowCollection &newRows, JoinList &joinList);
    StatusResult    rightJoin(Query &aQuery, Filters& aFilter, JoinList& joinList, RowCollection &newRows);
    
    StatusResult    selectRows(Query &aQuery, Filters& aFilter, RowCollection &aRows);
    StatusResult    updateRows(Entity &anEntity, Filters& whereFilter, Filters& setFilter, int& rowsUpdated);
    StatusResult    deleteRows(Entity& anEntity, Filters& aFilter, int& numDeleted);

    StatusResult    getIndexData(std::map<IndexKey, uint32_t> &indexes, Entity& anEntity);
    StatusResult    changeEntity(Keywords subCmd, Entity& anEntity, AttributeList attributes);
      
  protected:
    
    std::string     name;
    Storage         storage;
    bool            changed;
    std::fstream    stream; //stream storage uses for IO
  };

}
#endif /* Database_hpp */
