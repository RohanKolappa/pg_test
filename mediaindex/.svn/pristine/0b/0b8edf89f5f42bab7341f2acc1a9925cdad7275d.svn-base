/*
 * SystemUtils.cpp
 *
 *  Created on: Mar 16, 2010
 *      Author: rkale
 */
#include "CommonIncludes.hh"

#include "SystemUtils.hh"
#include <string.h>
#if defined WINDOWS
#include <Rpc.h>
#else
#ifdef USE_OSSP_UUID
#include "uuid.h"
#else
#include "uuid/uuid.h"
#endif
#endif

namespace mediaindex {

void GenerateUUID(uint8_t *pDestination)
{

#if defined WINDOWS
    UuidCreate(reinterpret_cast<UUID*>(pDestination));
#else
#ifdef USE_OSSP_UUID
    uuid_t *uuid;
    uuid_create(&uuid);
    uuid_make(uuid, UUID_MAKE_V4);
    uuid_export(uuid, UUID_FMT_STR, (void **)&pDestination, NULL);
    uuid_destroy(uuid);
#else
    uuid_generate(pDestination);
#endif
#endif
}

uint64_t GenerateUID()
{

    // Simple shortcut: Generate a full UUID and use its first 8 bytes
    uint8_t uuid[16];
    GenerateUUID(uuid);

    uint64_t * pUuid =  (uint64_t *)uuid;
    uint64_t uid = *pUuid;

    return uid;
}

}
