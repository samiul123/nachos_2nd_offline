#include "table.h"

Table::Table(int size){
  tableSize = size;
  processCount = 0;
  table = new void*[size];
  isUsed = new bool[size];
  for(int i = 0; i < size; i++){
    table[i] = NULL;
    isUsed[i] = false;
  }
  lock = new Lock("TblLock");
}

Table::~Table(){
  delete[] table;
}

int Table::Alloc(void *object){
  lock->Acquire();
  printf("in table lock\n");
  for(int i = 1; i < tableSize; i++){
    if(isUsed[i] == false){
      isUsed[i] = true;
      table[i] = object;
      processCount++;
      lock->Release();
      return i;
    }
  }
  printf("process table full\n");
  lock->Release();
  return -1;
}

void* Table::Get(int index){
  lock->Acquire();
  if(index < 1 || index >= tableSize || !isUsed[index]){
    lock->Release();
    return NULL;
  }
  else{
    lock->Release();
    return table[index];
  }
}

void Table::Release(int index){
  lock->Acquire();
  if(index > 0 && index < tableSize && isUsed[index]){
    isUsed[index] = false;
    processCount--;
  }
  lock->Release();
}
