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
//	BFC.IO.SerialPort.cpp
// 
// Description:
//	...
// 
// Author(s):
//	Kester AERNOUDT
// 
// ============================================================================

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#ifndef PLATFORM_WIN32
#include <termios.h>
#include <sys/types.h>
#else
#include <windows.h>
#include <io.h>
#define TO_HANDLE(fd) (HANDLE)_get_osfhandle(fd)
#endif

#include "BFC.Base.BufferParser.h"
#include "BFC.IO.SerialPort.h"

// ============================================================================

BFC_PTR_IMPL_NS2( BFC, IO, SerialPort )

// ============================================================================

using namespace BFC;
using namespace IO;

// ============================================================================

#ifdef PLATFORM_WIN32
#  define DEFAULT_DEVICE "COM1"
#else
#  define DEFAULT_DEVICE "/dev/ttyS0"
#endif

SerialPort::SerialPort(
		const	Buffer &	device,
		const	Uint32		mode
) : File(device.isEmpty()?DEFAULT_DEVICE:device, mode & (Read | Write | NonBlocking)) {

	setObjectName( "BFC.IO.SerialPort" );

	if (mode & ~(Read | Write | NonBlocking))
		msgWrn("Strange mode for serial port...");

}

SerialPort::~SerialPort() {

}

// ============================================================================

void SerialPort::open() {
	File::open();

#ifdef PLATFORM_WIN32
// extra init for WIN32
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 1;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(TO_HANDLE(fd), &timeouts))
		throw Failed(::strerror( errno ));
#else
	// extra initialization
	
	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));
	
	ts.c_lflag = 0;
	ts.c_oflag = 0;
	ts.c_cc[VTIME] = 0;
	ts.c_cc[VMIN] = 1;
	
	if(::tcsetattr(fd, TCSANOW, &ts) != 0)
		throw Failed(::strerror( errno ));
	
	tcflush(fd, TCOFLUSH);  
	tcflush(fd, TCIFLUSH);
	
#endif
}

// ============================================================================

void SerialPort::setBaudRate(SerialPort::BaudRate baud) {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#if defined( PLATFORM_WIN32 )

	DCB dcb;

	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	switch(baud)
	{
		case Baud300:
			dcb.BaudRate = CBR_300;
			break;

		case Baud600:
			dcb.BaudRate = CBR_600;
			break;

		case Baud1200:
			dcb.BaudRate = CBR_1200;
			break;

		case Baud2400:
			dcb.BaudRate = CBR_2400;
			break;

		case Baud4800:
			dcb.BaudRate = CBR_4800;
			break;
			
		case Baud9600:
			dcb.BaudRate = CBR_9600;
			break;

		case Baud19200:
			dcb.BaudRate = CBR_19200;
			break;

		case Baud38400:
			dcb.BaudRate = CBR_38400;
			break;

		case Baud57600:
			dcb.BaudRate = CBR_57600;
			break;

		case Baud115200:
			dcb.BaudRate = CBR_115200;
			break;

		case Baud230400:
			dcb.BaudRate = 230400;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
	}
	
	if(! ::SetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));
	
#else
	
	struct termios ts;
	speed_t speed = B0;

	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	switch(baud)
	{
		case Baud300:
			speed = B300;
			break;

		case Baud600:
			speed = B600;
			break;

		case Baud1200:
			speed = B1200;
			break;

		case Baud1800:
			speed = B1800;
			break;

		case Baud2400:
			speed = B2400;
			break;

		case Baud4800:
			speed = B4800;
			break;

		case Baud9600:
			speed = B9600;
			break;

		case Baud19200:
			speed = B19200;
			break;

		case Baud38400:
			speed = B38400;
			break;

#ifdef B57600			
		case Baud57600:
			speed = B57600;
			break;
#endif
			
#ifdef B115200
		case Baud115200:
			speed = B115200;
			break;
#endif

#ifdef B230400
		case Baud230400:
			speed = B230400;
			break;
#endif

		default:
			throw InvalidMode("invalid baud rate");
	}

	::cfsetispeed(&ts, speed);
	::cfsetospeed(&ts, speed);

	if(::tcsetattr(fd, TCSANOW, &ts) != 0)
		throw Failed(::strerror( errno ));

#endif
}

