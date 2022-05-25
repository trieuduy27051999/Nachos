#include "syscall.h"
#include "copyright.h"

int main()
{
  int line = 0;
  int i  =0;
  for(i =0;i<256;i++){
      line++;
      PrintInt(i);
      PrintString("   :    ");
      PrintChar(i);
      PrintString("\t");
      if(line==5){
        line = 0;
        PrintString("\n");
      }
  }
  Halt();
}
