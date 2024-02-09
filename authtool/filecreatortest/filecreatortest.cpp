// filecreatortest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include <stdlib.h>


#include "IFileCreator.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if( argc < 3)
	{
		printf(" usage: filecreator.exe <blob dir> <outputfilename> <input filetype>\n");
		printf(" usage: input filetype --> 2 for .ts and 3 for .mp4\n");
	    return 0;
	}

	IFileCreator* fc = IFileCreator::CreateObj(_tstoi(argv[3]),argv[1],argv[2],0,0,1);
	    
	if(fc == NULL)
	{
	   printf(" No FileCreator object created \n");
	   return 0;
	}
	
	int ret = fc->Initialize();
	if(ret)
	{
		printf(" Initialize failed\n");
		return 0;
	}
	
	ret = fc->CreateFile();
	if(ret)
	{
		printf(" CreateFile failed\n");
		return 0;
	}
  
    delete fc;
	return 0;
}

