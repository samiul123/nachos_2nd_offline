#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "bitmap.h"
#include "synch.h"
#include "machine.h"
#include "list.h"
#include "addrspace.h"

class MemoryManager {
private:
  /* data */
  int numPhysPages;
  BitMap* memMap;
  Lock* lock;

  // AddrSpace** spaceTable;
  // int* vpnTable;
  // int victimPage();
public:
  int numFreePages;
  MemoryManager (int numPages);
  ~MemoryManager ();
  int AllocPage();
  void FreePage(int physPageNum);
  bool PageIsAllocated(int physPageNum);

};
extern MemoryManager *mm;
#endif
