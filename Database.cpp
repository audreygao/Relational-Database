//
//  Database.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include "Storage.hpp"
#include "Database.hpp"
#include "Config.hpp"

namespace ECE141 {
  
using BlockList = std::deque<uint32_t>;
  Database::Database(const std::string aName, CreateDB)
    : name(aName), storage(stream), changed(true)  {
    std::string thePath = Config::getDBPath(name);
    stream.clear(); // Clear Flag, then create file...
    stream.open(thePath.c_str(), std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc);
    stream.close();
    stream.open(thePath.c_str(), std::fstream::binary | std::fstream::binary | std::fstream::in | std::fstream::out);
      
    //STUDENT: Write meta info to block 0...
    Block metaBlock = Block(BlockType::meta_block);
    metaBlock.header.type = static_cast<char>(BlockType::meta_block);
    storage.writeBlock(0, metaBlock);
  }

  Database::Database(const std::string aName, OpenDB)
    : name(aName), changed(false), storage(stream) {
      
      std::string thePath = Config::getDBPath(name);
      stream.open (thePath.c_str(), std::fstream::binary
                   | std::fstream::in | std::fstream::out);
      
      //STUDENT: Load meta info from block 0
      BlockList freeList;
      storage.each([&](const Block& theBlock, uint32_t index){
       if(theBlock.header.type == static_cast<char>(BlockType::free_block))
          freeList.push_back(index);
        return true;});
      storage.setAvailable(freeList);
  }

  Database::~Database() {
    if(changed) {
      //save db meta information to block 0?
      //save anything else in memory that changed...
    }
  }

  StatusResult Database::encode(std::ostream &anOutput) {
    return StatusResult{Errors::noError};
  }

  StatusResult Database::decode(std::istream &anInput) {
    return StatusResult{Errors::noError};
  }

  StatusResult Database::getIndexAtBlockNum(Index& anIndex, uint32_t indexBlockNum) {
    std::stringstream theIndexDecodeStream;
    if( storage.load(theIndexDecodeStream, indexBlockNum) ) {
      if( anIndex.decode(theIndexDecodeStream) ) {
        return StatusResult(Errors::noError);
      }
      return StatusResult(Errors::cantCreateIndex);
    }
    return StatusResult(Errors::readError);
  }

  void     Database::dump(std::vector<char>& typeList) {

    //add the block type of each block to the output vector
    storage.each([&](const Block& theBlock, uint32_t index){
      typeList.push_back(theBlock.header.type);
      return true;});
  }

  std::string     Database::getName() const {
    return name;
  }

StatusResult    Database::encodeAndSaveRows(RowCollection& aRowCollection, uint32_t aHashValue,
                                            std::string aPrimaryKey, uint32_t anIndexLocation) {
  //encode and write each of the rows to database
  std::map<Value, uint32_t> theNewIndexEntries;
  for(auto& row : aRowCollection) {
    std::stringstream rowStream;
    if( row -> encode(rowStream) ) {
      StorageInfo rowsInfo(aHashValue, rowStream.str().size());
      if( storage.save(rowStream, rowsInfo) && aPrimaryKey.size() ) { //if there's a primary key
        theNewIndexEntries[row -> getValue(aPrimaryKey)] = rowsInfo.start;
      }
    } else {
      return StatusResult(Errors::readError);
    }
  }
  
  //If there is a known index
  if(anIndexLocation) {
    //Load index into memory
    Index theIndex = Index(storage);
    if(getIndexAtBlockNum(theIndex, anIndexLocation)) {
      //Add new KV (id: blockNum)
      for(auto theEntryPair: theNewIndexEntries) {
        theIndex[std::get<int>(theEntryPair.first)] = theEntryPair.second;
      }
      std::stringstream theIndexEncodeStream;
      if( theIndex.encode(theIndexEncodeStream) ) {
        //Start writing back to storage
        StorageInfo theStorageInfo(theIndex.getTableId(), theIndexEncodeStream.str().size(),
                                   anIndexLocation, BlockType::index_block);
        if( storage.save(theIndexEncodeStream,theStorageInfo) )
          return StatusResult(Errors::noError);
      }
    }
    return StatusResult(Errors::cantCreateIndex);
  }
  return StatusResult(Errors::indexExists);
}

