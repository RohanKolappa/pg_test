/*
 * FFUtils.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: rkale
 */



#include <sys/stat.h>
#include "strmsrv_compatibility.h"

#include <sstream>
#include "FFUtils.hh"
#include "common.hh"




bool CFFUtils::GetTrackDirForSpeed(std::string mediaDir, double speed, std::string & trackDir)
{
    SYSLOG_INFO("Function called %s\n", mediaDir.c_str());
    trackDir = "";

    if (!IsDirectory(mediaDir))
        return false;

    if (speed <= 1.0)
        return true;

    std::vector<int> speedList;
    GetValidTrackDirectories(mediaDir, speedList);

    int trackSpeed = GetBestTrack(speed, speedList);
    if (trackSpeed > 0) {
        std::stringstream dirStream;
        dirStream << "ff_" << trackSpeed;
        trackDir = dirStream.str();
        return true;
    }

    return false;
}

void CFFUtils::GetValidTrackDirectories(std::string mediaDir, std::vector<int> & speedList)
{
    std::vector<std::string>  directoryList;
    int itemCount = GetDirectoryListing(mediaDir, directoryList, true);
    if (itemCount <= 0)
        return;

    for (int idx = 0; idx < itemCount; idx++) {
        // Does the directory name have the expected format (ff_XXX)?
        int trackSpeed = 0;
        int rc = sscanf(directoryList[idx].c_str(), "ff_%d",  &trackSpeed);
        if (rc != 1)
            continue;

        char strbuf[32];
        sprintf(strbuf, "ff_%d", trackSpeed);
        if (strcmp(strbuf, directoryList[idx].c_str()) != 0)
            continue;

        // Check for track speed limits
        if (trackSpeed <= 1 || trackSpeed > 512)
            continue;

        // Found a good one
        speedList.push_back(trackSpeed);

    }
}

void CFFUtils::DeleteAllTrackDirectories(std::string mediaDir)
{
    std::vector<std::string>  directoryList;
    int itemCount = GetDirectoryListing(mediaDir, directoryList, true);
    if (itemCount <= 0)
        return;

    for (int idx = 0; idx < itemCount; idx++) {
        // Does the directory name have the expected format (ff_XXX)?
        int trackSpeed = 0;
        int rc = sscanf(directoryList[idx].c_str(), "ff_%d",  &trackSpeed);
        if (rc != 1)
            continue;

        char strbuf[32];
        sprintf(strbuf, "ff_%d", trackSpeed);
        if (strcmp(strbuf, directoryList[idx].c_str()) != 0)
            continue;

        // Looks like a track directory, so delete it
        std::string trackDirectory = mediaDir + "/" + directoryList[idx];
        DeleteDirectoryTree(trackDirectory);
    }
}

void CFFUtils::DeleteThumbnailDirectory(std::string mediaDir)
{
    std::string tnDirectory = mediaDir + "/tn";
    DeleteDirectoryTree(tnDirectory);
}

int CFFUtils::GetBestTrack(double speed, std::vector<int> & speedList)
{
    int lastTrackSpeed = 0;
    for (int i = 0; i < (int)speedList.size(); i++) {
        int trackSpeed = speedList[i];
        if (speed > trackSpeed)
            continue;
        if (lastTrackSpeed == 0 || lastTrackSpeed > trackSpeed) {
            lastTrackSpeed = trackSpeed;
        }
    }
    return lastTrackSpeed;
}

bool CFFUtils::DeleteDirectoryTree(std::string path) {
    std::vector<std::string>  directoryList;
    int itemCount = GetDirectoryListing(path, directoryList);
    if (itemCount < 0)
        return false;

    bool rc = true;
    for (int idx = 0; idx < itemCount; idx++) {
        if (IsDirectory(directoryList[idx])) {
            rc = DeleteDirectoryTree(directoryList[idx]);
        }
        else {
            rc = (unlink(directoryList[idx].c_str()) != -1);
        }
        if (!rc)
            break;
    }

    if (rmdir(path.c_str()) == -1)
        return false;
    return rc;
}

int CFFUtils::GetDirectoryListing(std::string path,
        std::vector<std::string> & directoryList, bool onlyDirNames) {
#ifndef WINDOWS
    DIR * dp;
    struct dirent * ep;
    std::string pathBuf;

    dp = opendir(path.c_str());
    if (dp == NULL)
        return -1;

    int count = 0;
    while ((ep = readdir(dp)) != NULL) {
        if (strcmp(ep->d_name, ".") == 0)
            continue;
        if (strcmp(ep->d_name, "..") == 0)
            continue;
        pathBuf = path + "/" + ep->d_name;
        if (onlyDirNames) {
            if (!IsDirectory(pathBuf))
                continue;
            pathBuf = ep->d_name; // overwrite with just the base name
            directoryList.push_back(pathBuf);
        }
        else {
            directoryList.push_back(pathBuf);
        }
        count++;
    }

    closedir(dp);
    return count;
#else
    return 0;
#endif
}

bool CFFUtils::IsDirectory(std::string path) {
#ifndef WINDOWS
    struct stat buf;
    if (stat(path.c_str(), &buf) == -1) {
        return false;
    }
    if (!S_ISDIR(buf.st_mode)) {
        return false;
    }
    return true;
#else
    return false;
#endif
}

bool CFFUtils::CreateDirectory(std::string directoryPath) {
#ifndef WINDOWS
    if (mkdir(directoryPath.c_str(), 00777) == -1) {
        return false;
    }
    return true;
#else
    return false;
#endif
}

bool CFFUtils::ParseSpeed(const char * speed, double & speedValue) {
    if (strchr(speed, '/') != NULL) {
        int numerator = 0, denominator = 0;
        if (sscanf(speed, "%d/%d", &numerator, &denominator) != 2)
            return false;
        if (numerator <= 0 || denominator <= 0 ) {
            return false;
        }
        speedValue = (1.0 * numerator)/denominator;
    }
    else {
        // We have been passed a decimal float value.
        errno = 0;
        double val = strtod(speed, NULL);
        if (errno != 0)
            return false;
        speedValue = val;
    }
    return true;
}

bool CFFUtils::ParseTrackList(std::string input, std::vector<int> & speedList)
{
    speedList.clear();

    if (input.length() == 0)
        return false;

    std::string::size_type pos = 0;
    while (true) {
        std::string::size_type commapos = input.find(',', pos);
        if (commapos == std::string::npos) {
            if (!ValidateAndAddSpeed(input.substr(pos), speedList))
                return false;
            break;
        }

        std::string item = input.substr(pos, commapos - pos);
        if (!ValidateAndAddSpeed(item, speedList))
            return false;
        pos += (commapos - pos) + 1;
        if (pos == input.length())
            break;
    }

    return true;
}

bool CFFUtils::ValidateAndAddSpeed(std::string speedItem, std::vector<int> & speedList)
{
    if (speedItem.size() == 0)
        return false;
    if (speedItem.find_first_not_of("0123456789") != std::string::npos)
        return false;

    int value = atoi(speedItem.c_str());

    // Better be more than 1
    if (value <= 1)
        return false;

    // Better be more than the last value added
    if (speedList.size() > 0 && value <= speedList.back())
        return false;

    speedList.push_back(value);

    return true;
}
