/**********************************************************************************************************************/
/**
 * @file            trdp_ladder.h
 *
 * @brief           Global Variables for TRDP Ladder Topology Support
 *
 * @details
 *
 * @note            Project: TCNOpen TRDP prototype stack
 *
 * @author          Kazumasa Aiba, TOSHIBA
 *
 * @remarks All rights reserved. Reproduction, modification, use or disclosure
 *          to third parties without express authority is forbidden,
 *          Copyright TOSHIBA, Japan, 2012.
 *
 *
 *
 */

#ifndef TRDP_LADDER_H_
#define TRDP_LADDER_H_

#ifdef TRDP_OPTION_LADDER
/*******************************************************************************
 * INCLUDES
 */
#include "trdp_types.h"
#include "vos_shared_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************************************
 * DEFINES
 */
#define TRAFFIC_STORE_SIZE 65536			/* Traffic Store Size : 64KB */
#define SUBNET1	0x00000000					/* Sub-network Id1 */
#define SUBNET2	0x00002000					/* Sub-network Id2 */

/***********************************************************************************************************************
 * GLOBAL VARIABLES
 */

/* Traffic Store */
extern CHAR8 TRAFFIC_STORE[];				/* Traffic Store shared memory name */
extern mode_t PERMISSION	;					/* Traffic Store permission is rw-rw-rw- */
extern UINT8 *pTrafficStoreAddr;			/* pointer to pointer to Traffic Store Address */
extern VOS_SHRD_T  pTrafficStoreHandle;	/* Pointer to Traffic Store Handle */
extern UINT16 TRAFFIC_STORE_MUTEX_VALUE_AREA;		/* Traffic Store mutex ID Area */

/* PDComLadderThread */
extern CHAR8 pdComLadderThreadName[];		/* Thread name is PDComLadder Thread. */
extern BOOL pdComLadderThreadActiveFlag;	/* PDComLaader Thread active/noactive Flag :active=TRUE, nonActive=FALSE */
extern BOOL pdComLadderThreadStartFlag;	/* PDComLadder Thread instruction start up Flag :start=TRUE, stop=FALSE */

/* Sub-net */
extern UINT32 usingSubnetId;				/* Using SubnetId */


/***********************************************************************************************************************
 * PROTOTYPES
 */
/******************************************************************************/
/** Initialize TRDP Ladder Support
 *  Create Traffic Store mutex, Traffic Store, PDComLadderThread.
 *
 *	Note:
 *
 *	@retval			TRDP_NO_ERR
 *	@retval			TRDP_MUTEX_ERR
 */
TRDP_ERR_T trdp_ladder_init (
	void);

/******************************************************************************/
/** Finalize TRDP Ladder Support
 *  Delete Traffic Store mutex, Traffic Store.
 *
 *	Note:
 *
 *	@retval			TRDP_NO_ERR
 *	@retval			TRDP_MEM_ERR
 */
TRDP_ERR_T trdp_ladder_terminate (
	void);

/**********************************************************************************************************************/
/** Set pdComLadderThreadStartFlag.
 *
 *  @param[in]      startFlag         PdComLadderThread Start Flag
 *
 *  @retval         TRDP_NO_ERR			no error
 */
TRDP_ERR_T  trdp_setPdComLadderThreadStartFlag (
    BOOL startFlag);

/**********************************************************************************************************************/
/** Set SubNetwork Context.
 *
 *  @param[in]      SubnetId			Sub-network Id: SUBNET1 or SUBNET2
 *
 *  @retval         TRDP_NO_ERR			no error
 *  @retval         TRDP_PARAM_ERR		parameter error
 *  @retval         TRDP_NOPUB_ERR		not published
 *  @retval         TRDP_NOINIT_ERR	handle invalid
 */

TRDP_ERR_T  tlp_setNetworkContext (
    UINT32          subnetId);

/**********************************************************************************************************************/
/** Get SubNetwork Context.
 *
 *  @param[in,out]  pSubnetId			pointer to Sub-network Id
 *
 *  @retval         TRDP_NO_ERR			no error
 *  @retval         TRDP_PARAM_ERR		parameter error
 *  @retval         TRDP_NOPUB_ERR		not published
 *  @retval         TRDP_NOINIT_ERR	handle invalid
 */

TRDP_ERR_T  tlp_getNetworkContext (
    UINT32          *pSubnetId);

/**********************************************************************************************************************/
/** Get Traffic Store accessibility.
 *
 *  @retval         TRDP_NO_ERR			no error
 *  @retval         TRDP_PARAM_ERR		parameter error
 *  @retval         TRDP_NOPUB_ERR		not published
 *  @retval         TRDP_NOINIT_ERR	handle invalid
 */

TRDP_ERR_T  tlp_lockTrafficStore (
    void);

/**********************************************************************************************************************/
/** Release Traffic Store accessibility.
 *
 *  @retval         TRDP_NO_ERR			no error
 *  @retval         TRDP_PARAM_ERR		parameter error
 *  @retval         TRDP_NOPUB_ERR		not published
 *  @retval         TRDP_NOINIT_ERR	handle invalid
 */

TRDP_ERR_T  tlp_unlockTrafficStore (
    void);

#endif	/* TRDP_OPTION_LADDER */

#ifdef __cplusplus
}
#endif
#endif /* TRDP_LADDER_H_ */