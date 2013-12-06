#include <stdio.h>
#include "filesys.h"

int main()
{
   /* code */
   readdisk("virtualdisk9_11");
   format();
   const char* fileName = "testfileABC.txt";
   const char* mode = "w";
   Byte text[4*BLOCKSIZE] = "Hello";
   int length = strlen(text);
   MyFILE * file;
   file = myfopen(fileName, mode);
   for(int i =0; i<length; i++)
   {
   	Byte byte = text[i];
   	//printf("file's pos %d\n", text[i]);
   	myfputc ( byte, file );
   }
     	myfclose(file);

   printf("Writing disk\n");
   writedisk("virtualdisk12_14");
   return 0;
}