#ifdef OS_Linux
#pragma pack(1)
#endif

#ifndef QChild_H 
#define QChild_H  


#include "Semaphores.h"

#ifndef OK
  #define OK           0
  #define NOOK        -1
#endif


class QChild
{
 protected:
    int              ErrNo;
    Semaphore        Sem;
    int              *qchild;
    int              ShMemHandle;

 public:
    QChild();
    ~QChild();

   int SetQChild();
   int IncQChild();
   int DecQChild();
   int GetQChild(); 
   int Create(long SemKey, int perm);
   int Open(long SemKey);
   int Close();
   int Destroy();
   

}; 
#endif
