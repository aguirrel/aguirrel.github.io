#include "8583.h"
#include "GenResI.h"
#include <stdlib.h>

int GenCodRes(char* Res)
{
   int rn;
   int rc;

   rc=GenSem();
   srand(rc);
   rn=rand();
   switch (rn)
   {
       case 11:
	  strcpy(Res,"51");
	  break;
       case 10:
	  strcpy(Res,"11");
	  break;
       case 9:
	  strcpy(Res,"07");
	  break;
       case 8:
	  strcpy(Res,"05");
	  break; 
       case 7:
	  strcpy(Res,"04");
	  break;
       case 6:
	  strcpy(Res,"02");
	  break;
       case 5:
	  strcpy(Res,"01");
	  break;
       default:
	  strcpy(Res,"00");
   }
   return 1;
}

int GenRtaCie(Iso_Msg* ML, char* StrRes,int* len)
{
   Iso_Msg* ME;
   FILE* fd;
   int rc;
   char Aut[6];

   ME = new Iso_Msg(TYPE_VISA_CIERRE_RTA);
   ME->PutField(3,"920000");
   ME->PutField(7,ML->GetField(7));
   ME->PutField(11,ML->GetField(11));
   ME->PutField(15,ML->GetField(15));
   ME->PutField(24,ML->GetField(24));
   ME->PutField(37,"89389");

   rc=rand();
   sprintf(Aut,"%06d",rc); 
   ME->PutField(38,Aut);
   ME->PutField(39,"00");
   ME->PutField(41,ML->GetField(41));
   ME->PutField(42,ML->GetField(42)); 
   rc=ME->GetMsgString(StrRes);
   *len=rc;
   return 1;
}

int GenRtaRevDev(Iso_Msg* ML,char *StrRes, int* len)
{
   Iso_Msg* ME;
   int rc;
   char Aut[6];
   char Res[3];
   FILE *fd;

   ME = new Iso_Msg(TYPE_VISA_REV_DEVOL_RTA);
   ME->PutField(2,ML->GetField(3));
   ME->PutField(3,ML->GetField(3));
   ME->PutField(4,ML->GetField(4));
   ME->PutField(7,ML->GetField(7));
   ME->PutField(11,ML->GetField(11));
   ME->PutField(24,ML->GetField(24));
   ME->PutField(37,"00773");

   GenCodRes(Res);
   rc=rand();
   sprintf(Aut,"%06",rc);


   ME->PutField(38,Res);
   ME->PutField(39,Aut);
   ME->PutField(41,ML->GetField(41));
   ME->PutField(42,ML->GetField(42));
   ME->PutField(48,ML->GetField(48));
   ME->PutField(49,ML->GetField(49));
   rc=ME->GetMsgString(StrRes);
   *len=rc;
   fd=fopen("/online/log/X25sim","a");
   fprintf(fd,"GenRtaRevDev:: Longitud del string generado [%d]\n",*len);
   fclose(fd);
   return 1;
}

int GenRtaRevVen(Iso_Msg* ML,char *StrRes, int* len) 
{
   Iso_Msg* ME;
   int rc; 
   char Aut[6];
   char Res[3];
   FILE * fd;

   ME = new Iso_Msg(TYPE_VISA_REV_VENTA_RTA);
   ME->PutField(2,ML->GetField(3));
   ME->PutField(3,ML->GetField(3));
   ME->PutField(4,ML->GetField(4));
   ME->PutField(7,ML->GetField(7));
   ME->PutField(11,ML->GetField(11));
   ME->PutField(24,ML->GetField(24));
   ME->PutField(37,"00343");

   GenCodRes(Res);
   rc=rand();
   sprintf(Aut,"%06d",rc);

   ME->PutField(38,Res);
   ME->PutField(39,Aut);
   ME->PutField(41,ML->GetField(41));
   ME->PutField(42,ML->GetField(42));
   ME->PutField(48,ML->GetField(48));
   ME->PutField(49,ML->GetField(49));
   rc=ME->GetMsgString(StrRes);
   *len=rc;
   fd=fopen("/online/log/X25sim","a");
   fprintf(fd,"GenRtaRevVta:: Longitud del string generado [%d]\n",*len);
   fclose(fd);
   return 1;
}

