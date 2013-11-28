#include <stdio.h>
#include "filesys.h"

int main()
{
   /* code */
   readdisk("virtualdisk9_11");
   format();
   writedisk("virtualdisk9_11");
   return 0;
}