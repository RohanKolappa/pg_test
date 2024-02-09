// IPVSTranscode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IFileCreator.h"
#include "IPVSTranscodeVersion.h"

#include <iostream>
#include <string>
using namespace std;

#include "Options.h"

const int ETRANSCODE_OK             = 0;
const int ETRANSCODE_OUTOFMEMORY    = -1;
const int ETRANSCODE_WRONGARGS      = -2;
const int ETRANSCODE_CREATEFILE     = -3;

#ifdef UNICODE_BUILD
void ProcessArguments(AnyOption *a_pOptions, const int argc, wchar_t* argv[]);
#else
void ProcessArguments(AnyOption *a_pOptions, const int argc, char* argv[]);
#endif

bool FillArgList(AnyOption* a_pOptions, std::string& a_rStrBlobDir,
                 std::string& a_rStrOutFile, StTranscodeOpts& a_rStArgList,
                 std::string& a_rStrErrorMsg);

int IsArgsAreValid(int a_nArgs, char* a_strList[]);

#define PRINTCMDUSAGE \
    cout << "Usage: " << endl; \
    cout << argv[0] << " "; \
    cout << "--infile in_file --outfile out_file [--fps n] "; \
    cout << "[--startoffset n] [--duration n]" << endl; \
    cout << "[-t or --track audio|video|both]" << endl; \
    pOptions->printUsage();    

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
#define _tmain main
#endif

#ifdef UNICODE_BUILD
int _tmain(int argc, wchar_t* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    int iRetCode                = ETRANSCODE_OK;
    IFileCreator* pFileCreator  = NULL;
	AnyOption *pOptions         = new AnyOption();
	
    do {
		if((iRetCode = IsArgsAreValid(argc, argv)) != 0)
		{
			cerr << "Invalid Argument : " << argv[iRetCode] << endl;
			PRINTCMDUSAGE;
			break;
		}

        ProcessArguments(pOptions, argc, argv);

        if( !pOptions->hasOptions() || pOptions->getFlag('h') )
        { 
            PRINTCMDUSAGE;
            break;        
        }

        if( pOptions->getFlag("version")) 
        {
            char strVersion[128];
            sprintf(strVersion,"%d.%d.r%d.b%d", CXFILEVER);
            cout << argv[0];
            cout << ":" << "Version = " << strVersion << endl;	
            break;
        }

        StTranscodeOpts stArgList; 
        std::string strArgParsingErrorMsg;
        std::string strBlobDir;
        std::string strOutFile;

        if(!FillArgList(pOptions, strBlobDir, strOutFile,
                        stArgList, strArgParsingErrorMsg))
        {            
            cerr << strArgParsingErrorMsg << endl;
            PRINTCMDUSAGE;
            iRetCode = ETRANSCODE_WRONGARGS;
            break;
        }

        pFileCreator = IFileCreator::CreateObj(strBlobDir, strOutFile,
                                               stArgList);

        if(pFileCreator == NULL)
        {
            cerr << " Failed to create the V2D transcoder object " << endl;
            iRetCode = ETRANSCODE_OUTOFMEMORY;
            break;
        }

        int nRetVal = pFileCreator->Initialize();
        if(nRetVal)
        {
            cerr << " Failed in initializing the V2D transcoder object " << endl;
            iRetCode = ETRANSCODE_WRONGARGS;
            break;
        }

        nRetVal = pFileCreator->SetTargetFps(stArgList.nTargetFps);
        if(nRetVal)
        {
            cerr << " Failed in setting the target fps. " << endl;
            iRetCode = ETRANSCODE_WRONGARGS;
            break;
        }

        nRetVal = pFileCreator->CreateFile();
        if(nRetVal)
        {
            cerr << "Failed to create the output file. " << endl;
            iRetCode = ETRANSCODE_CREATEFILE;
            break;
        }

    } while(0);

	delete pOptions;
    delete pFileCreator;
	return iRetCode;
}

