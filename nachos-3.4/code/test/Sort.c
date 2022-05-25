#include "syscall.h"
#include "copyright.h"

int main()
{
  int n =0;
  int i=0;
  int j =0;
    int k = 0;
  int temp = 0;
  int array[100];
  PrintString("Nhap n, (n<=100) \n");

  n = ReadInt();
  while(n>100||n<=0){
     PrintString("N khong hop le, vui long nhap lai \n");
     n=ReadInt();
  }

  for(i = 0;i<n;i++){
      PrintString("Nhap phan tu thu ");
      PrintInt(i);
      PrintChar(':');
      array[i]=ReadInt();
      PrintString("\n");
  }
  i=0;


  for(i = 0;i<n-1;i++){
      j=0;
      for(;j<n-i-1;j++){
            if(array[j]>array[j+1]){
                temp =array[j];
                array[j]=array[j+1];
                array[j+1]=temp;
            }
      }
  }
  PrintString("Mang sau khi da sap xep : \n");

  for(;k<n;k++){
     PrintInt(array[k]);
     PrintChar(' ');
  }
  Halt();

}
