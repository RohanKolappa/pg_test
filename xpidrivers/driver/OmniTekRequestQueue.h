#pragma once
/*******************************************************************************
 *
 * Copyright (c) 2007 Image Processing Techniques, Ltd. www.imageproc.com.
 * 
 * Image Processing Techniques Ltd. licenses this software under specific terms and
 * conditions.  Use of any of the software or derviatives thereof in any
 * product without a Image Processing Techniques is strictly prohibited. 
 * 
 * This file is provided without any express or implied warranties, including, 
 * but not limited to, the implied warranties of merchantability and fitness 
 * for a particular purpose. It is not intended for use in life support 
 * appliances, devices, or systems. Use in such applications is expressly 
 * prohibited.
 *
 * Image Processing Techniques makes no guarantee or representations regarding the use of, 
 * or the results of the use of, the software and documentation in terms of correctness, 
 * accuracy, reliability, currentness, or otherwise; and you rely on the software,
 * documentation and results solely at your own risk.
 * 
 * IN NO EVENT SHALL IMAGE PROCESSING TECHNIQUES LTD. BE LIABLE FOR ANY LOSS OF USE, 
 * LOSS OF BUSINESS, LOSS OF PROFITS, INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL
 * DAMAGES OF ANY KIND.
 * 
 *
 *****************************************************************************/

//Request queue and helper functions
  

/*!
 \brief       Initialize a request queue structure
 \param [in]  pQueue pointer to queue struct to initialize
 \param [in]  Name for queue (will default if left NULL)
*/
void OmniTekRequestQueueInit(
  struct _OmniTekRequestQueue * pQueue,
  char *                        Name
);

/*!
 \brief       Add Request to a queue
 \param [in]  pQueue Request Queue to add to
 \param [in]  pRequest Request to add to queue
 \return      0 on success or error code
*/
int OmniTekRequestQueueAddRequest(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      * pRequest
);

/*!
 \brief       Remove Request to a queue
 \param [in]  pQueue Request Queue to remove from
 \param [in]  pRequest Request to remove from queue
 \return      0 on success or error code
*/
int OmniTekRequestQueueRemoveRequest(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      * pRequest
);

/*!
 \brief       Move Request between queues
 \param [in]  pCurrentQueue Request Queue to remove from
 \param [in]  pNewQueue Request Queue to add to
 \param [in]  pRequest Request to move
 \return      0 on success or error code
*/
int OmniTekRequestQueueMoveRequest(
  struct _OmniTekRequestQueue * pCurrentQueue,
  struct _OmniTekRequestQueue * pNewQueue,
  struct _OmniTekRequestQueueObject      * pRequest
);

/*!
 \brief       Does this queue contain a specified request
 \param [in]  pQueue Request Queue to look in
 \param [in]  pRequest to look for
 \return      0 if request is in queue or error code
*/
int OmniTekRequestQueueContains(  
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      * pRequest
);

/*!
 \brief       Is this request queue empty
 \param [in]  pQueue Request Queue to check
 \return      0 if not empty, positive integer otherwise
*/
int OmniTekRequestQueueIsEmpty(
  struct _OmniTekRequestQueue * pQueue
);

/*!
 \brief       How many entries in this queue
 \param [in]  pQueue Request Queue to check
 \return      number of entries in queue
*/
int OmniTekRequestQueueSize(
  struct _OmniTekRequestQueue * pQueue
);

/*!
 \brief       Get the next request from this queue
 \param [in]  pQueue Request Queue to retrieve request from
 \param [in]  ppRequest pointer to request struct pointer that will be set to the address of the next request
 \return      0 if succesful or error code if queue is empty
*/
int OmniTekRequestQueueNext(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      ** ppRequest
);

/*!
 \brief       Get the last request in this queue (for joining DMA SGLs)
 \param [in]  pQueue Request Queue to retrieve request from
 \param [in]  ppRequest pointer to request struct pointer that will be set to the address of the next request
 \return      0 if succesful or error code if queue is empty
*/
int OmniTekRequestQueueLast(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      ** ppRequest
);



/*!
 \brief       Initialize a new request
 \param [in]  pRequest pointer to request struct to initialize
 \return      0 on success
*/
int OmniTekRequestQueueInitRequest(
  struct _OmniTekRequestQueueObject * pRequest
);