int GenRtaBatUpl(Iso_Msg* ML, char *StrRes, int* len)
{
  Iso_Msg* ME;
  int rc;
  char Aut[6];
  char Res[3];
  FILE* fd;

  ME = new Iso_Msg(TYPE_VISA_BATCH_RTA);
  ME->PutField(2,ML->GetField(2));
  ME->PutField(3,ML->GetField(3));
  ME->PutField(4,ML->GetField(4));
  ME->PutField(7,ML->GetField(7));
  ME->PutField(11,ML->GetField(11));
  ME->PutField(24,ML->GetField(24));
  ME->PutField(37,ML->GetField(37));


  GenCodRes(Res);
  rc=rand();
  sprintf(Aut,"%06d",rc);

  ME->PutField(38,Aut);
  ME->PutField(39,Res);
  ME->PutField(41,ML->GetField(41));
  ME->PutField(42,ML->GetField(42));
  ME->PutField(48,ML->GetField(48));
  ME->PutField(39,ML->GetField(49));

  rc=ME->GetMsgString(StrRes);
  fd=fopen("/online/log/X25Sim","a");
  fprintf(fd,"GenRtaBatUpl:: Longitud del string generado [%d]\n",rc);
  fclose(fd);
  *len=rc;
  return 1;
}


int GenRtaDev(Iso_Msg* ML, char *StrRes,int* len)
{
    char Res[3];
    char Aut[7];
    char buffer[200];
    int rc;
    FILE* fd;
    Iso_Msg* ME;

    ME = new Iso_Msg(TYPE_VISA_DEVOL_RTA);

    strcpy(buffer,ML->GetField(2));
    ME->PutField(2,buffer);

    ME->PutField(3,"200000");
    ME->PutField(4,ML->GetField(4));
    ME->PutField(7,ML->GetField(7));
    ME->PutField(11,ML->GetField(11));
    ME->PutField(24,ML->GetField(24));
    ME->PutField(37,"98392");
    rc=rand();
    sprintf(Aut,"%06d",rc);
    ME->PutField(38,Aut);
    GenCodRes(Res);
    ME->PutField(39,Res);
    ME->PutField(41,ML->GetField(41));
    ME->PutField(42,ML->GetField(42));
    ME->PutField(48,ML->GetField(48));
    ME->PutField(49,ML->GetField(49));
    rc=ME->GetMsgString(StrRes); 
    fd=fopen("/online/log/X25Sim","a");
    fprintf(fd,"GenRtaDev:: Longitud del string creado: [%d]",rc);
    fclose(fd);
    *len=rc;
    return 1;
}


int ObtHora(char * in)
{
   char buffer[30];
   FILE* pd;

   pd=popen("date +%H%M%S","r");
   fread(buffer,6,1,pd);
   memcpy(in,buffer,6);
   buffer[6]='0';
   pclose(pd);
   return 1;
}

int ObtFech(char *in)
{
   char buffer[30];
   FILE* pd;

   pd=popen("date +%m%d","r");
   fread(buffer,6,1,pd);
   memcpy(in,buffer,6);
   buffer[6]='0';
   pclose(pd);
   return 1;
}

int GenRtaEchTst(Iso_Msg* ML,char * StrRes,int* len)
{
   FILE* fd;
   int rc;
   Iso_Msg* ME;
   char hora[7];
   char fech[5];

   ObtHora(hora); 
   ObtFech(fech);
   ME = new Iso_Msg(TYPE_VISA_TEST_RTA);
   ME->PutField(3,"990000");
   ME->PutField(12,ML->GetField(12));
   ME->PutField(13,ML->GetField(13));
   ME->PutField(24,ML->GetField(24)); 
   ME->PutField(41,ML->GetField(41));

   rc=ML->GetMsgString(StrRes);
   *len=rc;
   return 1;
}