  StatusResult    Database::saveEntity(Entity& anEntity){
    
    //encode the entity and the block number of the associated index block
    std::stringstream theEntityStream;
    if( anEntity.encode(theEntityStream) ) {
      uint32_t theIndexBlockNum = storage.calculateIndexLocation(theEntityStream.str().size());
      theEntityStream << " " << theIndexBlockNum;
      StorageInfo theEntityInfo(0, theEntityStream.str().size());
      theEntityInfo.type = BlockType::entity_block;
      if( storage.save(theEntityStream, theEntityInfo) ) {
        
        Index theIndex(storage, theIndexBlockNum);
        theIndex.setTableId(Entity::hashString(anEntity.getName()));
        
        //encode and save the index block to database
        std::stringstream theIndexStream;
        if( theIndex.encode(theIndexStream) ) {
          StorageInfo theIndexInfo(Entity::hashString(anEntity.getName()), theIndexStream.str().size(),
                                   theIndexBlockNum, BlockType::index_block);
          if( storage.save(theIndexStream, theIndexInfo) ) {
            return StatusResult();
          }
        }
      }
    }
    return StatusResult(Errors::unknownError);
  }

  //Update an entity i.e. update with new primary key counter
  StatusResult Database::updateEntity(Entity& anEntity) {
    std::stringstream theEntityStream;
    if( anEntity.encode(theEntityStream) ) {
      StorageInfo theEntityInfo(0, theEntityStream.str().size(), anEntity.getBlockNum(), BlockType::entity_block);
      if( storage.save(theEntityStream, theEntityInfo) ) {
        return StatusResult();
      }
    }
    return StatusResult(Errors::unknownError);
  }


  StatusResult    Database::decodeEntity(Entity& anEntity, const char * payload) {
    std::stringstream str;
    str << payload;
    return anEntity.decode(str);
  }

  StatusResult    Database::getAllEntities(EntityCollection& entities, StringList& tableNames) {
    //find all entity blocks in the database
    storage.each([&](const Block& theBlock, uint32_t index){
      if(theBlock.header.type == static_cast<char>(BlockType::entity_block)) {

        //decode the payload to retrieve the Entity
        Entity* anEntity = new Entity();
        if( decodeEntity(*anEntity, theBlock.payload) ) {
          anEntity -> setBlockNum(index);
          entities.push_back(std::unique_ptr<Entity>(anEntity));
          tableNames.push_back(anEntity -> getName());
        }
      }
      return true;
    });
    return StatusResult();
  }

  StatusResult    Database::getIndexData(std::map<IndexKey, uint32_t> &indexes, Entity& anEntity) {
    //load the index of the given table into memory
    Index theIndex(storage);
    if(getIndexAtBlockNum(theIndex, anEntity.getIndexBlockNum())) {
      //copy the data in the index
      theIndex.eachKV([&](const IndexKey& key, uint32_t Value){
        indexes[key] = Value;
        return true;
      });
      return StatusResult();
    }
    return StatusResult(Errors::cantCreateIndex);
  }

  StatusResult Database::dropTable(std::string entityName, int& rowsDeleted) {
    
    // 1. Load all entities into memory
    EntityCollection entities;
    StringList tableNames;
    if(getAllEntities(entities, tableNames)) {
      for(auto& entity: entities) {
        // 2. find the entity with the matching name
        if( entity != nullptr && entity -> getName() == entityName ) {
          //mark the entity block as free block
          storage.markBlockAsFree(entity -> getBlockNum());
          
          //delete all rows of this entity
          Filters aFilter;
          int rowsDeleted;
          if( deleteRows(*entity, aFilter, rowsDeleted) ) {
            storage.markBlockAsFree(entity -> getIndexBlockNum());
            return StatusResult(Errors::noError);
          }
        }
      }
      return StatusResult(Errors::unknownEntity);
    }
    return StatusResult(Errors::unknownError);
  }

