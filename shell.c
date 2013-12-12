#include <stdio.h>
#include "filesys.h"

int main()
{
   /* code */
   readdisk("virtualdisk9_11");
   format();
   //-----WRITE------------------------------------
   
   Byte text[4*BLOCKSIZE] = "ABaaaBABASad";
   MyFILE * file;
   file = myfopen("testfileABC.txt", "w");
   for(int i =0; i<4*BLOCKSIZE; i++)
   {
   	Byte byte = text[i];
   	//printf("file's pos %d\n", text[i]);
   	myfputc ( byte, file );
   }
     	myfclose(file);
 //---------GET------------------------------------
   //const char* fileName = "testfileABC.txt";
   //const char* mode = "r";
   MyFILE * file2;
   file2 = myfopen("testfileABC.txt", "r");
   for(int i =0; i<4*BLOCKSIZE; i++){
      Byte data = myfgetc(file2);
      FILE * f;
      f = 
      //printf("bytes %d\n", data);
   }

   //---------------------------------------------
   printf("Writing disk\n");
   writedisk("virtualdisk12_14");
   return 0;
}