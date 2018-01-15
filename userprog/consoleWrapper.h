#ifndef CONSOLEWRAPPER_H
#define CONSOLEWRAPPER_H

#include "console.h"

//this class is for synchronozation in console read and write
//
//
//
//
//

class ConsoleWrapper{
  private:
    Console *console ;
    //Lock* consoleLock;
  public:
    ConsoleWrapper();
    void DoLock();
    void ReleaseLock();
    char GetChar();
    void PutChar(char charToPut);
};

#endif