  StatusResult Database::getEntity(Entity &theEntity, std::string entityName) {
    //load all entities into memory
    EntityCollection entities;
    StringList tableNames;
    if(getAllEntities(entities, tableNames)) {
      for(auto& entity: entities) {
        //find the matching entity
        if( entity->getName() == entityName ) {
          theEntity = *entity;
          return StatusResult();
        }
      }
    }
    return StatusResult(Errors::unknownEntity);
  }

  StatusResult Database::leftJoin(RowCollection &leftRows, RowCollection &newRows, JoinList &joinList) {
    for(auto &row: leftRows) {
      for(auto join: joinList) {
        
        //change lhs operand value and update rhs ttype for use in filters.match
        join.expr.lhs.value = row->getValue(join.expr.lhs.name);
        join.expr.rhs.ttype = TokenType::identifier;
        
        Entity anEntity;
        
        if( getEntity(anEntity, join.table)) {
          
          //set up query and filter for selectRows in the other table
          Query aQuery;
          aQuery.setFrom(&anEntity);
          Filters aFilter;
          aFilter.add(new Expression(join.expr.lhs, join.expr.op, join.expr.rhs));
          RowCollection rightRows;
          
          if( selectRows(aQuery, aFilter, rightRows) ) {
            //even if there's no matching row in the other table, add a row for this row
            if(rightRows.size() == 0) {
              Row * aRow = new Row();
              KeyValues data;
              row->getKeyValues(data);
              aRow->setKeyValues(data);
              newRows.push_back(std::unique_ptr<Row>(aRow));
            }
            
            //add matching rows to the current row
            for(auto& rightRow: rightRows) {
              Row * aRow = new Row();
              KeyValues data;
              row->getKeyValues(data);
              aRow->setKeyValues(data);
              KeyValues rightData;
              rightRow->getKeyValues(rightData);
              aRow->addKeyValues(rightData);
              newRows.push_back(std::unique_ptr<Row>(aRow));
            }
          } else {
            return StatusResult(Errors::unknownError);
          }
        } else {
          return StatusResult(Errors::unknownTable);
        }
      }
    }
    return StatusResult();
  }

  StatusResult Database::rightJoin(Query &aQuery, Filters& aFilter, JoinList& joinList, RowCollection &newRows) {
    
    
    //select all rows for the right table
    RowCollection rightRows;
    Entity anEntity;
    if( getEntity(anEntity, joinList.at(0).table) ) {
      Query rightQuery;
      rightQuery.setFrom(&anEntity);
      if( selectRows(rightQuery, aFilter, rightRows) ) {
        //swap expression and table in Join
        Operand tempOp = joinList.at(0).expr.lhs;
        joinList.at(0).expr.lhs = joinList.at(0).expr.rhs;
        joinList.at(0).expr.rhs = tempOp;
        joinList.at(0).table = aQuery.getFrom()->getName();
        
        //use right table as left table in leftJoin
        if (leftJoin(rightRows, newRows, joinList))
          return StatusResult();
      }
    }
    return StatusResult(Errors::unknownError);
  }

