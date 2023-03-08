//
// Starting code for coursework 1. Requires "cwk1_extra.h", which should be placed in the same directory.
//
// To compile, either use the provided makefile, or from a shell (on cloud-hpc1.leeds.ac.uk) type:
//
// gcc -fopenmp -Wall -std=c99 -o cwk1 cwk1.c
//
// where '-fopenmp' is redundant until OpenMP functionality has been added, and '-Wall' (to turn on
// all warnings) is optional but recommended.
//
// Please do not change the compiler used by the makefile as this may not be installed on the machine used
// to test your code. Please only edit this file (and not the makefile).
//


//
// Includes
//

// Standard includes.
#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 
// Declarations and functions specific to this coursework. This file will
// be replaced with a different version as part of the assessment, and so there is
// no point making alterations to it - please only modify this file ('cwk1.c').
#include "cwk1_extra.h"
// You can read 'cwk1_extra.h' to see what it provides, but in summary:
//
// Global variables:
// int *stack;					// Stack array; allocated in allocateStack() and freed in finaliseStack().
// int maxStackSize0;			// The maximum stack size (=size of the array as allocated).
// int stackSize;				// The current stack size; initially zero.
//
// Functions:
// void allocateStack()			// Allocates memory for the stack array.
// void finaliseStack()			// Frees up memory used by the stack array.
// void displayStack( int )		// Prints the current stack. The argument is for messaging (0 for initial stack, 1 for final stack).
// int parseCommandLine( int argc, char** argv, int* initSize, int* numPop, int* invert, int* rotate )
//								// Parses the 5 command line arguments and sets the values of initSize, numPop, invert, and rotate.
//								// Prints an error message and returns -1 for invalid values.
//


//
// Functions for stack management.
//

// Push an item to the stack. Prints an error message if the stack limit has already been reached.

void pushToStack( int newItem )
{
	omp_lock_t stackLock;
	omp_init_lock(&stackLock);
	if( stackSize==maxStackSize )
	{
		printf( "Cannot add to stack; already at its maximum size (of %i).\n", maxStackSize );
		return;
	}

	// Add the 'item' (i.e. the integer value) to the top of the stack, and also increment the stack size.
	omp_set_lock(&stackLock);
	stack[stackSize++] = newItem;
	omp_unset_lock(&stackLock);
	omp_destroy_lock(&stackLock);
}

// Removes an item from the stack but does not return the value.
void popFromStack()
{

	if (stackSize>0)
	{
		stackSize--;
	}
	
}

// Inverts the stack in-place; that is, the order of all elements is reversed.
void invertStack()
{
	int i, j, temp;
	//#pragma omp parallel for private(i, j, temp)
	for (i = 0, j = stackSize - 1; i < j; i++, j--) {
		temp = stack[i];
		stack[i] = stack[j];
		stack[j] = temp;
	}
}

// Rotates the stack down to the given value.

void rotateStack( int depth )
{
	int i, temp = stack[stackSize-depth];
	#pragma omp parallel for
	for( i=0; i<depth-1; i++ )
		stack[stackSize-depth+i] = stack[stackSize-depth+i+1];

	stack[stackSize-1] = temp;
}

//
// Main
//
int main( int argc, char** argv )
{
	int i;

	//
	// Initialisation. Parse command line arguments and allocate memory for the stack.
	//

	// These are the parameters set by command line arguments. This does not include the maximum stack size,
	// which is the global variable 'maxStackSize' (defined in 'cwk1_extra.h') and is set in parseCommandLine() below.
	int initStackSize, numToPop, invertYesNo, rotateDepth;

	// Get the values from the command line. parseCommandLine() gives and error message and returns -1 for bad values.
	if( parseCommandLine(argc,argv,&initStackSize,&numToPop,&invertYesNo,&rotateDepth)==-1 )	
		return EXIT_FAILURE;

	// Allocates memory for the stack. Must also call 'finaliseStack' before termination.
	allocateStack();

	//
	// 1. Add multiple items to the stack in a loop. This loop needs to be parallelised as part of the coursework.
	//
	#pragma omp parallel for
	for( i=1; i<=initStackSize; i++ )
	{
		pushToStack( i * i );
	}

	// Display the initial stack. The argument '0' means this is the initial stack.
	displayStack( 0 );		// DO NOT ALTER THIS LINE - your code must call displayStack(0) immediately after the stack is populated.

	//
	// 2. Remove multiple items from the stack. This loop needs to be parallelised as part of the coursework.
	//
	
	for( i=1; i<=numToPop; i++ )
	{
		#pragma omp atomic
		popFromStack();
	}

	//
	// 3. Invert the stack.
	//
	if( invertYesNo==1 ) invertStack();

	//
	// 4. Rotate the stack.
	//
	if( rotateDepth>0 ) rotateStack( rotateDepth );

	//
	// Clean up and quit.
	//

	// Display the stack in its initial state. The argument '1' means this is the final stack
	displayStack( 1 );		// DO NOT ALTER THIS LINE - your code must call displayStack(1) just before finaliseStack().

	// You MUST call finaliseStack() at this point.
	finaliseStack();

	return EXIT_SUCCESS;
}

