/* filesys.c
 * 
 * provides interface to virtual disk
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "filesys.h"


diskblock_t  virtualDisk [MAXBLOCKS] ;           // define our in-memory virtual, with MAXBLOCKS blocks
fatentry_t   FAT         [MAXBLOCKS] ;           // define a file allocation table with MAXBLOCKS 16-bit entries
fatentry_t   rootDirIndex            = 0 ;       // rootDir will be set by format
direntry_t * currentDir              = NULL ;
fatentry_t   currentDirIndex         = 0 ;

/* writedisk : writes virtual disk out to physical disk
 * 
 * in: file name of stored virtual disk
 */


void writedisk ( const char * filename )
{
   printf ( "writedisk> virtualdisk[0] = %s\n", virtualDisk[0].data ) ;
   FILE * dest = fopen( filename, "w" ) ;
   if ( fwrite ( virtualDisk, sizeof(virtualDisk), 1, dest ) < 0 )
      fprintf ( stderr, "write virtual disk to disk failed\n" ) ;
   //write( dest, virtualDisk, sizeof(virtualDisk) ) ;
   fclose(dest) ;
   
}

void readdisk ( const char * filename )
{
   FILE * dest = fopen( filename, "r" ) ;
   if ( fread ( virtualDisk, sizeof(virtualDisk), 1, dest ) < 0 )
      fprintf ( stderr, "write virtual disk to disk failed\n" ) ;
   //write( dest, virtualDisk, sizeof(virtualDisk) ) ;
      fclose(dest) ;
}
void readblock(diskblock_t * block, int block_address){
    memmove(block->data, virtualDisk[block_address].data, MAXBLOCKS);
}

/* the basic interface to the virtual disk
 * this moves memory around
 */

void writeblock ( diskblock_t * block, int block_address )
{
   //printf ( "writeblock> block %d = %s\n", block_address, block->data ) ;
   memmove ( virtualDisk[block_address].data, block->data, BLOCKSIZE ) ;
   //printf ( "writeblock> virtualdisk[%d] = %s / %d\n", block_address, virtualDisk[block_address].data, (int)virtualDisk[block_address].data ) ;
}
/* OPEN file with given parameters: filename and mode.
    *Search in root directory if file with given filename exists.
    * If so, record files index from entry list to fileindex variable and
    * If file's mode is readable:
    * read disk block and copy it to files buffer(disk block).
    * set files possition(byte within block) to 0 .
    * return file.
    * If file with given name is not located in root directory:
    * get next awailable entry in root directory's entrylist
    * and assign it's value to fileindex variable and increase next awailable entry by one.
    * preapare directory entry.
    * set entrylength, set unused to FALSE, isdir to TRUE, copy files name to name.
    * write directory block.
    * return file.
    */
MyFILE * myfopen(char * filename, char* mode)
{

  MyFILE *file = malloc(sizeof(MyFILE));
  diskblock_t buffer;
  dirblock_t rootDir;
  (*file).blockno = 4;
  readblock(&buffer,3);
  rootDir = buffer.dir;


  int fileIndex = -1;
  
  for(int i = 0; i<rootDir.nextEntry && fileIndex == -1;i++)
  {
    if(strcmp(rootDir.entrylist[i].name, filename)==0)
      {
        fileIndex = i;
        if(strcmp(mode, "r")==0){
          (*file).buffer = buffer;
          (*file).pos = 0;
          return file;
        }
      }
  }
  if(fileIndex == -1)
  {
    fileIndex = rootDir.nextEntry;
    rootDir.nextEntry++;
  }
  rootDir.entrylist[fileIndex].isdir = TRUE;
  rootDir.entrylist[fileIndex].unused = FALSE;
  rootDir.entrylist[fileIndex].entrylength = strlen(filename);
  strcpy(rootDir.entrylist[fileIndex].name, filename);
  writeblock(&rootDir, 3);
  return file;
}
/* PUT file's content to file piece by piece (look in shell.c).
  * get current file's possition(byte within block) and check if it is smaller or equal to BLOCKSIZE.
  * if so put byte(look in shell.c) to block(files buffer) by file's current possition. increase possition by 1.
  * if current file's possition(byte within block) is grater then BLOCKSIZE:
  *    Set current file's possition(byte within block) to 0.
  *    Write block by blockno(blocknumber whic is currently 4) to virtual disk and increase nlockno by 1.
  *    Delete buffer's content by setting it's content to 0.
  *    Update FAT: 
  *       set FAT block chain
  *       write FAT blocks 1 and 2 to virtual disk
  */
