// mediaconverter.cpp : Defines the entry point for the console application.


#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <stdlib.h>
#include <boost/program_options.hpp>

#include "libmediaconverter.hh"
#include "MatroskaFileWriter.hh"
#include "MinuteFileWriter.hh"
#include "MatroskaFileInfo.hh"
#include "EditList.hh"

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
#include "FFMediaProcessor.hh"
#include "AdaptiveMediaGenerator.hh"
#endif

using namespace mediaindex;
namespace po = boost::program_options;
using namespace std;

#define MC_INFO_XML_FAILED 2
#define MC_UN_HANDELED_EXCEPTION 3
#define MC_UN_KNOWN_EXCEPTION 4

// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}

// Function used to check that 'opt1' and 'opt2' are not specified
// at the same time.
void conflicting_options(const po::variables_map& vm,
                         const char* opt1, const char* opt2)
{
    if (vm.count(opt1) && !vm[opt1].defaulted()
        && vm.count(opt2) && !vm[opt2].defaulted())
        throw invalid_argument(string("Conflicting options '")
                          + opt1 + "' and '" + opt2 + "'.");
}

// Function used to check that of 'for_what' is specified, then
//   'required_option' is specified too.
void option_dependency(const po::variables_map& vm,
                        const char* for_what, const char* required_option)
{
    if (vm.count(for_what) && !vm[for_what].defaulted())
        if (vm.count(required_option) == 0 || vm[required_option].defaulted())
            throw invalid_argument(string("Option '") + for_what
                              + "' requires option '" + required_option + "'.");
}


void usage(const string& a_rstrProgramName)
{
    clog << "Usage: " << endl;
    clog << a_rstrProgramName << " -c -f out_file -b blob_dir [-d db_dir]" << endl;
    clog << " or " << endl;
    clog << a_rstrProgramName << " -N -t stream_type -s sdp_file  -f out_file -b blob_dir" << endl;

    clog << " or " << endl;
    clog << a_rstrProgramName << " -x -f in_file -b blob_dir [-d db_dir] [-C cutlist]" << endl;
    clog << " or " << endl;
    clog << a_rstrProgramName << " -x -f in_file -b blob_dir [-d db_dir] [-C cutlist] [-H HLS] [-T thumbnail]" << endl;
    clog << " or " << endl;
    clog << a_rstrProgramName << " -i <metadata|bookmarks|size|all> -f in_file [-C cutlist]" << endl;
}

bool IsHLSSupported(po::variables_map &a_vm, const string& a_rstrProgramName)
{
    if (!a_vm["HLS"].as<bool>()) {
        return false;
    }

    string strStreamType = (a_vm.count("streamtype") == 0 ? "" :
                            a_vm["streamtype"].as<string>());

    string strSDPFile = (a_vm.count("sdpfile") == 0 ? "" :
                         a_vm["sdpfile"].as<string>());

    if (strcmp(strStreamType.c_str(), "RTP") == 0) {
        if (strSDPFile.empty()) {
            cerr << "Error: SDP file is required for RTP Streams. \n";
            usage(a_rstrProgramName);
            return false;
        }
        else {
            return true;
        }
    }

    return false;
}

