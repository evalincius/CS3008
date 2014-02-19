/* shell.c
 * 
 * provides main method for program
 * 
 * By Edgaras Valincius
 * Student Id. 51121624
 */
#include <stdio.h>
#include "filesys.h"

int main()
{
   /* code */
   readdisk("virtualdisk9_11");
   format();
   writedisk("virtualdisk9_11");
   //-----WRITE------------------------------------
   
   Byte text[4*BLOCKSIZE] = "This is test string";
   MyFILE * file;
   file = myfopen("testfile.txt", "w"); // opens virtual file
   for(int i =0; i<4*BLOCKSIZE; i++) 
   {
   	Byte byte = text[i]; // iterates through text which is content to put to file
   	myfputc ( byte, file ); // puts byte to file.
   }
    myfclose(file); 	// close file.
 //---------GET------------------------------------
   
   MyFILE * file2;
   FILE *fp; 
   fp=fopen("testfile12_14_copy.txt","w"); // opens real file
   file2 = myfopen("testfile.txt", "r"); // opens virtual file
   for(int i =0; i<4*BLOCKSIZE; i++){
      Byte data = myfgetc(file2); //gets data byte prom file
      putc(data,fp); //puts it to real file 
   } 
   fclose(fp); // close real file
   //---------------------------------------------
   printf("Writing disk\n");
   writedisk("virtualdisk12_14");
   return 0;
}