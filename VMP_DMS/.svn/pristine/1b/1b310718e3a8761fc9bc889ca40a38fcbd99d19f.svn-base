// ============================================================================
// 
// Copyright (c) 2003-2014 Barco N.V.
// 
// ============================================================================
// 
// This file is part of "BFC::IO".
// 
// "BFC::IO" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "BFC::IO" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with "BFC::IO"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
// 
// ============================================================================
// 
// Filename:
//	BFC.IO.SerialPort.h
// 
// Description:
//	...
// 
// Author(s):
//	Kester AERNOUDT
// 
// ============================================================================

#ifndef _BFC_IO_SerialPort_H_
#define _BFC_IO_SerialPort_H_

// ============================================================================

#include "BFC.IO.DLL.h"

// ============================================================================

#include "BFC.Base.SPtr.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

BFC_PTR_DECL( BFC_IO_DLL, SerialPort )

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#include "BFC.IO.File.h"

// ============================================================================

namespace BFC {
namespace IO {

// ============================================================================

/// A serial port.

///
/// \ingroup BFC_IO

class BFC_IO_DLL SerialPort : public File {

	public:

	/** Baud rates. */

	enum BaudRate { BaudOther = -1, Baud300 = 300, Baud600 = 600,
					Baud1200 = 1200, Baud1800 = 1800, Baud2400 = 2400,
					Baud4800 = 4800, Baud9600 = 9600, Baud19200 = 19200,
					Baud38400 = 38400, Baud57600 = 57600,
					Baud115200 = 115200, Baud230400 = 230400 };

	/** Parity types = "?noe" */

	enum Parity { ParityOther = -1, ParityNone, ParityOdd, ParityEven };

	static const Buffer parityEncoding;

	/** Data bits per byte. */
	
	enum DataBits { DataBitsOther = -1, DataBits5 = 5, DataBits6 = 6,
					DataBits7 = 7, DataBits8 = 8 };

	/** Stop bits. */
	
	enum StopBits { StopBitsOther = -1, StopBits1 = 1, StopBits2 = 2 };

	/** Flow control modes  = "?ohx" */

	enum FlowControl { FlowOther = -1, FlowOff, FlowHardware, FlowXOnXOff };

	static const Buffer flowcontrolEncoding;

	/** Construct a new SerialPort for the given device.
	 *
	 * @param device The device: a serial device file like
	 * "/dev/ttyS0" on Linux or "/dev/cu.serial0" on Mac OS X, or a
	 * label like "COM1" on Windows (these are the defaults)
	 * @param mode The mode for opening the device
	 */
	
	SerialPort(
		const	Buffer &	device = "",
		const	Uint32		mode = Read | Write
	);

	virtual ~SerialPort(
	);

	/** Open device, and clear local echo
	 *
	 * @throw Failed If local echo cannot be changed
	 */

	virtual void open(
	);

	/** Flush any pending I/O on the connection.
	 *
	 * @throw IOException If an I/O error occurs, or if the conneciton is not
	 * open.
	 */

	void flush();

	/** Set connections settings.
	 *
	 * @param settings String containing all settings for the connection (baudrate,parity,databits,stopbits,flowcontrol)
	 */
	
	void setConfig(const Buffer& settings);

	/** Get connections settings.
	 *
	 * @return settings String containing all settings for the connection.
	 */
	
	Buffer getConfig();

	/** Set the baud rate for the connection.
	 *
	 * @param baud The new baud rate.
	 * @throw IOException If an I/O error occurs, or if the supplied baud rate
	 * is <code>BaudOther</code>, or if the connection is not open.
	 */
	
	void setBaudRate(BaudRate baud);

	/** Get the baud rate for the connection.
	 *
	 * @return The current baud rate; <code>BaudOther</code> if the baud rate
	 * is not one of the predefined enum values.
	 * @throw IOException If an I/O error occurs, or if the connection is not
	 * open.
	 */
	
	BaudRate getBaudRate();

	/** Set the parity type for the connection.
	 *
	 * @param parity The new parity type.
	 * @throw IOException If an I/O error occurs, or if the supplied parity
	 * type is <code>ParityOther</code>, or if the connection is not open.
	 */    

	void setParity(Parity parity);

	/** Get the parity type for the connection.
	 *
	 * @return The current parity type; <code>ParityOther</code> if the parity
	 * type is not one of the predefined enum values.
	 * @throw IOException If an I/O error occurs, or if the connection is not
	 * open.
	 */
	
	Parity getParity();

	/** Set the number of data bits for the connection.
	 *
	 * @param bits The new data bits value.
	 * @throw IOException If an I/O error occurs, or if the supplied data bits
	 * value is <code>DataBitsOther</code>, or if the connection is not open.
	 */    
	
	void setDataBits(DataBits bits);

	/** Get the data bits value for the connection.
	 *
	 * @return The current data bits value; <code>DataBitsOther</code> if the
	 * value is not one of the predefined enum values.
	 * @throw IOException If an I/O error occurs, or if the connection is not
	 * open.
	 */
	
	DataBits getDataBits();

	/** Set the number of stop bits for the connection.
	 *
	 * @param bits The new stop bits value.
	 * @throw IOException If an I/O error occurs, or if the supplied stop bits
	 * value is <code>StopBitsOther</code>, or if the connection is not open.
	 */    
	
	void setStopBits(StopBits bits);

	/** Get the stop bits value for the connection.
	 *
	 * @return The current stop bits value; <code>StopBitsOther</code> if the
	 * value is not one of the predefined enum values.
	 * @throw IOException If an I/O error occurs, or if the connection is not
	 * open.
	 */
	
	StopBits getStopBits();

	/** Set the flow control mode for the connection.
	 *
	 * @param flow The new flow control mode.
	 * @throw IOException If an I/O error occurs, or if the supplied flow
	 * control mode is <code>FlowControlOther</code>, or if the connection is
	 * not open.
	 */    
	
	void setFlowControl(FlowControl flow);

	/** Get the flow control mode for the connection.
	 *
	 * @return The current flow control mode; <code>FlowControlOther</code> if
	 * the flow control mode is not one of the predefined enum values.
	 * @throw IOException If an I/O error occurs, or if the connection is not
	 * open.
	 */
	
	FlowControl getFlowControl();

	/** Send a break on the serial port for the specified amount of time.
	 *
	 * @param duration The duration. On Windows, the duration is
	 * specified in milliseconds. On other platforms, the meaning of
	 * the value is implementation-defined.
	 */

	void sendBreak(Uint32 duration);

	/** Get the BaudRate enumeration value for the given baud rate value.
	 *
	 * @baudRate A baud rate.
	 * @return The BaudRate value that is closest to, but not greater than,
	 * the given baud rate.
	 */
	
	static BaudRate lookupBaudRate(Uint32 baudRate);

	//	for the error codes...

	BASE_CLASS_GEN_EXCEPTION("BFC.IO.SerialPort");

	CLASS_EXCEPTION(InvalidMode, "Invalid mode");
	CLASS_EXCEPTION(Failed, "Failed");

protected :

private :

	SerialPort(
		const SerialPort&
	);

	SerialPort& operator = (
		const SerialPort&
	);

};

// ============================================================================

} // namespace IO
} // namespace BFC

// ============================================================================

#endif // _BFC_IO_SerialPort_H_

// ============================================================================

