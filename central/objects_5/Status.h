#ifdef OS_Linux
#pragma pack(1)
#endif

#include "Semaphores.h"

#ifndef OK
  #define OK	0
  #define NOOK	-1
#endif 

class CaStatus
{
   protected:
      int	status;
      Semaphore Sem;

   public:
     inline CaStatus() {status=OK;} ;
     int Open(long SemId, int Perm);
     int Status();
     int SetCaDown();
     int SetCaUp();
	 int Destroy();
};
