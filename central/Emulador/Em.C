#include "Queues.h"
#include "Log.h"
#include "Config.h"
#include "8583.h"
#include <stdlib.h>


main()
{
   Log		LogAlarm;
   Config	CfgCred("credito.ini");
   char		buffer1[500];
   char		buffer2[500];
   Queue	XQueue,EQueue;
   long		QueueID,QueuePerm;
   struct	tm *newtime;
   time_t	ltime;
   int 		ret;
   int 		len;
   Iso_Msg	*IsoMsg;
   Iso_Msg	*IsoRta;

   LogAlarm.Open("../log/Em.log");
   LogAlarm.SetLevel(10);

   CfgCred.GetItem("CAVisaDaemon","QueueID",buffer1);
   QueueID=atol(buffer1);
   CfgCred.GetItem("CAVisaDaemon","QueuePerms",buffer2);
   QueuePerm=atol(buffer2);
   ret=XQueue.Create(QueueID,QueuePerm);

   if (ret!=OK)
      exit(ret); 

   CfgCred.GetItem("Daemon","QueueID",buffer1);
   QueueID=atol(buffer1);
   CfgCred.GetItem("Daemon","QueuePerms",buffer2);
   QueuePerm=atol(buffer2);
   ret=EQueue.Open(QueueID,QueuePerm);
   if (ret!=OK)
   { 
      exit(ret); 
   }
   
   time(&ltime); 
   newtime=localtime(&ltime);
   srandom(newtime->tm_sec);
   LogAlarm.Put(1,"Comenzando Operacion %s\n",asctime(newtime)); 

   while (ret==OK)
   {
	len=XQueue.GetMsg(0, buffer1, sizeof(buffer1));
	if (len==NOOK)
	{
	   break;
	}

	IsoMsg=new Iso_Msg(buffer1,len);

	switch (IsoMsg->GetMsgType())
	{
	   case TYPE_VISA_VENTA:
	       LogAlarm.Put(1,"Recibido PriceOn [%s] [%s]\n",
		      IsoMsg->GetField(41),IsoMsg->GetField(11));
	       IsoRta = new Iso_Msg(TYPE_VISA_VENTA_RTA);
	       IsoRta->PutField(2,IsoMsg->GetField(2));
	       IsoRta->PutField(11, IsoMsg->GetField(11));
	       IsoRta->PutField(24, IsoMsg->GetField(24));

	       sprintf(buffer1,"%012%d",random());
	       IsoRta->PutField(37,buffer1);

	       sprintf(buffer1,"%06d",random());
	       IsoRta->PutField(38,buffer1);

	       IsoRta->PutField(39,"00");
	       IsoRta->PutField(41, IsoMsg->GetField(41));

	       len=IsoRta->GetMsgString(buffer1);

	       ret=EQueue.SendMsg(EVENT_PRICEONLINE_RTA,buffer1,len);

               break;
        }

   }
}






 

