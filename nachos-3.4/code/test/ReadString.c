#include "syscall.h"
#include "copyright.h"

int main()
{
  char buffer[256];

  ReadString(buffer,255);
  Halt();
}