int GenRtaVta(Iso_Msg* ML, char* StrRes,int* len) 
{
   char Res[3];
   char Aut[7];
   char buffer[150];
   FILE* fd;
   int rc;
   Iso_Msg* ME;

   ME = new Iso_Msg(TYPE_VISA_VENTA_RTA);

   strcpy(buffer,ML->GetField(2));
   ME->PutField(2,buffer);

   ME->PutField(3,"000000");
   ME->PutField(4,ML->GetField(4));
   ME->PutField(7,ML->GetField(7));
   ME->PutField(11,ML->GetField(11));
   ME->PutField(24,ML->GetField(24));
   ME->PutField(37,"222233");
   GenCodRes(Res);
   rc=rand();
   sprintf(Aut,"%06d",rc);
   ME->PutField(38,Aut);
   ME->PutField(39,Res);
   ME->PutField(41,ML->GetField(41));
   ME->PutField(42,ML->GetField(42));
   ME->PutField(48,ML->GetField(48));
   ME->PutField(49,ML->GetField(49));
   rc=ME->GetMsgString(StrRes);
   fd=fopen("/online/log/X25Sim","a");
   fprintf(fd,"GenRtaDev:: Longitud del string creado: [%d]",rc);
   fclose(fd);
   *len=rc;
   return 1;
} 

int GenRtaVtaLoc(Iso_Msg* ML,char* StrRes,int* len)
{
  char Aut[7]; 
  char Res[3]; 
  int rc;
  Iso_Msg* ME;
  FILE* fd;

  ME = new Iso_Msg(TYPE_VISA_VENTA_LOCAL_RTA);
  ME->PutField(2,ML->GetField(2));
  ME->PutField(3,"200000");
  ME->PutField(4,ML->GetField(4));
  ME->PutField(7,ML->GetField(7));
  ME->PutField(11,ML->GetField(11));
  ME->PutField(24,ML->GetField(24));
  ME->PutField(37,"834833");
  rc=rand();
  sprintf(Aut,"%06d",rc);
  ME->PutField(38,Aut);
  GenCodRes(Res);
  ME->PutField(39,Res);
  ME->PutField(41,ML->GetField(41));
  ME->PutField(42,ML->GetField(42));
  ME->PutField(48,ML->GetField(48));
  ME->PutField(49,ML->GetField(49));
  rc=ME->GetMsgString(StrRes);
  fd=fopen("/online/log/X25Sim","a");
  fprintf(fd,"GenRtaDev:: Longitud del string creado: [%d]",rc);
  fclose(fd);
  *len=rc;
  return 1;
}


int GenRtaDevLoc(Iso_Msg* ML,char* StrRes,int* len) 
{
   int rc;
   char Aut[7];
   char Res[3];
   Iso_Msg* ME;
   FILE* fd;
 
   ME= new Iso_Msg(TYPE_VISA_DEVOL_LOCAL_RTA);

   ME->PutField(2,ML->GetField(2));
   ME->PutField(3,"200000");
   ME->PutField(4,ML->GetField(4));
   ME->PutField(7,ML->GetField(7));
   ME->PutField(11,ML->GetField(11));
   ME->PutField(24,ML->GetField(24));
   ME->PutField(37,"93849");
   rc=rand();
   sprintf(Aut,"%06d",rc);
   ME->PutField(38,Aut);
   GenCodRes(Res);
   ME->PutField(39,Res);
   ME->PutField(42,ML->GetField(42));
   ME->PutField(48,ML->GetField(48));
   ME->PutField(49,ML->GetField(49));
   rc=ME->GetMsgString(StrRes);
   fd=fopen("/online/log/X25Sim","a");
   fprintf(fd,"GenRtaDev:: Longitud del string creado: [%d]",rc);
   fclose(fd);
   *len=rc;
   return 1; 
}

int GenSem() 
{
   FILE* pd;
   char Sec[3];
   
   pd=popen("date +%S","r");
   fread(Sec,3,1,pd);
   pclose(pd);
   return atoi(Sec);
}
