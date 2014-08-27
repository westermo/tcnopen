/*********************************************************************************************************************/
/**
 * @file            tau_ctrl.c
 *
 * @brief           Functions for train switch control
 *
 * @details
 *
 * @note            Project: TCNOpen TRDP prototype stack
 *
 * @author          Armin-H. Weiss (initial version)
 *
 * @remarks This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. 
 *          If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *          Copyright Bombardier Transportation Inc. or its subsidiaries and others, 2013. All rights reserved.
 *
 * $Id$
 *
 *
 */

/**********************************************************************************************************************
 * INCLUDES
 */

#include <string.h>
#include <stdio.h>

#include "trdp_types.h"
#include "trdp_utils.h"
#include "trdp_if_light.h"
#include "trdp_proto.h"
#include "tau_ctrl.h"

/**********************************************************************************************************************
 * DEFINES
 */


/**********************************************************************************************************************
 * TYPEDEFS
 */


/**********************************************************************************************************************
 *   Locals
 */

 static TRDP_PUB_T      priv_pubHandle          = 0;    /*    Our identifier to the publication                     */
 static TRDP_SUB_T      priv_subHandle          = 0;    /*    Our identifier to the subscription                    */
 static TRDP_IP_ADDR_T  priv_ecspIpAddr         = 0;    /*    ECSP IP address                                       */
 static TRDP_IP_ADDR_T  priv_ecscIpAddr         = 0;    /*    ECSC IP address                                       */
 static BOOL8           priv_ecspCtrlInitialised= FALSE;
  

/**********************************************************************************************************************/
/*    Train switch control                                                                                            */
/**********************************************************************************************************************/

/**********************************************************************************************************************/
/**    Function to init  ECSP control interface
 *
 *  @param[in]      appHandle           Application handle
 *  @param[in]      ecspIpAddr          ECSP address
 *  @param[in]      ecscIpAddr          ECSC address
 *
 *  @retval         TRDP_NO_ERR     no error
 *  @retval         TRDP_INIT_ERR   initialisation error
 *
 */
EXT_DECL TRDP_ERR_T tau_initEcspCtrl ( TRDP_APP_SESSION_T   appHandle, 
                                       TRDP_IP_ADDR_T       ecspIpAddr, 
                                       TRDP_IP_ADDR_T       ecscIpAddr)
{
	/* session already opened, handle publish/subscribe */
    TRDP_ERR_T             err;
    
    priv_ecspIpAddr = ecspIpAddr;
    priv_ecscIpAddr = ecscIpAddr;

	/*    Copy the packet into the internal send queue, prepare for sending.    */
    /*    If we change the data, just re-publish it    */
    err = tlp_publish(  appHandle,                  /*    our application identifier        */
                        &priv_pubHandle,            /*    our pulication identifier         */
                        TRDP_ECSP_CTRL_COMID,       /*    ComID to send                     */
                        0,                          /*    ecnTopoCounter                    */
                        0,                          /*    opTopoCounter                     */
                        appHandle->realIP,          /*    default source IP                 */
                        ecspIpAddr,                 /*    where to send to                  */
                        ECSP_CTRL_CYCLE,            /*    Cycle time in us                  */
                        0,                          /*    not redundant                     */
                        TRDP_FLAGS_MARSHALL,        /*    packet flags - UDP, no call back  */
                        NULL,                       /*    default qos and ttl               */
                        (UINT8 *)NULL,              /*    initial data                      */
                        0                           /*    data size                         */
                        );
    if ( err != TRDP_NO_ERR )
    {
        vos_printLog(VOS_LOG_ERROR, "tlp_publish() failed !\n");
        return err;
    }

    err = tlp_subscribe( appHandle,                 /*    our application identifier            */
                         &priv_subHandle,           /*    our subscription identifier           */
                         NULL,                      /*    user ref                              */
                         NULL,                      /*    callback function                     */
                         TRDP_ECSP_STAT_COMID,      /*    ComID                                 */
                         0,                         /*    ecnTopoCounter                        */
                         0,                         /*    opTopoCounter                         */
                         0,                         /*    Source IP filter                      */
                         ecscIpAddr,                /*    Default destination    (or MC Group)  */
                         TRDP_FLAGS_MARSHALL,       /*    packet flags - UDP, no call back      */
                         ECSP_STAT_TIMEOUT,         /*    Time out in us                        */
                         TRDP_TO_SET_TO_ZERO);      /*    delete invalid data on timeout        */

    if ( err != TRDP_NO_ERR )
    {
        vos_printLog(VOS_LOG_ERROR, "tlp_subscribe() failed !\n");
        return err;
    }

    priv_ecspCtrlInitialised = TRUE;

    return err;
}


/**********************************************************************************************************************/
/**    Function to close  ECSP control interface
 *
 *  @param[in]      appHandle           Application handle
 *
 *  @retval         TRDP_NO_ERR         no error
 *  @retval         TRDP_NOINIT_ERR     module not initialised
 *  @retval         TRDP_UNKNOWN_ERR    undefined error
 *
 */

