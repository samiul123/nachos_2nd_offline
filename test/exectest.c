/*
 * exectest.c
 *
 * Exec a simple program.  Return the result of Exec.
 */

#include "syscall.h"

int
main()
{
    int result = 1000;
		char buffer[10];
		char bufferWrite[5];
		int i;
		for(i = 0; i < 5; i++){
			bufferWrite[i] = i + 'a';
		}
    // result = Exec("./test/array");
		Read(buffer, 5, 1);
		Write(bufferWrite, 5, 1);
    //Exit(result);
}
