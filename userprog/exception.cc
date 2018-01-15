// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "addrspace.h"
#include "synch.h"
#include "table.h"
#include "consoleWrapper.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

// int strUserToKernel(char* src, char* dst, int size){
//   int virtualAddress = (int)src;
//   int data;
//   for(int i = 0; i < size; i++){
//     if(!machine->ReadMem(virtualAddress, sizeof(char), &data)){
//       return -1;
//     }
//     dst[i] = (char)data;
//     virtualAddress++;
//   }
//   dst[size] = '\0';
//   return 0;
// }
//
extern ConsoleWrapper* synchConsole;
void updateProgramCouonter(){
  int programCounter;
  programCounter = machine->ReadRegister(PCReg);
  machine->WriteRegister(PrevPCReg, programCounter);
  programCounter = machine->ReadRegister(NextPCReg);
  machine->WriteRegister(PCReg, programCounter);
  programCounter += 4;
  machine->WriteRegister(NextPCReg, programCounter);
}

void processCreator(void* arg){
  currentThread->space->InitRegisters();
  currentThread->space->RestoreState();
  machine->Run();
}

void ExceptionHandler(ExceptionType which){
  int type = machine->ReadRegister(2);
  //halt handler
  if ((which == SyscallException) && (type == SC_Halt)) {
    printf("Halt exception found\n");
    DEBUG('a', "Shutdown, initiated by user program.\n");
 	  interrupt->Halt();
  }
  //exec handler
  else if((which == SyscallException) && (type == SC_Exec)){
    printf("Exec excption found\n");
    //get the address of the filename
    int buffaddr = machine->ReadRegister(4);
    char *filename = new char[100];
    //now we have the address of the filename
    //so reading the filename
    int ch;
    //if(!machine->ReadMem(buffaddr, sizeof(char), (int*)&ch))return;
    int i  =0;
    while (true) {
      //filename[i] = (char)ch;
      printf("exception 1\n");
      machine->ReadMem(buffaddr, sizeof(char), (int*)&filename[i]);
      printf("exception 2\n");
      if (filename[i] == '\0')
        break;
      buffaddr++;
      i++;
    }
    //now we have the filename in filename
    OpenFile *executable = fileSystem->Open(filename);
    if (executable == NULL) {
    	printf("Unable to open file %s\n", filename);
    	return;
    }
    //printf("qqqqqqqqqqn\n");
    AddrSpace *space;
    space = new AddrSpace(executable);
    Thread *t = new Thread(filename);
    t->space = space;
    t->id = processTable->Alloc((void*)t);
    if(t->id == -1){
      printf("There's no free memory to allocate");
      return;
    }
    printf("%d allcated in processTable\n", t->id);
    DEBUG('z', "%d allcated in processTable\n", t->id);


    delete executable;
    machine->WriteRegister(2, t->id);
    delete filename;
    t->Fork(processCreator,NULL);
  }
  //Exit handler
  else if((which == SyscallException) && (type == SC_Exit)){
    printf("exit exception found\n");
    int val = machine->ReadRegister(4);
    printf("exiting with value: %d\n", val);
    //releasing address space of currentThread
    currentThread->space->FreeMemory();
    //deleting address space of currentThread
    //printf("in exit exception 1\n");
    delete currentThread->space;
    //index in processTable
    int indexInTable = currentThread->id;
    //releasing from process table also
    processTable->Release(indexInTable);
    //printf("in exit exception 2\n");
    //finally finishing the thread
    printf("Exiting thread: %d\n", currentThread->id);
    if(processTable->processCount == 0){
      interrupt->Halt();
    }
    // printf("Exiting thread: %d\n", currentThread->id);
    currentThread->Finish();
    // printf("Exiting thread: %d\n", currentThread->id);
  }
  //read handler
  else if((which == SyscallException) && (type == SC_Read)){
    printf("read exception found\n");
    int address = machine->ReadRegister(4);
    //int strSize = machine->ReadRegister(5);
    synchConsole->DoLock();
    char* stringBuf = new char[100];
    int i = 0;
    //reading from console
    while (true) {
      stringBuf[i] = synchConsole->GetChar();
      i++;
      //printf("value of i: %d\n",i);
      if(stringBuf[i] == '\0'){
        //stringBuf[i] = '\0';
        break;
      }
      // stringBuf[i] = synchConsole->GetChar();
      // i++;
    }

    //writing to OS what's written in Console
    i = 0;
    while (i < sizeof(stringBuf)) {
        machine->WriteMem(address + i, sizeof(char), (int)stringBuf[i]);
        i++;
    }
    //printf("Entered value from console: %s\n",stringBuf);
    //machine->WriteRegister(2, strSize);
    //bzero(stringBuf, sizeof(char) * strSize);
    synchConsole->ReleaseLock();
  }
  //writeHandler
  else if((which == SyscallException) && (type == SC_Write)){
    printf("write exception found\n");
    int address = machine->ReadRegister(4);
    int strSize = machine->ReadRegister(5);
    synchConsole->DoLock();
    int i = 0;
    while (i <= strSize) {
      if(i == strSize){
        synchConsole->PutChar('\n');
        break;
      }
      int charToPut;
      machine->ReadMem(address + i, sizeof(char), &charToPut);
      synchConsole->PutChar((char)charToPut);
      i++;
    }
    synchConsole->ReleaseLock();
  }
  else {
    printf("Unexpected user mode exception %d %d\n", which, type);
  	ASSERT(false);
  }
  updateProgramCouonter();

}
