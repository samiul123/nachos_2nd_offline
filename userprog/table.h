#ifndef TABLE_H
#define TABLE_H

#include "utility.h"
#include "synch.h"

class Table{
public:
  Table(int size);
  ~Table();
  int Alloc(void *object);
  void *Get(int index);
  void Release(int index);
  int processCount;
private:
  Lock* lock;
  int tableSize;
  void** table;
  bool* isUsed;
};
extern Table *processTable;

#endif
