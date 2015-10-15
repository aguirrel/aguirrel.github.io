static char sccsid[] = "@(#)24	1.1  src/sx25/usr/samples/sx25/comio/svcxmit.c, libx25l, x25sw420, 9527A 12/8/89 14:37:04";
/* COMPONENT_NAME: (LIBX25L) X.25 API Sample programs
 *
 * FUNCTIONS:
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <NLchar.h>
#include <x25sdefs.h>


#define LINK_NAME    "x25s0"            /* Default link                 */
#define CALLING_ADDR "54321"            /* Calling Network User Address */
#define CALLED_ADDR  "1234502"          /* Called  Network User Address */

#define SAMPLE_NAME "IBMSAMP"           /* Name from routing list       */

#define INFO        "Hello World"
#define INFO2       "Goodbye Everyone"

/*****************************************************************************/
/* Function     main                                                         */
/* Description  This program is designed to demonstrate usage of the X.25    */
/*              API. It makes a call, transmits some data, and then clears   */
/*              the call.                                                    */
/*              Note that little error checking is done by this program.     */
/* Return       0 if successful                                              */
/*              1 if error occurs                                            */
/*****************************************************************************/
int main(
  int argc,
  char *argv[])
{
  int conn_id;        /* Used to identify the call after making the call.    */
  int ctr_id;         /* Counter identifier to be associated with this call. */
  int rc;             /* Used for return codes.                              */
  int ctr_num = 1;    /* The number of counters in counter array.            */

  /***************************************************************************/
  /* The following structures are defined in the x25sdefs.h file.            */
  /***************************************************************************/
  struct cb_msg_struct cb_msg;
  struct cb_link_name_struct cb_link_name;
  struct ctr_array_struct ctr_array[1];
  struct cb_call_struct cb_call;
  struct cb_clear_struct cb_clear;
  struct cb_data_struct cb_data;

  /***************************************************************************/
  /* Initialise API for access to a link                                     */
  /***************************************************************************/
  cb_link_name.flags = X25FLG_LINK_NAME;
  cb_link_name.link_name = LINK_NAME;
  rc = x25_init(&cb_link_name);
  if (rc < 0)
  {
    (void)printf("%s: x25_init failed : x25_errno = %d errno = %d\n",
	argv[0],x25_errno,errno);
    return(1);
  }
  else
  {
    /************************************************************************/
    /* Obtain a counter to notify of incoming packets                       */
    /************************************************************************/
    ctr_id = x25_ctr_get();

    /************************************************************************/
    /* Set the flags in the cb_call structure to indicate which fields      */
    /* have been filled in. The fields which this program sets              */
    /* are the calling and called addresses, and the link on which to call. */
    /* The D-bit field must also be set, as there will be a data packet     */
    /* sent later which uses the D-bit procedure.                           */
    /************************************************************************/
    cb_call.flags = X25FLG_LINK_NAME;      /* Set flag for using linkname.  */
    cb_call.link_name = LINK_NAME;

    cb_call.flags |= X25FLG_CALLING_ADDR;  /* Set flags for calling addr    */
    cb_call.calling_addr = CALLING_ADDR;

    cb_call.flags |= X25FLG_CALLED_ADDR;   /* Set flag for called address   */
    cb_call.called_addr = CALLED_ADDR;

    cb_call.flags |= X25FLG_D_BIT;
    /************************************************************************/
    /* Having set the values in the cb_call structure, make the call. The   */
    /* return code is the connection identifier to reference this call later*/
    /************************************************************************/
    conn_id = x25_call(&cb_call,ctr_id);
    if (conn_id == -1)
    {
      (void)printf("%s: x25_call failed : x25_errno = %d errno = %d\n",
	argv[0],x25_errno,errno);
      return(1);
    }
    else
      (void)printf("%s: Placed outgoing call\n",argv[0]);
    /************************************************************************/
    /* After making the connection wait for some sort of confirmation. This */
    /* may be a call clear or a call connected packet.                      */
    /************************************************************************/

    /************************************************************************/
    /* Wait for the counter value to change indicating an incoming message. */
    /* There is only one counter in the array so there is no point in       */
    /* testing to see if it is the correct counter.                         */
    /************************************************************************/
    ctr_array[0].flags = X25FLG_CTR_ID;
    ctr_array[0].flags |= X25FLG_CTR_VALUE;
    ctr_array[0].ctr_id = ctr_id;
    ctr_array[0].ctr_value = 0;

    (void)x25_ctr_wait(ctr_num,ctr_array);

    (void)x25_receive(&conn_id,&cb_msg);  /* Receive the incoming message.  */

    /************************************************************************/
    /* If the incoming message shows that the call has been connected, data */
    /* may be sent.                                                         */
    /************************************************************************/
    if (cb_msg.msg_type == X25_CALL_CONNECTED)
    {
      cb_data.flags = X25FLG_DATA;
      cb_data.data_len = strlen(INFO);
      cb_data.data = INFO;
      (void)x25_send(conn_id,&cb_data);

      (void)printf("%s: Data sent\n",argv[0]);

      /**********************************************************************/
      /* Send some more data but this time with the D bit set.              */
      /* The other DTE must send an acknowledgement to this data, which we  */
      /* wait for.                                                          */
      /**********************************************************************/
      cb_data.flags = X25FLG_DATA;
      cb_data.flags |= X25FLG_D_BIT;
      cb_data.data_len = strlen(INFO2);
      cb_data.data = INFO2;
      (void)x25_send(conn_id,&cb_data);
      (void)printf("%s: Data sent\n",argv[0]);

      (void)x25_ctr_wait(ctr_num,ctr_array);   /* Wait for acknowledgement. */
      (void)x25_receive(&conn_id,&cb_msg);     /* Get acknowledgement.      */

      if (cb_msg.msg_type == X25_DATA_ACK)
	(void)printf("%s: Data has been acknowledged.\n",argv[0]);
      else
	(void)printf("%s: Unexpected packet received.\n",argv[0]);

      /**********************************************************************/
      /* Clear the call now that transmission is completed.                 */
      /**********************************************************************/
      cb_clear.flags = X25FLG_CAUSE;
      cb_clear.flags |= X25FLG_DIAGNOSTIC;
      cb_clear.cause = 0;            /* The CCITT code for DTE-originated   */
      cb_clear.diagnostic = 0;       /* No further information              */

      (void)printf("%s: Clearing the call.",argv[0]);

      /**********************************************************************/
      /* The x25_call_clear function can return information from the clear  */
      /* confirmation packet. However, this isn't required here, so set the */
      /* third parameter to NULL.                                           */
      /**********************************************************************/
      (void)x25_call_clear(conn_id,&cb_clear,(struct cb_msg_struct *)NULL);
    }
    /************************************************************************/
    /* If the message received was a clear indication message then terminate*/
    /* program.                                                             */
    /************************************************************************/
    else if (cb_msg.msg_type == X25_CLEAR_INDICATION)
    {
      (void)printf("%s: Call cleared. Cause = 0x%02x Diagnostic = 0x%02x\n",
	argv[0], cb_msg.msg_point.cb_clear->cause,
	cb_msg.msg_point.cb_clear->diagnostic);
    }

    /************************************************************************/
    /* Finally, tidy up by removing allocated resources                     */
    /************************************************************************/
    (void)x25_ctr_remove(ctr_id);
    (void)x25_term(&cb_link_name);
  }
  return(0);
}






