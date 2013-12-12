#include <stdio.h>
#include "filesys.h"

int main()
{
   /* code */
   readdisk("virtualdisk9_11");
   format();
   //-----WRITE------------------------------------
   
   Byte text[4*BLOCKSIZE] = "ABaaaBABASad asdasasdas asdasdasdasdasd";
   MyFILE * file;
   file = myfopen("testfile.txt", "w");
   for(int i =0; i<4*BLOCKSIZE; i++)
   {
   	Byte byte = text[i];
   	//printf("file's pos %d\n", text[i]);
   	myfputc ( byte, file );
   }
     	myfclose(file);
 //---------GET------------------------------------
   
   MyFILE * file2;
   FILE *fp;
   fp=fopen("file.txt","w");
   file2 = myfopen("testfile.txt", "r");
   for(int i =0; i<4*BLOCKSIZE; i++){
      Byte data = myfgetc(file2);
      putc(data,fp);
   } 
   fclose(fp);
   //---------------------------------------------
   printf("Writing disk\n");
   writedisk("virtualdisk12_14");
   return 0;
}