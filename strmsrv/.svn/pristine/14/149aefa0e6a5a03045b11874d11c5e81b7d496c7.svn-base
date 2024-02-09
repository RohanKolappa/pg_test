/*
 * FFUtils.hh
 *
 *  Created on: Aug 2, 2012
 *      Author: rkale
 */

#ifndef FFUTILS_HH_
#define FFUTILS_HH_

#include <string>
#include <vector>

class CFFUtils
{
public:
    static bool GetTrackDirForSpeed(std::string mediaDir, double speed, std::string & trackDir);
    static void DeleteAllTrackDirectories(std::string mediaDir);

    static void DeleteThumbnailDirectory(std::string mediaDir);

    static bool IsDirectory(std::string path);
    static bool CreateDirectory(std::string directoryPath);
    static bool DeleteDirectoryTree(std::string path);
    static int GetDirectoryListing(std::string path,
            std::vector<std::string> & directoryList, bool onlyDirNames = false);

    static bool ParseSpeed(const char * speed, double & speedValue);

    static bool ParseTrackList(std::string input, std::vector<int> & speedList);

private:

    static void GetValidTrackDirectories(std::string mediaDir, std::vector<int> & speedList);
    static int GetBestTrack(double speed, std::vector<int> & speedList);
    static bool ValidateAndAddSpeed(std::string speedItem, std::vector<int> & speedList);
};

#endif /* FFUTILS_HH_ */