SerialPort::BaudRate SerialPort::getBaudRate() {
	if(! isConnected())
		throw NotConnected();

#ifdef PLATFORM_WIN32

	DCB dcb;

	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	BaudRate baud;
	
	switch(dcb.BaudRate)
	{
		case CBR_300:
			baud = Baud300;
			break;

		case CBR_600:
			baud = Baud600;
			break;

		case CBR_1200:
			baud = Baud1200;
			break;

		case CBR_2400:
			baud = Baud2400;
			break;

		case CBR_4800:
			baud = Baud4800;
			break;

		case CBR_9600:
			baud = Baud9600;
			break;

		case CBR_19200:
			baud = Baud19200;
			break;

		case CBR_38400:
			baud = Baud38400;
			break;

		case CBR_57600:
			baud = Baud57600;
			break;

		case CBR_115200:
			baud = Baud115200;
			break;

		case 230400:
			baud = Baud230400;
			break;

		default:
			baud = BaudOther;
			break;
	}

	return(baud);
	
#else

	struct termios ts;
	BaudRate baud;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	speed_t speed = ::cfgetispeed(&ts);
	speed_t ospeed = ::cfgetospeed(&ts);

	if(speed != ospeed)
		return(BaudOther);

	switch(speed)
	{
		case B300:
			baud = Baud300;
			break;

		case B600:
			baud = Baud600;
			break;

		case B1200:
			baud = Baud1200;
			break;

		case B2400:
			baud = Baud2400;
			break;

		case B4800:
			baud = Baud4800;
			break;

		case B9600:
			baud = Baud9600;
			break;

		case B19200:
			baud = Baud19200;
			break;

		case B38400:
			baud = Baud38400;
			break;

#ifdef B57600
		case B57600:
			baud = Baud57600;
			break;
#endif

#ifdef B115200
		case B115200:
			baud = Baud115200;
			break;
#endif
	
#ifdef B230400
		case B230400:
			baud = Baud230400;
			break;
#endif

		default:
			baud = BaudOther;
			break;
	}

	return(baud);

#endif
}

// ============================================================================

void SerialPort::setParity(SerialPort::Parity parity) {
	if(! isConnected())
		throw NotConnected();

#ifdef PLATFORM_WIN32

	DCB dcb;

	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	switch(parity)
	{
		case ParityNone:
			dcb.Parity = NOPARITY;
			break;

		case ParityOdd:
			dcb.Parity = ODDPARITY;
			break;

		case ParityEven:
			dcb.Parity = EVENPARITY;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
			break;
	}
	
	if(! ::SetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));
	
#else

	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	switch(parity)
	{
		case ParityNone:
			ts.c_cflag &= ~PARENB;
			break;

		case ParityOdd:
			ts.c_cflag |= (PARENB | PARODD);
			break;

		case ParityEven:
			ts.c_cflag |= PARENB;
			ts.c_cflag &= ~PARODD;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
			break;
	}

	if(::tcsetattr(fd, TCSANOW, &ts) != 0)
		throw Failed(::strerror( errno ));
	
#endif
}

/*
 */

SerialPort::Parity SerialPort::getParity() {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	DCB dcb;
	
	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	Parity parity;
	
	switch(dcb.Parity)
	{
		case NOPARITY:
			parity = ParityNone;
			break;

		case ODDPARITY:
			parity = ParityOdd;
			break;

		case EVENPARITY:
			parity = ParityEven;
			break;

		default:
			parity = ParityOther;
	}

	return(parity);
	
#else

	struct termios ts;
	Parity parity = ParityNone;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	if((ts.c_cflag & PARENB) != 0)
		parity = ((ts.c_cflag & PARODD) != 0) ? ParityOdd : ParityEven;

	return(parity);

#endif
}

// ============================================================================

void SerialPort::setDataBits(SerialPort::DataBits bits) {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	DCB dcb;
	
	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	switch(bits)
	{
		case DataBits5:
			dcb.ByteSize = 5;
			break;

		case DataBits6:
			dcb.ByteSize = 6;
			break;

		case DataBits7:
			dcb.ByteSize = 7;
			break;

		case DataBits8:
			dcb.ByteSize = 8;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
	}
	
	if(! ::SetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));
	
#else

	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	ts.c_cflag &= ~CSIZE;

	switch(bits)
	{
		case DataBits5:
			ts.c_cflag |= CS5;
			break;

		case DataBits6:
			ts.c_cflag |= CS6;
			break;

		case DataBits7:
			ts.c_cflag |= CS7;
			break;

		case DataBits8:
			ts.c_cflag |= CS8;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
	}

	if(::tcsetattr(fd, TCSANOW, &ts) != 0)
		throw Failed(::strerror( errno ));

#endif
}

/*
 */

