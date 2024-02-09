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
#include <linux/workqueue.h>
#include <linux/kobject.h>

#include "../include/OmniTekIoctl_linux.h"
#include "OmniTekRequestQueue.h"



/*! 
\brief Called when a DMA Interrupt completes
*/
typedef void (OmnitekDmaInterruptComplete)(void *, u32);


/*!
  \brief      Register a resource
  \details    Called to register a resource during hardware scanning
  \param pExt  Pointer to extension to add resource to
  \param Type Type of resource to add 
  \param Bar Resource BAR number
  \param RegOffset Offset of first register for resource
  \return    pointer to the resource struct
*/
PResource 
AddResource(
		struct _OMNITEK_INTERFACE_EXTENSION	*pExt,    
		ResourceType				                Type,  
		u8								                  Bar,   
		u32								                RegOffset 
		);

/*!
  \brief      Remove a resource
  \details    Called to remove a resource during driver shutdown
  \param pResource Pointer to resource to be removed
  \return  0 on success or error code
*/
int
RemoveResource(
	PResource pResource                             
	);

/*!
  \brief      Initialise DMA Resource
  \param pResource DMA) resource to initialise
  \details    Called to initialise the DMA resource and set up individual channels
 */
void
DmaResourceInit(
  PResource pResource                          
	);

/*!
  \brief      Register watchdog for resource
  \details    
  \param pExt Pointer to extension to register watchdog for
  \param Id ID Of session to watchdog
  \param pRequest Details for command
  \return  0 on success or error code
*/
int
ResourceRegisterWatchdog(
	struct _OMNITEK_INTERFACE_EXTENSION	*pExt,
	u32 Id,
	POmniTekKernelRequest pRequest
	);

/*!
  \brief      Send control command to the resource
  \details    
  \param pExt Pointer to extension with resource to control
  \param pRequest Details for command
 \return  0 on success or error code
*/
int
ResourceControl(                                
  struct _OMNITEK_INTERFACE_EXTENSION	*pExt,    
	struct _OmnitekKernelRequest * pRequest                     
	);

/*!
  \brief      Check a resource
  \details    Confirms that this resource is available on the specified extension
  \param pExt Pointer to extension with resource to check
  \param pResource Resource to check
  \return true if resource belongs to the supplied extension
  */
bool
ResourceCheck(
  struct _OMNITEK_INTERFACE_EXTENSION	*pExt,   
	PResource pResource                          
	);


/*!
  \brief      Release a reference to a resource
  \details    The specified resource reference is released and the reference count is decremented
  \param pExt             Pointer to extension to lock frame for
  \param SessionId    ID for session 
  \param pResource    Resource to release
  \return  0 on success or error code
*/
int
ReleaseResource(
  struct _OMNITEK_INTERFACE_EXTENSION	*pExt,    
	u32									              SessionId,  
	PResource								            pResource 
	);



/*!
 \brief       Find a DMA Channel by number
 \param [in]  pExt Device extension to find channel in
 \param [in]  Identifier Number of channel to find
 \return      Pointer to DMA Channel resource
*/
PResource
DmaChannelFind(
	struct _OMNITEK_INTERFACE_EXTENSION * pExt,
	u32 Identifier
	);

/*!
  \brief      Set the event callback for a DMA channel
*/
int
DmaChannelSetEventFunc
(
  struct _Resource * pResource,
  void (*event_func)(struct _Resource * pResource, void * arg),
  void * arg
  );
