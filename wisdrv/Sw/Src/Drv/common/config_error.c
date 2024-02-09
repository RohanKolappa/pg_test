/******************************************************************************
*
*    Copyright WIS Technologies (c) (2003)
*    All Rights Reserved
*
*******************************************************************************
*
*    FILE: 
*        config_error.c
*
*    DESCRIPTION:
*    
*   
*
*    AUTHOR:
*	
*
*    <SOURCE CONTROL TAGS TBD>
*
******************************************************************************/

#include "config_error.h"

char* config_error_message[] =
{
	"success",
	"file not exist",
	"file corrupted",
	"file version mismatch",

	"incomplete configuration",
	"too much entry",
	"invalid resolution",
	"invalid sequence mode",
	"invalid gop size",
	"invalid gop mode",
	"invalid dvd compliant parameters",
	"invalid aspect ratio",
	"invalid mpeg4 four character code",
	"invalid inverse telecine parameters",
	"invalid field picture mode",
	"invalid deinterlace mode",
	"invalid bitrate control parameters",
	"invalid iip parameters",
	"invalid drop frame parameters",
	"invalid resolution for specific tv standard",
	"invalid fps for specific tv standard",
	"invalid iip filter parameters",
	"invalid search range parameters",
	"invalid stream header parameters",
	"invalid video capabilities",

	"incompatible format extension",

	"unknown stream parameter",
	"unknown bitrate parameter",
	"unknown resolution parameter",
	"unknown framerate parameter",

	"hardware limitation",
	"firmware limitation",
	"driver limitation",
	"software limitation",

	"invalid sensor setting",
	"invalid stream setting",
	"invalid audio setting",

	"parser: too many config entries",
	"parser: invalid entry name",
	"parser: duplicated entry name",
	"parser: symbol not found",
	"parser: invalid tv standard",

	"parser: mandetory stream parameter missing",
	"parser: invalid compress mode",
	"parser: invalid sequence mode",
	"parser: invalid mpeg4 mode",
	"parser: invalid dvd compliant",	
	"parser: invalid deinterlace mode",
	"parser: invalid search range",
	"parser: invalid aspect ratio",

	"parser: mandetory framerate parameter missing",

	"parser: mandetory bitrate parameter missing",
	"parser: invalid target bitrate or initial Q",
	"parser: invalid peak bitrate",
	"parser: invalid converge speed",
	"parser: invalid lambda",
	"parser: invalid quantize parameters",
								    
	"parser: mandetory resolution parameter missing",
	"parser: invalid bitrate range",
	"parser: invalid resolution scale and offset",

	"out of memory",
	"other error",
	"unknown error"
};

/****************************** end of config_error.c ************************/

