// progtest.cc
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include "memoryManager.h"
#include "table.h"
#include "consoleWrapper.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------
MemoryManager* mm;
Table* processTable;
ConsoleWrapper* synchConsole;
// Console *console;
// Semaphore *readAvail;
// Semaphore *writeDone;

void
StartProcess(const char *filename)
{
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;
    mm = new MemoryManager(NumPhysPages);
    processTable = new Table(NumPhysPages);
    synchConsole = new ConsoleWrapper();
    // console = new Console(NULL, NULL, ReadAvail, WriteDone, 0);
    // readAvail = new Semaphore("read avail", 0);
    // writeDone = new Semaphore("write done", 0);
    if (executable == NULL) {
    	printf("Unable to open file in ProgTest %s\n", filename);
    	return;
    }
    printf("fsdghkdjklkd\n");
    space = new AddrSpace(executable);
    currentThread->space = space;

    delete executable;			// close file

    printf("progtest 1\n");
    space->InitRegisters();// set the initial register values
    printf("progtest 2\n");
    space->RestoreState();		// load page table register
    printf("progtest 3\n");
    machine->Run();
    printf("progtest 4\n");			// jump to the user progam
    //ASSERT(false);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(void* arg) { readAvail->V(); }
static void WriteDone(void* arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void
ConsoleTest (const char *in, const char *out)
{
    char ch;
    printf("In progtest consol Test\n");
    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);

    for (;;) {
    	readAvail->P();		// wait for character to arrive
    	ch = console->GetChar();
    	console->PutChar(ch);	// echo it!
    	writeDone->P() ;        // wait for write to finish
    	if (ch == 'q') return;  // if q, quit
    }
}
