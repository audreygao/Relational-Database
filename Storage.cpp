//
//  Storage.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//


#include <sstream>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <cstring>
#include "Storage.hpp"
#include "Config.hpp"

namespace ECE141 {


  // USE: ctor ---------------------------------------
  Storage::Storage(std::iostream &aStream) : BlockIO(aStream) {
  }

  // USE: dtor ---------------------------------------
  Storage::~Storage() {
  }

  bool Storage::each(const BlockVisitor &aVisitor) {
    //implement this as a visitor pattern for storage blocks
    uint32_t count = getBlockCount();
    Block theBlock;
    bool result = true;
    for(uint32_t index = 0; index < count; index++) {
      readBlock(index, theBlock);
      if(!aVisitor(theBlock, index)) result = false;
    }
    return result;
  }
  
  //write logic to get the next available free block
  uint32_t Storage::getFreeBlock() {
    if(available.size())
      return available.front();
    return getBlockCount();
  }

  void Storage::setAvailable(BlockList freeBlocks) {
    available = freeBlocks;
  }

  void Storage::popFree() {
    if(available.size())
      available.pop_front();
  }
  
  //write logic to mark a block as free...
  StatusResult Storage::markBlockAsFree(uint32_t aPos) {
    releaseBlocks(aPos);
    return StatusResult{Errors::noError};
  }

  // USE: for use with storable API...
  //   Write logic to mark a sequence of blocks as free)
  //   starting at given block number, following block sequence
  //   defined in the block headers...
  StatusResult Storage::releaseBlocks(uint32_t aPos,bool aInclusive) {
    uint32_t blockNum = aPos;
    uint8_t pos, count;
    do {
      Block aBlock;
      readBlock(blockNum, aBlock);
      blockNum = aBlock.header.next;
      pos = aBlock.header.pos + 1;
      count = aBlock.header.count;
      
      aBlock.header.type = (char)BlockType::free_block;
      writeBlock(aPos, aBlock);
    } while(pos < count);
    return StatusResult{Errors::noError};
  }

  //Write logic to break stream into N parts, that fit into
  //a series of blocks. Save each block, and link them together
  //logically using data in the header...
  StatusResult Storage::save(std::iostream &aStream,
                             StorageInfo &anInfo) {
    //determine the number of blocks needed
    size_t theSize = anInfo.size;
    size_t theParts = theSize / kPayloadSize;    
    uint8_t count = (theSize % kPayloadSize) ? (theParts + 1) : theParts;

    //determine the starting block number (given or get available block index)
    uint32_t currentBlockNum = anInfo.start == -1 ? getFreeBlock() : anInfo.start;
    uint32_t nextBlockNum;

    //get the ith chunk from the stream
    for(size_t thePos=0; thePos<count; thePos++) {
      //extract a block from the stream
      Block theBlock(anInfo.type);
      aStream.get(theBlock.payload, kPayloadSize);

      //next available block index after current block is written
      popFree();
      nextBlockNum = getFreeBlock();

      //fill out the header info for current block
      theBlock.header = BlockHeader(anInfo.type);
      theBlock.header.count = count;
      theBlock.header.pos = thePos;
      theBlock.header.id = currentBlockNum; // !!!!!!newly added blocknum of the block to header
      anInfo.start = currentBlockNum;
      if(thePos < count - 1) theBlock.header.next = nextBlockNum; //if more blocks: get next free block number
      theBlock.header.refId = anInfo.refId;

      //write the block
      writeBlock(currentBlockNum, theBlock);
      currentBlockNum = nextBlockNum;
    }
    return StatusResult{Errors::noError};
  }

  //Write logic to read an ordered sequence of N blocks, back into
  //a stream for your caller
  StatusResult Storage::load(std::iostream &anOut,uint32_t aStartBlockNum) {
    uint8_t count = 0;
    uint8_t i = 0;
    uint32_t blockNum = aStartBlockNum;
      
    do {
      Block aBlock;
      readBlock(blockNum, aBlock);
      count = aBlock.header.count;
      i = aBlock.header.pos + 1;
      anOut.write(aBlock.payload, strlen(aBlock.payload));

      //get the blockNum of the next block (that belongs to this sequence)
      blockNum = aBlock.header.next;
    }while(i < count);
    //anOut.flush();

    return StatusResult{Errors::noError};
  }

uint32_t Storage::calculateIndexLocation(uint32_t aSize) {
  size_t theParts = aSize / kPayloadSize;
  uint8_t count = (aSize % kPayloadSize) ? (theParts + 1) : theParts;
  BlockList theAvailableList = available;
  if(count < available.size())
    return available[count];
  else
    return getBlockCount() + (count - available.size());
}
 
}