int main(int argc, char** argv)
{
    int retVal = 0;
    CEditList * editList = NULL;

    try {

       //To display the allowable options when we run the program

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Produces this help message")
            ("create,c",  po::bool_switch(), "Create media archive")
            ("native,N", po::bool_switch(), "Create media in native format")
            ("extract,x",  po::bool_switch(), "Extract media archive")
            ("info,i", po::value<string>(), "Get information")
            ("file,f",  po::value<string>(), "Input or Output file name")
            ("db-dir,d",  po::value<string>(), "Input or Output db dir")
            ("blob-dir,b",  po::value<string>(), "Input or Output blob name")
            ("cutlist,C",  po::value<string>(), "Comma separated list of cutpoints")
            ("streamtype,t", po::value<string>(), "Stream type. [V2D|UDP|RTP|MPEGTS|SNAPSHOT]")
            ("sdpfile,s", po::value<string>(), "Path to SDP file")
            ("args,a",  po::value<string>(), "Argument values. Ex: Offset for gettimecodeatoffset etc.")
            ("HLS,H",  po::bool_switch(), "Create HLS content")
            ("thumbnail,T",  po::bool_switch(), "Create Thumbnail while extracting mkd file")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if ( (argc == 1) || vm.count("help") ) {
            clog << desc << endl;
            return 0;
        }

        conflicting_options(vm, "create", "extract");
        conflicting_options(vm, "create", "info");
        conflicting_options(vm, "extract", "info");
        conflicting_options(vm, "native", "create");
        conflicting_options(vm, "native", "extract");
        conflicting_options(vm, "native", "info");
        option_dependency(vm, "create", "file");
        option_dependency(vm, "extract", "file");
        option_dependency(vm, "info", "file");
        option_dependency(vm, "create", "blob-dir");
        option_dependency(vm, "extract", "blob-dir");
        option_dependency(vm, "native", "file");
        option_dependency(vm, "native", "streamtype");
        option_dependency(vm, "native", "blob-dir");


        string strMediaFile = vm["file"].as<string>();
        string strDbDir = (vm.count("db-dir") == 0) ? "" :
            vm["db-dir"].as<string>();
        string strBlobDir = (vm.count("blob-dir") == 0) ? "" :
            vm["blob-dir"].as<string>();

        string strInfo = (vm.count("info") == 0) ? "" :
            vm["info"].as<string>();

        string strArgs = (vm.count("args") == 0) ? "" :
                    vm["args"].as<string>();

        if (vm.count("cutlist") > 0) {
            string cutListText = vm["cutlist"].as<string>();
            editList = new CEditList();
            if (!editList->Set(cutListText)) {
                throw invalid_argument(string("Invalid cutlist '") + cutListText + "'");
            }
        }

        string strStreamType = (vm.count("streamtype") == 0 ? "" :
                         vm["streamtype"].as<string>());

        string strSDPFile = (vm.count("sdpfile") == 0 ? "" :
                         vm["sdpfile"].as<string>());

        if ( (vm["create"].as<bool>()) ) {
            // Create indicates converting Minute file to Matroska format.
            CMatroskaFileWriter mkWriter(strMediaFile, strDbDir, strBlobDir);
            if (editList != NULL) {
                mkWriter.SetEditList(editList);
            }
            retVal = mkWriter.Write();
        }

        else if (vm["extract"].as<bool>()) {
            // Extract indicates converting Matroska format to Minute file.
            CMinuteFileWriter writer(strMediaFile, strDbDir, strBlobDir);

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
            CAdaptiveMediaGenerator *pMediaGenerator = NULL;
            if (IsHLSSupported(vm, argv[0])) {
                pMediaGenerator = new CAdaptiveMediaGenerator(strBlobDir, strSDPFile, 0, false);
                writer.AddMediaProcessor(pMediaGenerator);
            }

            std::vector<int >trackSpeeds;
            trackSpeeds.push_back(8);
            trackSpeeds.push_back(64);
            trackSpeeds.push_back(256);

            CFFMediaProcessor *pTNGenerator = NULL;
            if (vm["thumbnail"].as<bool>())
            {
                pTNGenerator = new CFFMediaProcessor(strMediaFile, strBlobDir,
                                                     trackSpeeds, strStreamType, 0,
                                                     false, true, strSDPFile);
                pTNGenerator->EnablePrimaryTN(true);
                pTNGenerator->EnableFFTracks(false);
                pTNGenerator->InitMedia();
                writer.AddMediaProcessor(pTNGenerator);
            }
#endif

            if (editList != NULL)
                writer.SetEditList(editList);
            retVal = writer.Write();
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
             delete pMediaGenerator;
             delete pTNGenerator;
#endif
        }

        else if (strInfo.size() > 0) {
            CMatroskaFileInfo fileInfo(strMediaFile);
            if (editList != NULL)
                fileInfo.SetEditList(editList);
            bool result = fileInfo.PrintInfo(strInfo, strArgs);
            if (!result)
                retVal = MC_INFO_XML_FAILED;
        }
        else if (vm["native"].as<bool>()) {
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__)
            //Create standard format video file.
            if (strcmp(strStreamType.c_str(), "RTP") == 0) {
                if (strSDPFile.empty()) {
                    cerr << "Error: SDP file is required for RTP Streams. \n";
                    usage(argv[0]);

                    return 0;
                }
            }
            std::vector<int >trackSpeeds;
            trackSpeeds.push_back(8);
            trackSpeeds.push_back(64);
            trackSpeeds.push_back(256);

            CFFMediaProcessor ffMediaProcessor(strBlobDir, strBlobDir,
                                               trackSpeeds, strStreamType,
                                               0, false, true, strSDPFile);
            ffMediaProcessor.EnableFFTracks(false);
            ffMediaProcessor.SetNativeOutputFileName(strMediaFile);
            ffMediaProcessor.SetNativeOutputMode();
            if (!ffMediaProcessor.ProcessMedia())
                printf("FFMediaProcessor::ProcessMedia failed \n");
#endif
        }
    }
    catch(exception& e) {
        cerr << "Error: " << e.what() << "\n";
        usage(argv[0]);
        retVal = MC_UN_HANDELED_EXCEPTION;
    }
    catch(...) {
        cerr << "Unknown Exception\n";
        retVal = MC_UN_KNOWN_EXCEPTION;
    }

    delete editList;

	return retVal;
}