EXT_DECL TRDP_ERR_T tau_terminateEcspCtrl (TRDP_APP_SESSION_T   appHandle)
{
    if (priv_ecspCtrlInitialised == TRUE)
    {
        /* clean up */
        TRDP_ERR_T             err;
	
        priv_ecspCtrlInitialised = FALSE;

        err = tlp_unpublish(appHandle, priv_pubHandle);
        if ( err != TRDP_NO_ERR )
        {
            vos_printLog(VOS_LOG_ERROR, "tlp_unpublish() failed!\n");
            return err;
        }

        err = tlp_unsubscribe(appHandle, priv_subHandle);
        if ( err != TRDP_NO_ERR )
        {
            vos_printLog(VOS_LOG_ERROR, "tlp_unsubscribe() failed !\n");
            return err;
        }

	    return err;
    }

    return TRDP_NOINIT_ERR;
}


/**********************************************************************************************************************/
/**    Function to set ECSP control information
 *
 *
 *  @param[in]      appHandle       Application handle
 *  @param[in]      pEcspCtrl       Pointer to the ECSP control structure
 *
 *  @retval         TRDP_NO_ERR     no error
 *  @retval         TRDP_NOINIT_ERR module not initialised
 *  @retval         TRDP_PARAM_ERR  Parameter error
 *
 */
EXT_DECL TRDP_ERR_T tau_setEcspCtrl ( TRDP_APP_SESSION_T    appHandle,
                                      TRDP_ECSP_CTRL_T      *pEcspCtrl)
{
    if (priv_ecspCtrlInitialised == TRUE)
    {
        return tlp_put(appHandle, priv_pubHandle, (UINT8 *) pEcspCtrl, sizeof(TRDP_ECSP_CTRL_T));
    }

    return TRDP_NOINIT_ERR;
}

/**********************************************************************************************************************/
/**    Function to get ECSP status information
 *
 *  @param[in]      appHandle       Application handle
 *  @param[in/out]  pEcspStat       Pointer to the ECSP status structure
 *  @param[in/out]  pPdInfo         Pointer to PD status information
 *
 *  @retval         TRDP_NO_ERR     no error
 *  @retval         TRDP_NOINIT_ERR module not initialised
 *  @retval         TRDP_PARAM_ERR  Parameter error
 *
 */
EXT_DECL TRDP_ERR_T tau_getEcspStat ( TRDP_APP_SESSION_T   appHandle,
                                      TRDP_ECSP_STAT_T    *pEcspStat,
                                      TRDP_PD_INFO_T      *pPdInfo)
{
    if (priv_ecspCtrlInitialised == TRUE)
    {
        UINT32      receivedSize =      sizeof(TRDP_ECSP_STAT_T);
        return tlp_get(appHandle,
                  priv_subHandle,
                  pPdInfo,
                  (UINT8 *) pEcspStat,
                  &receivedSize);
    }
    
    return TRDP_NOINIT_ERR;
}


/**********************************************************************************************************************/
/**    Function for ECSP confirmation/correction request, reply will be received via call back
 *
 *  @param[in]      TRDP_APP_SESSION_T  appHandle
 *  @param[in]      pUserRef            user reference returned with reply
 *  @param[in]      EcspConfRequest     Pointer to confirmation data
 *
 *  @retval         TRDP_NO_ERR     no error
 *  @retval         TRDP_NOINIT_ERR module not initialised
 *  @retval         TRDP_PARAM_ERR  Parameter error
 *
 */
EXT_DECL TRDP_ERR_T tau_requestEcspConfirm ( TRDP_APP_SESSION_T         appHandle,
                                             const void                *pUserRef,
                                             TRDP_ECSP_CONF_REQUEST_T  *pEcspConfRequest)
{
    if (priv_ecspCtrlInitialised == TRUE)
    {
        TRDP_UUID_T sessionId;                 /*    Our session ID for sending MD                             */
       
        return tlm_request( appHandle,                      /* appHandle */
                            pUserRef,                       /* pUserRef */
                            NULL,                           /* callback function */
                            &sessionId,                     /* pSessionId */
                            TRDP_ECSP_CONF_REQUEST_COMID,   /* comId */
                            0,                              /* etbTopoCnt */
                            0,                              /* opTrnTopoCnt */
                            appHandle->realIP,              /* srcIpAddr */
                            priv_ecspIpAddr,                /* destIpAddr */
                            TRDP_FLAGS_NONE,                /* pktFlags */
                            1,                              /* numReplies */
                            ECSP_CONF_REPLY_TIMEOUT,        /* replyTimeout */
                            NULL,                           /* pSendParam */
                            (const UINT8 *) &pEcspConfRequest,
                            sizeof(TRDP_ECSP_CONF_REQUEST_T),
                            NULL,                           /* srcUri */
                            NULL);                          /* destUri */
    }

    return TRDP_NOINIT_ERR;
}