#include "memoryManager.h"
#include "system.h"
#include "sysdep.h"

MemoryManager::MemoryManager(int numPages){
  numPhysPages = numPages;
  numFreePages = numPages;
  memMap = new BitMap(numPages);
  lock = new Lock("mapLock");
}

MemoryManager::~MemoryManager(){
  delete memMap;
  delete lock;
}

int MemoryManager::AllocPage(){
  lock->Acquire();
  int physNum = memMap->Find();
  if(physNum == -1){
    lock->Release();
    return -1;
  }
  numFreePages--;
  lock->Release();
  return physNum;
}

void MemoryManager::FreePage(int physPageNum){
  lock->Acquire();
  memMap->Clear(physPageNum);
  numFreePages++;
  lock->Release();
}

bool MemoryManager::PageIsAllocated(int physPageNum){
  lock->Acquire();
  bool isAllocated = memMap->Test(physPageNum);
  lock->Release();
  return isAllocated;
}
