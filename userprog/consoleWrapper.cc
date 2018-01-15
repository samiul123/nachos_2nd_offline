#include "consoleWrapper.h"
#include "synch.h"

Lock* consoleLock = new Lock((char*)"consoleLock");
// Lock* availableToRead = new Lock((char*)"availableToRead");
// Lock* writeDone = new Lock((char*)"write done");
Semaphore* readAvail = new Semaphore("read avail", 0);
Semaphore* writeDone = new Semaphore("write done", 0);

void ReadAvail(void* arg){
  //availableToRead->Release();
  //sending th thread to readyToRun list
  readAvail->V();
}

void WriteDone(void* arg){
  //writeDone->Release();
  //sending the thread to readyToRun list
  writeDone->V();
}

ConsoleWrapper::ConsoleWrapper(){
  //initializing console
  //giving NULL as param for keyboard input
  console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
}

void ConsoleWrapper::DoLock(){
  printf("Console acquired lock\n");
  //acquiring lock
  consoleLock->Acquire();
}

void ConsoleWrapper::ReleaseLock(){
  //releasing lock such that others can access
  printf("console relased lock\n");
  consoleLock->Release();
}

char ConsoleWrapper::GetChar(){
  //availableToRead->Acquire();
  readAvail->P();
  //getting char from console
  char charToReturn = console->GetChar();
  printf("Console returning char\n");
  return charToReturn;
}

void ConsoleWrapper::PutChar(char charToPut){
  //putting char to console
  console->PutChar(charToPut);
  //writeDone->Acquire();
  writeDone->P();
}