SerialPort::DataBits SerialPort::getDataBits() {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	DCB dcb;
	
	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	DataBits bits;
	
	switch(dcb.ByteSize)
	{
		case 5:
			bits = DataBits5;
			break;

		case 6:
			bits = DataBits6;
			break;

		case 7:
			bits = DataBits7;
			break;

		case 8:
			bits = DataBits8;
			break;

		default:
			bits = DataBitsOther;
			break;
	}
	
	return(bits);
	
#else

	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	DataBits bits;

	switch (ts.c_cflag & CSIZE) {
	case CS5:	bits = DataBits5; break;
	case CS6:	bits = DataBits6; break;
	case CS7:	bits = DataBits7; break;
	case CS8:	bits = DataBits8; break;
	default:	bits = DataBitsOther;
	}

	return(bits);

#endif
}

// ============================================================================

void SerialPort::setStopBits(SerialPort::StopBits bits) {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	DCB dcb;
	
	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	switch(bits)
	{
		case StopBits1:
			dcb.StopBits = ONESTOPBIT;
			break;

		case StopBits2:
			dcb.StopBits = TWOSTOPBITS;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
	}
	
	if(! ::SetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));
	
#else

	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	switch(bits)
	{
		case StopBits1:
			ts.c_cflag &= ~CSTOPB;
			break;

		case StopBits2:
			ts.c_cflag |= CSTOPB;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
	}

	if(::tcsetattr(fd, TCSANOW, &ts) != 0)
		throw Failed(::strerror( errno ));

#endif
}

/*
 */

SerialPort::StopBits SerialPort::getStopBits() {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	DCB dcb;
	
	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	StopBits bits;
	
	switch(dcb.StopBits)
	{
		case ONESTOPBIT:
			bits = StopBits1;
			break;

		case TWOSTOPBITS:
			bits = StopBits2;
			break;

		default:
			bits = StopBitsOther;
			break;
	}
	
	return(bits);
	
#else

	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	StopBits bits = StopBits1;

	if((ts.c_cflag & CSTOPB) != 0)
		bits = StopBits2;

	return(bits);

#endif
}

// ============================================================================