void myfputc ( Byte byte, MyFILE *file )
{
  int filepos = (*file).pos;
    if(filepos <= BLOCKSIZE){
      (*file).buffer.data[filepos] = byte;
      filepos= ++(*file).pos;
    }else{
      int tempblnr = (*file).blockno;
      (*file).blockno ++;
      (*file).pos = 0;
      writeblock(&(*file).buffer, tempblnr);
      memset(&(*file).buffer,0,sizeof(diskblock_t));
      diskblock_t block ;
      readblock(&block,1);
      diskblock_t block2 ;
      readblock(&block2,2);
      for(int i = tempblnr; i < BLOCKSIZE; i++){
        FAT[i] = UNUSED;
      }
      FAT[tempblnr] = (*file).blockno ;
      FAT[(*file).blockno] = ENDOFCHAIN;
      
      for(int i = 0; i < 512; i++){
        block.fat[i] = FAT[i];
      }
      writeblock(&block, 1);
      for(int i = 512; i < 1024; i++){
         block2.fat[i-512] = FAT[i];
      }
      writeblock(&block2, 2);
    }
  
}
/* GET file's content from file piece by piece (look in shell.c).
  * get current file's possition(byte within block) and check if it is smaller or equal to BLOCKSIZE.
  * if so get byte(look in shell.c) from block(files buffer) by file's current possition. increase possition by 1.
  * if current file's possition(byte within block) is grater then BLOCKSIZE:
  *    Set current file's possition(byte within block) to 0.
  *    Delete buffer's content by setting it's content to 0.
  *    Read block by blockno(blocknumber whic is currently 4) so we have to increase blockno by 1.
  *    get byte(look in shell.c) from block(files buffer) by file's current possition.
  * return that byte.
  */
Byte myfgetc(MyFILE *file)
{   fatentry_t num;
    Byte Buffdata;
    int filepos = (*file).pos;
    if(filepos <= BLOCKSIZE){
      Buffdata = (*file).buffer.data[filepos];
      filepos= ++(*file).pos;
    }else{
      memset(&(*file).buffer,0,sizeof(diskblock_t));
      (*file).pos = 0;
      num = (*file).blockno++;
      readblock(&(*file).buffer,num);
      Byte Buffdata = (*file).buffer.data[filepos];
      
    }
    return Buffdata;
}
/* CLOSE file
  *free allocated memory for file.
  */
void myfclose(MyFILE *file)
{
  free(file);
}


/* read and write FAT
 * 
 * please note: a FAT entry is a short, this is a 16-bit word, or 2 bytes
 *              our blocksize for the virtual disk is 1024, therefore
 *              we can store 512 FAT entries in one block
 * 
 *              how many disk blocks do we need to store the complete FAT:
 *              - our virtual disk has MAXBLOCKS blocks, which is currently 1024
 *                each block is 1024 bytes long
 *              - our FAT has MAXBLOCKS entries, which is currently 1024
 *                each FAT entry is a fatentry_t, which is currently 2 bytes
 *              - we need (MAXBLOCKS /(BLOCKSIZE / sizeof(fatentry_t))) blocks to store the
 *                FAT
 *              - each block can hold (BLOCKSIZE / sizeof(fatentry_t)) fat entries
 */

/* implement format()
 */
void format ( )
{
   diskblock_t block ;
   direntry_t  rootDir ;
   dirblock_t db;
   
   int         pos             = 0 ;
   int         fatentry        = 0 ;
   int         fatblocksneeded =  (MAXBLOCKS / FATENTRYCOUNT ) ;

   /* prepare block 0 : fill it with '\0',
    * use strcpy() to copy some text to it for test purposes
	* write block 0 to virtual disk
	*/
  int i;
  for ( i=0; i< BLOCKSIZE; i++){ block.data[i] = '\0';}
  strcpy( (char*)block.data, "CS3008 blem" );
  writeblock( &block, 0);
   
	/* prepare FAT table: fill it with UNUSED,
   * set FAT own block chain
	 * write FAT blocks 1 and 2 to virtual disk
	 */
   for(i = 3; i < BLOCKSIZE; ++i){
     FAT[i] = UNUSED;
    }
    FAT[1] = 2 ;
    FAT[2] = ENDOFCHAIN;
    FAT[3] = ENDOFCHAIN;
    for(i = 0; i < 512; i++){
     block.fat[i] = FAT[i];
    }
    writeblock(&block, 1);
    for(i = 512; i < 1024; i++){
     block.fat[i-512] = FAT[i];
    }
    writeblock(&block, 2);
   
	 /* prepare root directory
    * fill all block to 0
	  * write root directory block to virtual disk
	  */
  
  memset(&db,0,sizeof(dirblock_t));
  db.isdir = 1;
  db.nextEntry = 0;
  writeblock(&db, 3);
/* format rest of blocks
    * fill all block to 0
    * write blocks to virtual disk
    */
  for(i=0; i<BLOCKSIZE; i++){

    block.data[i]= 0;
    
  }
  for(i=3; i<BLOCKSIZE; i++){
    
    writeblock(&block, i);
    
  }

  
}

/* use this for testing
 */


void printBlock ( int blockIndex )
{
   //printf ( "New Block\n") ;
   printf ( "virtualdisk[%d] = %s\n", blockIndex, virtualDisk[blockIndex].data ) ;
}

