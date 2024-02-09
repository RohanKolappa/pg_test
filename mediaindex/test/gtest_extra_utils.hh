/*
 * gtest_extra_utils.hh
 *
 *  Created on: Nov 9, 2012
 *      Author: rkale
 */

#ifndef GTEST_EXTRA_UTILS_HH_
#define GTEST_EXTRA_UTILS_HH_

#include <string>

namespace GTestUtils {

    inline bool SkipCleanup() {
         const char *env_value = getenv("SKIPCLEANUP");
         if ((env_value != NULL) && (strcmp(env_value,"1") == 0))
             return true;
         return false;
     }

    inline void ConditionalDeleteDirectory(std::string a_name) {
        if (!GTestUtils::SkipCleanup())
            GTestUtils::DeleteDirectory(a_name);
    }

}


#endif /* GTEST_EXTRA_UTILS_HH_ */