void SerialPort::setFlowControl(SerialPort::FlowControl flow) {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	DCB dcb;

	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	switch(flow)
	{
		case FlowOff:
			dcb.fOutxCtsFlow = FALSE;
			dcb.fRtsControl = RTS_CONTROL_DISABLE;
			dcb.fInX = FALSE;
			dcb.fOutX = FALSE;
			break;
			
		case FlowHardware:
			dcb.fOutxCtsFlow = TRUE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			dcb.fInX = FALSE;
			dcb.fOutX = FALSE;
			break;
			
		case FlowXOnXOff:
			dcb.fOutxCtsFlow = FALSE;
			dcb.fRtsControl = RTS_CONTROL_DISABLE;
			dcb.fInX = TRUE;
			dcb.fOutX = TRUE;
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
	}
	
	if(! ::SetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

#else

	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	switch(flow)
	{
		case FlowOff:
			ts.c_cflag &= ~CRTSCTS;
			ts.c_iflag &= ~(IXON | IXOFF);
			break;

		case FlowHardware:
			ts.c_cflag |= CRTSCTS;
			ts.c_iflag &= ~(IXON | IXOFF);
			break;

		case FlowXOnXOff:
			ts.c_cflag &= ~CRTSCTS;
			ts.c_iflag |= (IXON | IXOFF);
			break;

		default:
			throw InvalidMode("unsupported value for parameter");
	}

	if(::tcsetattr(fd, TCSANOW, &ts) != 0)
		throw Failed(::strerror( errno ));

#endif
}

/*
 */

SerialPort::FlowControl SerialPort::getFlowControl() {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	DCB dcb;
	
	if(! ::GetCommState(TO_HANDLE(fd), &dcb))
		throw Failed(::strerror( errno ));

	FlowControl flow;
	
	if((dcb.fOutxCtsFlow == FALSE) && (dcb.fRtsControl == RTS_CONTROL_DISABLE)
		 && (dcb.fInX == FALSE) && (dcb.fOutX == FALSE))
		flow = FlowOff;
	else if((dcb.fOutxCtsFlow == TRUE)
					&& (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE)
					&& (dcb.fInX == FALSE) && (dcb.fOutX == FALSE))
		flow = FlowHardware;
	else if((dcb.fOutxCtsFlow = FALSE)
					&& (dcb.fRtsControl == RTS_CONTROL_DISABLE)
					&& (dcb.fInX == TRUE) && (dcb.fOutX == TRUE))
		flow = FlowXOnXOff;
	else
		flow = FlowOther;

	return(flow);
	
#else

	struct termios ts;
	
	if(::tcgetattr(fd, &ts) != 0)
		throw Failed(::strerror( errno ));

	FlowControl flow = FlowOther;

	if((ts.c_cflag & CRTSCTS) != 0)
		flow = FlowHardware;
	else if((~ts.c_iflag & (IXON | IXOFF)) == 0)
		flow = FlowXOnXOff;
	else if((ts.c_iflag & (IXON | IXOFF)) == 0)
		flow = FlowOff;
	else
		flow = FlowOther;

	return(flow);

#endif
}

// ============================================================================

void SerialPort::sendBreak(Uint32 duration) {
	if ( !isConnected() ) {
		throw NotConnected();
	}
	
#ifdef PLATFORM_WIN32

	::SetCommBreak(TO_HANDLE(fd));
	::Sleep(duration);
	::ClearCommBreak(TO_HANDLE(fd));
	
#else

	::tcsendbreak(fd, duration);
	
#endif
}

// ============================================================================

void SerialPort::flush() {
	if ( !isConnected() ) {
		throw NotConnected();
	}

#ifdef PLATFORM_WIN32

	if(! ::FlushFileBuffers(TO_HANDLE(fd)))
		throw Failed(::strerror( errno ));

	if(! ::PurgeComm(TO_HANDLE(fd), (PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
														 | PURGE_RXCLEAR)))
		throw Failed(::strerror( errno ));
	
#else

	if(::tcflush(fd, TCIOFLUSH) != 0)
		throw Failed(::strerror( errno ));

#endif
}

// ============================================================================

const Buffer SerialPort::parityEncoding = "?noe";

const Buffer SerialPort::flowcontrolEncoding = "?ohx";

void SerialPort::setConfig(const Buffer& settings) {
	BufferArray a = BufferParser::tokenize(settings, ",");
	if (a.size() > 0) {
		Uint32 baud = a[0].toUint32();
//msgDbg("Setting baud to " + Buffer(baud));
		setBaudRate(lookupBaudRate(baud));
	}
	if (a.size() > 1) {
		if (a[1].length() != 1)
			throw InvalidMode("Parity " + a[1]);
		Int32 pos = parityEncoding.findFwd(a[1][0]);
//msgDbg("Setting parity to " + Buffer(pos-1));
		setParity((Parity)(pos-1));
	}
	if (a.size() > 2) {
		Int32 databits = a[2].toUint32();
//msgDbg("Setting databits to " + Buffer(databits));
		setDataBits((DataBits)(databits));
	}
	if (a.size() > 3) {
		Int32 stopbits = a[3].toUint32();
//msgDbg("Setting stopbits to " + Buffer(stopbits));
		setStopBits((StopBits)(stopbits));
	}
	if (a.size() > 4) {
		if (a[4].length() != 1)
			throw InvalidMode("FlowControl" + a[4]);
		Int32 pos = flowcontrolEncoding.findFwd(a[4][0]);
//msgDbg("Setting flowcontrol to " + Buffer(pos-1));
		setFlowControl((FlowControl)(pos-1));
	}
}

Buffer SerialPort::getConfig() {
	Buffer output;

	BaudRate baud = getBaudRate();
	output = Buffer((Int32)baud);

	Parity par = getParity();
	output += ",";
	output += parityEncoding[par+1];

	DataBits databits = getDataBits();
	output += ",";
	output += Buffer((Int32)databits);


	StopBits stopbits = getStopBits();
	output += ",";
	output += Buffer((Int32)stopbits);


	FlowControl fc = getFlowControl();
	output += ",";
	output += flowcontrolEncoding[fc+1];

	return output;
}

// ============================================================================

SerialPort::BaudRate SerialPort::lookupBaudRate(Uint32 baudRate)
{
	if(baudRate <= 300)
		return(Baud300);
	else if(baudRate <= 600)
		return(Baud600);
	else if(baudRate <= 1200)
		return(Baud1200);
	else if(baudRate <= 1800)
		return(Baud1800);
	else if(baudRate <= 2400)
		return(Baud2400);
	else if(baudRate <= 4800)
		return(Baud4800);
	else if(baudRate <= 9600)
		return(Baud9600);
	else if(baudRate <= 19200)
		return(Baud19200);
	else if(baudRate <= 38400)
		return(Baud38400);
	else if(baudRate <= 57600)
		return(Baud57600);
	else if(baudRate <= 115200)
		return(Baud115200);
	else
		return(Baud230400);
}



