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

/*!
 \enum _RequestStatus
 \brief This describes the current status of a request
*/
typedef enum _RequestStatus
{
  OMNITEK_REQUEST_UNINITIALIZED = 0,    /*!< Request is uninitialized */
  OMNITEK_REQUEST_INITIALIZED,          /*!< Request has been initialized - user space pointers are set */
  OMNITEK_REQUEST_USER_KERNEL_COPIED,   /*!< Request has been received by kernel and user data copied to kernel space */
  OMNITEK_REQUEST_PENDING,              /*!< Request is pending (e.g. DMA transfer) */
  OMNITEK_REQUEST_PROCESSED,            /*!< Request has been processed (e.g. DMA complete or function call complete) */
  OMNITEK_REQUEST_KERNEL_USER_COPIED,   /*!< Data has been copied from kernel output buffer to user output buffer */
  OMNITEK_REQUEST_COMPLETE,             /*!< Request has been completed (once returned to user it is no longer valid) */
  OMNITEK_REQUEST_CANCELLED = -1        /*!< Request has been cancelled (once returned to user it is no longer valid) */
} RequestStatus;

struct _OmnitekKernelRequest;
struct _OmnitekUserRequest;


/*!
 \struct OmniTekKernelRequest
*/
typedef struct _OmniTekKernelRequest
{
  u32                 * pInBuffer;      /*!< Pointer to input buffer (user->kernel) in user space */
  u32                   inBufferSize;   /*!< Size of input buffer */
  u32                 * pOutBuffer;     /*!< Pointer to output buffer (kernel->user) in user space */
  u32                   outBufferSize;  /*!< Size of output buffer */
  struct _OmniTekUserRequest * userRequest;    /*!< The user request that this kernel request originated from */
  struct _OMNITEK_INTERFACE_EXTENSION *
                        pExt;           /*!< Pointer to the interface extension that this request is targetting */
} OmniTekKernelRequest, *POmniTekKernelRequest;


/*!
 \struct OmniTekUserRequest
*/
typedef struct _OmniTekUserRequest
{
  
  u32 * pInBuffer;      /*!< Pointer to input buffer (user->kernel) in user space */
  u32   inBufferSize;   /*!< Size of input buffer */
  u32 * pOutBuffer;     /*!< Pointer to output buffer (kernel->user) in user space */
  u32   outBufferSize;  /*!< Size of output buffer */
  pid_t pid;            /*!< Process ID that request originated from - will be signalled on completion */
  struct _OmniTekKernelRequest * kernelRequest; /*!< Associated kernel request */
} OmniTekUserRequest, *POmniTekUserRequest;
