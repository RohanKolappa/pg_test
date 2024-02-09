// filecreator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"

#include "IFileCreator.h"


int main(int argc, char* argv[])
{
	
    if( argc < 3)
	{
		printf(" usage: filecreator.exe <blob dir> <outputfilename>\n");
	    return 0;
	}

	IFileCreator* fc = IFileCreator::CreateObj(argv[1],argv[2]);
    
	if(fc == NULL)
	{
	   printf(" No FileCreator object created \n");
	   return 0;
	}
	
	int ret = fc->;
	if(!ret)
	{
		printf(" Init failed\n");
		return 0;
	}

	ret = fc->CreateFile();
	if(!ret)
	{
		printf(" CreateFile failed\n");
		return 0;
	}
  
	return 0;
}
