#pragma once

#ifndef MEM_MODEL_H
#define MEM_MODEL_H


class mem_model
{
    public:
        mem_model(
                unsigned int address_width,
                unsigned int data_width_bytes,
				unsigned int addr_offset
            ) : _address_width(address_width) ,
                _data_width_bytes(data_width_bytes),
                _buffer_size(2 << _address_width),
				_addr_offset(addr_offset)
                {};

        virtual ~mem_model() {};

		virtual void		set_buffer_size(unsigned int size) 
		{_buffer_size = size;}

		virtual void		set_address_width(unsigned int width)
		{_address_width = width;}

		virtual void		set_data_width(unsigned int width)
		{_data_width_bytes = width;}

		virtual void		set_addr_offset(unsigned int offset)
		{_addr_offset = offset;}

        virtual unsigned int buffer_size()      { return _buffer_size;}
        virtual unsigned int address_width()    { return _address_width;}
        virtual unsigned int address_start()    { return _data_width_bytes >> 1;}
        virtual unsigned int address_mask()     { return ((1 << (_address_width + 1)) -1) & ~((1 << (_data_width_bytes-1)) -1); }
        virtual unsigned int data_width_bytes() { return _data_width_bytes ;}
		virtual unsigned int address_lsb()
		{
			int j = _data_width_bytes;
			int i = 0 ;
			while (j) {
				j >>=1;
				i++;
			};
			return i-1;
		}

		virtual unsigned char byte_mask(unsigned int byte)	
			{ return 0xFF; }	//Bits active in this byte


        virtual int setBytes(unsigned int offset, unsigned char * data, unsigned int length) = 0;
        virtual int getBytes(unsigned int offset, unsigned char * data, unsigned int length) = 0;

		virtual int setByte(unsigned int offset, unsigned char data) = 0;
		virtual unsigned char getByte(unsigned int offset) = 0;
		virtual char * getAddrPin(unsigned int pin)	{	return "not supported"; };
		virtual char * getDataPin(unsigned int pin)	{	return "not supported"; };
		virtual char * getDevName() = 0;


    protected:
		int _addr_offset;

    private:
        int _address_width;
        int _data_width_bytes;
        int _buffer_size;
};

#endif // MEM_MODEL_H
