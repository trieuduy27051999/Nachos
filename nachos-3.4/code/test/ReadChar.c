#include "syscall.h"
#include "copyright.h"

int main()
{
  char c;
  c = ReadChar();
  Halt();
}
