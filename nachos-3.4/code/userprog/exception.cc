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
#define MAX_LENGTH  255
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

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char* User2System(int virtAddr, int limit)
{
	int i; //chi so index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //can cho chuoi terminal
	if (kernelBuf == NULL)
		return kernelBuf;

	memset(kernelBuf, 0, limit + 1);

	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do {
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

// Inscrease program counter
// Pre-PC register assigned by PC register
// PC register assigned by Next-PC register
// Next-PC resgister assigned by 4-byte ahead register
void IncreasePC()
{
	machine->registers[PrevPCReg] = machine->registers[PCReg];
	machine->registers[PCReg] = machine->registers[NextPCReg];
	machine->registers[NextPCReg] += 4;
}



void ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);

	switch (which) {
	case NoException:
		interrupt->Halt();
		return;

	case PageFaultException:
		printf("\nNo valid translation found.\n");
		ASSERT(FALSE);
		break;

	case ReadOnlyException:
		printf("\nWrite attempted to page marked \"read-only\".\n");
		ASSERT(FALSE);
		break;

	case BusErrorException:
		printf("\nTranslation resulted in an invalid physical address.\n");
		ASSERT(FALSE);
		break;

	case AddressErrorException:
		printf("\nUnaligned reference or one that was beyond the end of the address space.\n");
		ASSERT(FALSE);
		break;

	case OverflowException:
		printf("\nInteger overflow in add or sub.\n");
		ASSERT(FALSE);
		break;

	case IllegalInstrException:
		printf("\nUnimplemented or reserved instr\n");
		ASSERT(FALSE);
		break;

	case NumExceptionTypes:
		printf("\nNumExceptionTypes\n");
		ASSERT(FALSE);
		break;


	case SyscallException:
		switch (type) {

		case SC_Halt:
		{
			printf("\nShutdown, initiated by user program.\n");
			interrupt->Halt();
			return;
		}
		case SC_ReadInt:
		{

            char * buffer = NULL;
            buffer = new char[MAX_LENGTH +1];

			int lenOfNumber = gSynchConsole->Read(buffer,MAX_LENGTH);
			int firstIndex = 0;
			bool isNeg = false;
			if(buffer[0] == '-'){
                firstIndex = 1;
                isNeg =true;
			}
			for(int i=firstIndex; i<lenOfNumber; i++)
                {
                    if(buffer[i]<'0'||buffer[i]>'9'){
                        printf("This is not a valid number!!");
                        machine->WriteRegister(2,0);
                        IncreasePC();
                        delete buffer;
                        break;
                    }
                }
            int number =0;
            for(int i = firstIndex;i<lenOfNumber;i++){
                number =number*10 + int(buffer[i]-48);
            }
            if (isNeg) number = number*-1;
            machine->WriteRegister(2,number);
            printf("Read number successfull!");
            IncreasePC();
            delete buffer;
			break;

		}
		case SC_PrintInt:
		{

            int number=0;
            number = machine->ReadRegister(4);
            if (number==0)
            {
                gSynchConsole->Write("0",1);
                IncreasePC();
                return;
            }
            bool isNeg=false;
            int Numlen = 0;
            int firstIndex =0;

            if(number< 0){
                isNeg =true;
                firstIndex =1;
                number = number *-1;

            }
            int temp = number;
            while(temp>0){
                Numlen++;
                temp/=10;
            }
            char* buffer = new char [MAX_LENGTH+1];
            for (int i = firstIndex + Numlen -1; i>= firstIndex; i--)
            {
                buffer[i]=(char)((number%10)+48);
                number/=10;
            }
            if(isNeg)
            {
                buffer[0]='-';
                buffer[Numlen+1]='\0';
                gSynchConsole->Write(buffer,Numlen+1);
                delete buffer;
                IncreasePC();
                return;
            }
            buffer[Numlen]='\0';
            gSynchConsole->Write(buffer,Numlen);
            delete buffer;
            IncreasePC();
            return;


		}
		case SC_ReadChar:
		{

			break;
		}
		case SC_PrintChar:
		{
			char ch;
			ch = (char) machine->ReadRegister(4);
			gSynchConsole->Write(&ch, 1);
			break;
		}
		case SC_ReadString:
		{

			break;
		}
		case SC_PrintString:
		{
			char ch;
			ch = (char) machine->ReadRegister(4);
			gSynchConsole->Write(&ch, 1);
			break;
		}

		IncreasePC();
		break;
	}
	}





	return;
}