  StatusResult Database::selectRows(Query &aQuery, Filters& aFilter, RowCollection &aRows) {
    Entity * anEntity;
    if((anEntity = aQuery.getFrom())) {
      AttributeList aList;
      anEntity->getAttributeList(aList);
      StringList attributeNames = anEntity->getAttributeNamesList(aList);
      
      //retrieve the Index of the given table
      Index theIndex(storage, anEntity -> getIndexBlockNum());
      if( getIndexAtBlockNum(theIndex, anEntity -> getIndexBlockNum())) {
        theIndex.each([&](const Block& theBlock, uint32_t index) {
          //check that the block is 1st and a data block that belongs to the given table
          if(theBlock.header.type == static_cast<char>(BlockType::data_block) &&
            theBlock.header.refId == Entity::hashString(anEntity->getName()) && theBlock.header.pos == 0) {
            
            //load the row into memory
            std::stringstream str;
            if(storage.load(str, index)) {
              Row * aRow = new Row();
              aRow->setBlockNumber(index);
              aRow->setAttributeList(aList);
              if(aRow->decode(str)) {
                //apply filters
                KeyValues data;
                aRow->getKeyValues(data);
                if(aFilter.matches(data)) {
                  aRows.push_back(std::unique_ptr<Row>(aRow));
                }
              }
            }
          }
          return true;
        });
        return StatusResult();
      }
    }
    return StatusResult(Errors::unknownError);
  }

  StatusResult Database::updateRows(Entity &anEntity, Filters& whereFilter, Filters& setFilter, int& rowsUpdated) {
    Query aQuery;
    aQuery.setFrom(&anEntity);
    RowCollection theRows;
    if( selectRows(aQuery, whereFilter, theRows) ) {
      KeyValues newData;
      uint32_t hashVal = Entity::hashString(anEntity.getName());
      AttributeList aList;
      anEntity.getAttributeList(aList);
      
      //iterate over each selected rows and update the values in the row
      for(auto& row: theRows) {
        row->getKeyValues(newData);
        setFilter.updateValuesInList(newData);
        row->setKeyValues(newData);
      }
      encodeAndSaveRows(theRows, hashVal);
      rowsUpdated = theRows.size();
    }
    
    return StatusResult();
  }

  StatusResult Database::deleteRows(Entity& anEntity, Filters& aFilter, int& numDeleted) {
    Query aQuery;
    aQuery.setFrom(&anEntity);
    RowCollection theRows;
    if( selectRows(aQuery, aFilter, theRows) ) { //select rows that match the filter
      
      //decode to get the index of the given table
      Index theIndex(storage);
      if(getIndexAtBlockNum(theIndex, anEntity.getIndexBlockNum())){
        
        //iterate over each selected rows
        for(auto& row: theRows) {
          //erase the index and mark the row as free
          if( theIndex.erase(std::get<int>(row -> getValue(anEntity.getPrimaryKey()))) &&
              storage.markBlockAsFree(row->getBlockNumber()) ) {
              numDeleted++;
          }
        }
        return StatusResult();
      }
    }
    return StatusResult(Errors::unknownError);
  }

  StatusResult Database::changeEntity(Keywords subCmd, Entity& anEntity, AttributeList attributes) {
    switch(subCmd) {
      //add all new attributes to the entity
      case Keywords::add_kw:
        for(auto att: attributes) {
          anEntity.addAttribute(att);
        }
        break;
      case Keywords::drop_kw:
        for(auto att: attributes) {
          anEntity.removeAttribute(att.getName());
        }
        break;
      default: break;
    }
    updateEntity(anEntity);
    
    
    Query aQuery;
    aQuery.setFrom(&anEntity);
    Filters aFilter;
    RowCollection theRows;
    uint32_t hashVal = Entity::hashString(anEntity.getName());
    if( selectRows(aQuery, aFilter, theRows) ) {
      for(auto& row: theRows) {
        for(auto att: attributes) {
          switch(subCmd) {
            //add all new attributes to the entity
            case Keywords::add_kw:
              row->addValue(att.getName(), Value{0});
              break;
            case Keywords::drop_kw:
              row->removeValue(att.getName());
              break;
            default: break;
          }
        }
      }
      encodeAndSaveRows(theRows, hashVal);
      return StatusResult();
    }
    
    
    return StatusResult(Errors::unknownError);
  }
}