#ifdef UNICODE_BUILD
void ProcessArguments(AnyOption *a_pOptions, const int a_nArgc, wchar_t* a_pArgv[])
#else 
void ProcessArguments(AnyOption *a_pOptions, const int a_nArgc, char* a_pArgv[])
#endif
{
	a_pOptions->addUsage("Options : ");
	
	a_pOptions->addUsage(" -f or --fps Target Fps ");
	a_pOptions->addUsage(" -o or --outfile OutPut FileName/path");
	a_pOptions->addUsage(" -i or --infile Input directory path");
	a_pOptions->addUsage(" -s or --startoffset(Millisecs) Start Offset in Milli Secs");
	a_pOptions->addUsage(" -d or --duration(Millisecs) End Offset in Milli Secs");
	a_pOptions->addUsage(" -v or --version Displays the version");
	a_pOptions->addUsage(" -h or --help Displays help");
    a_pOptions->addUsage(" -t or --track audio|video|both");

	a_pOptions->setFlag("help", 'h');
	a_pOptions->setFlag("version", 'v');

	a_pOptions->setOption("infile", 'i');
	a_pOptions->setOption("outfile", 'o');
	a_pOptions->setOption("fps", 'f');
    a_pOptions->setOption("track", 't');

	a_pOptions->setOption("startoffset", 's');
	a_pOptions->setOption("duration", 'd');
	
    char * pCharArgv[DEFAULT_MAXOPTS];	
#ifdef UNICODE_BUILD	
	for(int nIndex = 0; nIndex < a_nArgc; nIndex++)
	{
		size_t nLen = wcslen(a_pArgv[nIndex]);
		pCharArgv[nIndex] = new char[nLen + 1];

		wcstombs( pCharArgv[nIndex], a_pArgv[nIndex], wcslen(a_pArgv[nIndex]) );
		pCharArgv[nIndex][nLen] = '\0';
	}
#else
    for(int nIndex = 0; nIndex < a_nArgc; nIndex++)
        pCharArgv[nIndex] = a_pArgv[nIndex];
#endif

	a_pOptions->processCommandArgs( a_nArgc, pCharArgv );

#ifdef UNICODE_BUILD
	for(int nIndex = 0; nIndex < a_nArgc; nIndex++)
	{
		delete [] pCharArgv[nIndex] ;
	}
#endif
}

bool FillArgList(AnyOption* a_pOptions, std::string& a_rStrBlobDir, 
                 std::string& a_rStrOutFile, StTranscodeOpts & a_rStArgList,
                 std::string& a_rStrErrorMsg)
{
    if(!a_pOptions->getValue('i'))
    {
        a_rStrErrorMsg = "Input directory must be specified.";
        return false;
    }

    if(!a_pOptions->getValue('o'))
    {
        a_rStrErrorMsg = "Output filename must be specified.";
        return false;
    }

    if(!a_pOptions->getValue('t'))
    {
        a_rStArgList.strAVMode = "both";
    }
    else
    {
        std::string strAVMode = a_pOptions->getValue('t');

        if(strAVMode != "both" && strAVMode != "audio" 
                                && strAVMode != "video")
        {
            a_rStrErrorMsg = "Invalid av mode(--track or -t) option, valid options are audio|video|both.";
            return false;
        }
        
        a_rStArgList.strAVMode = strAVMode;
    }
    
    a_rStrBlobDir = a_pOptions->getValue('i');
    a_rStrOutFile = a_pOptions->getValue('o');
    a_rStArgList.nStartOffset = 0;
    a_rStArgList.nDurationTime = 0;
    a_rStArgList.nTargetFps = 30;

	if( a_pOptions->getValue("startoffset") )
	{
        a_rStArgList.nStartOffset = ::atol(a_pOptions->getValue("startoffset"));
		if(a_rStArgList.nStartOffset < 0)
		{
            a_rStrErrorMsg = "Start offset is not correct.";
			return false;
		}
	}
	
	if( a_pOptions->getValue("duration") )
	{
        a_rStArgList.nDurationTime = ::atol(a_pOptions->getValue("duration"));
	}

	if( a_pOptions->getValue("fps") )
	{
        a_rStArgList.nTargetFps = ::atoi(a_pOptions->getValue("fps"));
	}

	return true;
}

int IsArgsAreValid(int a_nArgs, char* a_strList[])
{
	int nIndex = 0;
	int nLen = 0;

	while(nIndex != (a_nArgs - 1))
	{
		nIndex++;

		if(a_strList[nIndex][0] != '-')
			continue;
		else
		{
			if(a_strList[nIndex][1] != '-')
			{
				nLen = ::strlen(a_strList[nIndex]);
				if(nLen > 2)
					return nIndex;
			}
			else
			{
				nLen = ::strlen(a_strList[nIndex]);
				if(nLen <= 3)
					return nIndex;
			}
		}
	};

	return 0;
}
