#pragma once

#include "ptp_main.h"


//extern int efdma_channel_acquire(struct _OMNITEK_INTERFACE_EXTENSION * pExt, int fdma_num,struct efdma_channel ** ppEFDMA_Channel);

//extern void efdma_channel_release(struct efdma_channel * pEFDMA_Channel);

////TODO: Vector IO version of this to support fragmented ethernet packets etc?
//extern ssize_t efdma_channel_transmit_packet(struct efdma_channel * pEFDMA_Channel, char * pBuffer, size_t length, unsigned char key, size_t min_length);

////TODO: Should we also have a receive_packet_user for direct copy to a user buffer?
////TODO: The receive function should also specify the packet key.  If required we could support multiple callbacks dependent on
////      packet key.
//extern ssize_t efdma_channel_receive_packet(struct efdma_channel * pEFDMA_Channel, char * pBuffer, size_t length);

//extern int efdma_channel_set_rx_callback(struct efdma_channel * pEFDMA_Channel, efdma_receive_func * rx_func, void * priv);

//extern int efdma_channel_set_throttle_drop(struct efdma_channel * pEFDMA_Channel, bool enable);

/*!
 \page Driver_EFDMA_API NetViz Driver EFDMA API Documentation
 \section Introduction
 This section details the operation and use of the EFDMA Kernel module

 \section Overview
 EFDMA is implemented as a separate layer above the OmniTek DMA driver.  When an EFDMA channel is created the module will acquire DMA resources from the
 OmniTek DMA module and set up ringbuffers.  The EFDMA module presents a packet-based interface for transferring data.
 
 \subsection Ring Buffers
 The EFDMA Channel makes use of two ringbuffers, one for send and one for received.  The ringbuffers are divided into individual 'slices', each of which is a 
 DMA transaction.  The ringbuffer data structure has two pointer structures - a DMA pointer and a user pointer.  The former is used to track the progress of the 
 DMA controller through the ringbuffer, and the later to track the user transfers to/from the buffers.

 The ringbuffer slices are automatically re-queued once they have been completed.  In the case of the transmit this is whenever the scatter-gather interrupt is
 received for a slice as that slices data has now been transferred to the hardware.  For the receive side slices can be requeued when the data from the slice
 has been taken.

 The ringbuffers can be viewed as a separate layer of the EFDMA with a number of functions and callbacks to handle transmission and reception.

 \subsection Packet Layer
 The EFDMA module automatically handles packetization of data.  Hardware headers and padding are automatically added to packets for transmission, and are
 stripped from receive packets.

 \subsubsection Packet Transmission
 When the EFDMA module is requested to transmit a packet it first writes the required header to the ringbuffer - this is the number of bytes payload + header
 size.  It then copies the payload data from the provided buffer to the ringbuffer, and finally up to 7 bytes padding to align the transfer to a 64-bit 
 boundary.  Finally it writes the total (header+payload+padding) size to the hardware register to initiate the transfer. 

 \subsubsection Packet Reception
 Packet reception is indicated by an event interrupt.  This is processed by the OmniTek DMA Driver module which executes a callback function - in this case the
 EFDMA event handler - efdma_channel_event_func.  This function reads the number of bytes available and increments a total, then schedules the ringbuffer 
 dma_work (efdma_channel_dma_work) function to execute.  Multiple small packets are coalesced into one single process through this method.  The
 efdma_channel_dma_work function then indicates to the ringbuffer the amount of data that has been received causing the dma_pointer to update
 (ringbuffer_xferred), in turn calling the wake function callback (efdma_channel_wake_func).

 The wake function then processes the available data into packets.  Given the total data available it retrieves the next packet size and calls the rx_func 
 callback with the size available.  This rx_func should then retrieve the specified size into a buffer and process it.  The wake function will then strip any 
 padding bytes from the ringbuffer.  This process is repeated until the available data has been processed.

 \section Using the EFDMA Module
 The only requirement for the EFDMA module is that an rx callback function be provided.  If not set a dummy function in the EFDMA module will simply dump the
 received data.

 Initializing the EFDMA channel is as follows (for example code see the ethernet driver or test_efdma module):
 call efdma_channel_acquire to create an efdma_channel struct.
 call efdma_channel_set_rx_callback to set the packet receive function.

 On shutdown call the efdma_channel_release function to close the channel and free resources.

 Packets are transmitted using the efdma_channel_transmit_packet function, this copies data from the provided buffer to the ringbuffer and initiates the
 hardware transmit.

 Packet reception is indicated by a call to the packet receive function.  This should then call efdma_channel_packet_receive to copy data from the buffer.
 This mechanism allows direct copying of data to a buffer allocated by the packet_receive function e.g. a struct skb_buff network socket buffer.

 \section Throttle Behaviour
 If the throttle option is set then the EFDMA ringbuffer will drop packets whenever the buffer starts to become too full.  This is achieved by pausing the
 underlying FDMA Channel when there are two or less further slices remaining.  The packet portion of the firmware should cleanly drop the received packets
 allowing us to restart once the slices are finished.

 This situation arises when the buffer fills faster than the received packets are processed, resulting in several slices of un-processed packets building
 up.  Once these slices have been processed and re-queued the buffer will automatically restart.
 
 \section Notes
 Currently the ringbuffers allocate (each) 5 single 4k pages for their buffers.  This will be updated to allow the ringbuffer size to be specified if
 required.

*/
