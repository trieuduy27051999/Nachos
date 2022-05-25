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
                // Dau vao: void
                //Dau ra : tra ve mot so nguyen
                //Muc dich: Doc mot so nguyen tu console.

                char * buffer = NULL;  // Mang ki tu luu tru cac chu so cua so nguyen nhan duoc.
                buffer = new char[MAX_LENGTH +1];// do dai toi da la 255 chu so

                int lenOfNumber = gSynchConsole->Read(buffer,MAX_LENGTH);// so ki tu doc duoc tu console
                int firstIndex = 0;
                bool isNeg = false;
                if(buffer[0] == '-'){
                    firstIndex = 1;
                    isNeg =true;  // Neu la so am thi doc tu phan tu thu 1
                }
                for(int i=firstIndex; i<lenOfNumber; i++)//Kiem tra xem so co hop le khong, neu khong tra ve 0
                    {
                        if(buffer[i]<'0'||buffer[i]>'9'){
                            printf("So da nhap khong hop le!!");
                            machine->WriteRegister(2,0);
                            IncreasePC();
                            delete buffer;
                            //interrupt->Halt();
                            break;
                        }
                    }
                int number =0;
                for(int i = firstIndex;i<lenOfNumber;i++){ // Chuyen day ki tu thanh so
                    number =number*10 + int(buffer[i]-48);
                }
                if (isNeg) number = number*-1;
                machine->WriteRegister(2,number); //Ghi ra thanh ghi so 2 gia tri number
                //printf("Read number successfull!");
                IncreasePC();
                delete buffer;
                //interrupt->Halt();
                break;


            }
            case SC_PrintInt:
            {
                //Dau vao: Mot so nguyen
                //Dau ra: Khong
                //Muc dich: In so nguyen ra console
                int number=0;
                number = machine->ReadRegister(4); // Doc so can tu ghi tu thanh ghi so 4
                if (number==0)
                {
                    gSynchConsole->Write("0",1); // Ghi ra so 0
                    IncreasePC();
                    break;
                }
                bool isNeg=false;
                int Numlen = 0;  // So chu so cua so nguyen can in.
                int firstIndex =0;

                if(number< 0){ //Neu so nguyen < 0 thi ki tu dau tien cua chuoi in ra la '-'
                    isNeg =true;
                    firstIndex =1;
                    number = number *-1;

                }
                int temp = number; // Dem so chu so cua so nguyen
                while(temp>0){
                    Numlen++;
                    temp/=10;
                }
                char* buffer = new char [MAX_LENGTH+1]; // Mang luu so nguyen dang chuoi
                for (int i = firstIndex + Numlen -1; i>= firstIndex; i--)// Chuyen so nguyen thanh chuoi
                {
                    buffer[i]=(char)((number%10)+48);
                    number/=10;
                }
                if(isNeg)//Neu la so am thi ki tu dau tien la '-'
                {
                    buffer[0]='-';
                    buffer[Numlen+1]='\0';
                    gSynchConsole->Write(buffer,Numlen+1);//In chuoi so nguyen ra man hinh console
                    //printf("Print Int Successful!");
                    delete buffer;
                    IncreasePC();
                    //interrupt->Halt();
                    break;
                }
                buffer[Numlen]='\0';//
                gSynchConsole->Write(buffer,Numlen);
                //printf("Print Int Successful!");
                delete buffer;

                IncreasePC();
                //interrupt->Halt();
                break;


            }
            case SC_ReadChar:
            {
                //Dau vao: khong
                //Dau ra: 1 ki tu.
                //Chuc Nang: Doc 1 ki tu tu console
                char * buffer = NULL;//Chuoi luu gia tri doc duoc tu console voi do dai toi da 255 ki tu
                buffer = new char[MAX_LENGTH +1];

                int lenOfBuffer = gSynchConsole->Read(buffer,MAX_LENGTH); //So ki tu doc duoc tu console

                if(lenOfBuffer== 0){//Neu khong doc duoc ki tu nao
                    printf("Ki tu rong!");
                    machine->WriteRegister(2,0);
                    break;
                }
                if(lenOfBuffer> 1){//Neu doc duoc nhieu hon 1 ki tu
                    printf("Chi duoc nhap mot ki tu !");
                    machine->WriteRegister(2,0);
                    break;
                }


                char c = buffer[0];
                machine->WriteRegister(2,c); // Ghi ki tu doc duoc xuong thanh ghi so 2.
                printf("Read char successful!");
                delete buffer;
                interrupt->Halt();
                break;
            }
            case SC_PrintChar:
            {
                //Dau vao : 1 ki tu kieu char
                //Dau ra: Khong
                //Chuc Nang:In mot ki tu ra console
                char ch;
                ch = (char) machine->ReadRegister(4);//Lay ki tu can in ra console tu thanh ghi so 4
                gSynchConsole->Write(&ch, 1);//In ki tu ra console
                //printf("\n Print char successful!");
                IncreasePC();
                //interrupt->Halt();
                break;
            }
            case SC_ReadString:
            {
                //Dau vao: Chuoi buffer luu gia tri doc duoc, do dai chuoi can doc
                //Dau ra: Khong
                //Chuc nang: Doc chuoi tu man hinh console
                char * buffer;
                int len, virtAdd; //
                virtAdd = machine->ReadRegister(4);//Lay dia chi tu thanh ghi so 4
                len = machine->ReadRegister(5);// lay do dai ki tu toi da tu thanh ghi so 5
                buffer = User2System(virtAdd,len);// copy chuoi tu vung nho user sang vung nho system
                gSynchConsole->Read(buffer,len);//Doc chuoi ki tu tu console
                System2User(virtAdd,len,buffer);//Copy chuoi tu vung nho System sang vung nho User
                //printf("Read String successful!");
                delete buffer;
                //interrupt->Halt();
                IncreasePC();
                break;
            }
            case SC_PrintString:
            {
                //Dau vao: Chuoi ki tu can in ra console
                //Dau ra : khong
                //Muc dich: In chuoi ki tu ra console
                char * buffer;
                int len, virtAdd;
                virtAdd = machine->ReadRegister(4); // Doc dia chi tu thanh ghi so 4
                len = 0;

                buffer = User2System(virtAdd,MAX_LENGTH);//Copy chuoi tu vung nho user sang vung nho system
                while(buffer[len]!= '\0') len++;// Dem so luong ki tu cua chuoi
                gSynchConsole->Write(buffer,len+1);//In chuoi ki tu ra console
                delete buffer;
                IncreasePC();
                //interrupt->Halt();

            }


	    }
	}





	return;
}
