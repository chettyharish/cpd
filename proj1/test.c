#include <stdio.h>
#include <stdlib.h>

main()
{
  printf("%s: tsize = %s, myid = %s\n", getenv("HOST"),
	 getenv("TSIZE"), getenv("MYID"));
}
