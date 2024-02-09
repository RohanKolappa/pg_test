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
#include "OmniTek_linux.h"
#include "OmniTekRequestQueue.h"

void OmniTekRequestQueueInit(
  struct _OmniTekRequestQueue * pQueue,
  char *                        Name
)
{
  INIT_LIST_HEAD(&(pQueue->Entries));
  if (Name)
    pQueue->Name = Name;
  else
    pQueue->Name = "Request Queue";
}


int OmniTekRequestQueueAddRequest(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      * pRequest
)
{
  //Cannot add the request to a queue if the request is already in a different queue:
  if (unlikely(pRequest->pCurrentQueue == pQueue))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Request %p is already in specified queue %s!\n",pRequest,pQueue->Name);
    return -EINVAL;
  }
  
  if (unlikely(pRequest->pCurrentQueue != 0))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Request %p is already in another queue %s, can't add to queue %s!\n",pRequest,
                                                                                                           pRequest->pCurrentQueue->Name,
                                                                                                           pQueue->Name);
    return -EINVAL;
  }
  
  list_add_tail(&(pRequest->Object),&(pQueue->Entries));
  pRequest->pCurrentQueue = pQueue;
 
  OmniTekDebug(REQUEST_QUEUE,KERN_INFO, "Request %p added to queue %s\n",pRequest,pQueue->Name);

  return 0;
  
}

int OmniTekRequestQueueRemoveRequest(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      * pRequest
)
{
  if (unlikely(pRequest->pCurrentQueue == 0))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Request %p is not in a queue, can't remove!\n",pRequest);
    return -EINVAL;
  }
  if (unlikely(pRequest->pCurrentQueue != pQueue))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Request %p is not in specified queue %s, can't remove!\n",pRequest,pQueue->Name);
    return -EINVAL;
  }

  list_del(&(pRequest->Object));
  pRequest->pCurrentQueue = 0;

  OmniTekDebug(REQUEST_QUEUE,KERN_INFO, "Request %p removed from queue %s\n",pRequest,pQueue->Name);

  return 0;


}

int OmniTekRequestQueueMoveRequest(
  struct _OmniTekRequestQueue * pCurrentQueue,
  struct _OmniTekRequestQueue * pNewQueue,
  struct _OmniTekRequestQueueObject      * pRequest
)
{
  //TODO: Should this be more atomic?

  int status = 0;
  status = OmniTekRequestQueueRemoveRequest(pCurrentQueue,pRequest);
  if (unlikely(status)) return status;

  status = OmniTekRequestQueueAddRequest(pNewQueue,pRequest);
  if (unlikely(status)) return status;
  

  OmniTekDebug(REQUEST_QUEUE,KERN_INFO, "Request %p moved from queue %s to queue %s\n",pRequest,pCurrentQueue->Name,pNewQueue->Name);

  return 0;


}

int OmniTekRequestQueueContains(  
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      * pRequest
)
{

  struct _OmniTekRequestQueueObject  * pCurRequest;

  //We don't error out here as checking the specified queue might uncover some other bug...
  if (unlikely(pRequest->pCurrentQueue != pQueue))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Request %p doesn't think it's in this queue %s!\n",pRequest, pQueue->Name);
  }

  if (unlikely(pRequest->pCurrentQueue == 0))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Request %p doesn't think it's in any queue!\n",pRequest);
  }

  list_for_each_entry(pCurRequest,&(pQueue->Entries),Object)
  {
    if (pCurRequest == pRequest)
    {
      OmniTekDebug(REQUEST_QUEUE,KERN_INFO, "Request %p is in queue %s\n",pRequest,pQueue->Name);
      return 0;
    }
  }
      
  OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Request %p is not in queue %s\n",pRequest,pQueue->Name);
  return -EINVAL;
  

}

int OmniTekRequestQueueIsEmpty(
  struct _OmniTekRequestQueue * pQueue
)
{
  return list_empty(&(pQueue->Entries));
}

int OmniTekRequestQueueSize(
  struct _OmniTekRequestQueue * pQueue
)
{
  struct list_head * pList;
  int count = 0;
  list_for_each(pList,&(pQueue->Entries))
  {
    count++;
  }
  OmniTekDebug(REQUEST_QUEUE,KERN_INFO, "Queue %s has %d entries\n",pQueue->Name,count);
  return count;
}

int OmniTekRequestQueueNext(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      ** ppRequest
)
{
  struct _OmniTekRequestQueueObject      * pRequest;

  if (list_empty(&(pQueue->Entries)))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Queue %s is empty\n",pQueue->Name);    
    dump_stack();
    return -EINVAL;
  }

  pRequest = list_entry(pQueue->Entries.next,struct _OmniTekRequestQueueObject,Object);
  *ppRequest = pRequest;
  return 0;
  
}

int OmniTekRequestQueueLast(
  struct _OmniTekRequestQueue * pQueue,
  struct _OmniTekRequestQueueObject      ** ppRequest
)
{
  struct _OmniTekRequestQueueObject      * pRequest;

  if (list_empty(&(pQueue->Entries)))
  {
    OmniTekDebug(REQUEST_QUEUE,KERN_ERR, "Queue %s is empty\n",pQueue->Name);    
    return -EINVAL;
  }

  pRequest = list_entry(pQueue->Entries.prev,struct _OmniTekRequestQueueObject,Object);
  *ppRequest = pRequest;
  return 0;
  
}



int OmniTekRequestQueueInitRequest(
  struct _OmniTekRequestQueueObject * pRequest
)
{
  INIT_LIST_HEAD(&(pRequest->Object));
  pRequest->pCurrentQueue = 0;
  return 0;
}
