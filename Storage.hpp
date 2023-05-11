//
//  Storage.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <string>
#include <fstream>
#include <iostream>
#include <deque>
#include <functional>
#include "BlockIO.hpp"
#include "Errors.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {
      
  struct CreateDB {}; //tags for db-open modes...
  struct OpenDB {};

  const int32_t kNewBlock=-1;

  class Storable {
  public:
    virtual StatusResult  encode(std::ostream &anOutput)=0;
    virtual StatusResult  decode(std::istream &anInput)=0;
  };

  struct StorageInfo {
    
    // 0, tableId, aSize, blockNum, BlockType::index_block
    StorageInfo(uint32_t aRefId, size_t theSize, uint32_t aStartPos=kNewBlock, BlockType aType=BlockType::data_block)
      : type(aType), start(aStartPos), refId(aRefId), size(theSize) {}
     
    BlockType type;
    uint32_t   start; //block#
    uint32_t    refId;
    size_t    size;
  };
  
  using BlockVisitor = std::function<bool(const Block&, uint32_t)>;

  using BlockList = std::deque<uint32_t>;

  // USE: Our storage class (for stream IO)
  class Storage : public BlockIO {
  public:
        
    Storage(std::iostream &aStream);
    ~Storage();
 
    StatusResult save(std::iostream &aStream, StorageInfo &anInfo);
    StatusResult load(std::iostream &aStream, uint32_t aStartBlockNum);

    bool         each(const BlockVisitor &aVisitor);

    StatusResult markBlockAsFree(uint32_t aPos);
    
    void setAvailable(BlockList freeBlocks);
    void popFree();
    
    uint32_t calculateIndexLocation(uint32_t aSize);

  protected:
  
    StatusResult releaseBlocks(uint32_t aPos, bool aInclusive=false);
    uint32_t     getFreeBlock(); //pos of next free (or new)...
            
    BlockList    available;
  };

  using BlockList = std::deque<uint32_t>;
  using BlockVisitor = std::function<bool(const Block&, uint32_t)>;

  struct BlockIterator {
    virtual bool each(const BlockVisitor)=0;
  };

}


#endif /* Storage_hpp */
