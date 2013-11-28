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
   
   for ( i=0; i< BLOCKSIZE; i++) block.data[i] = '\0';
   strcpy( (char*)block.data, "CS3008 blem" );
   writeblock( &block, 0);
   
	/* prepare FAT table
	 * write FAT blocks to virtual disk
	 */
   for(i = 3; i < BLOCKSIZE; ++i){
     FAT[i] = UNUSED;
    }
   FAT[1] = 2 ;
   FAT[2] = ENDOFCHAIN;
  
    for(i = 0; i < 512; i++){
     block.fat[i] = FAT[i];
    }
    writeblock(&block, 1);
    for(i = 512; i < 1024; i++){
     block.fat[i-512] = FAT[i];
    }
    writeblock(&block, 2);
   
	 /* prepare root directory
	  * write root directory block to virtual disk
	  */
  
  memset(&db,0,sizeof(dirblock_t));
  db.isdir = 1;
  db.nextEntry = 0;
  writeblock(&db, 3);

  for(i=0; i<BLOCKSIZE; i++){

    block.data[i]= 0;
    
  }
  for(i=4; i<BLOCKSIZE; i++){
    
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

