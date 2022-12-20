#pragma once

#include "Typedefs.hpp"
#include "Enums.hpp"
#include "Constexpr.hpp"
#include "Macros.hpp"

extern DLL st6_malloc_t st6_malloc;
extern DLL st6_free_t st6_free;
#define ST6_ALLOCATION_DEFINED

#include "FLCore/FLCoreCommon.h"
#include "FLCore/FLCoreServer.h"
#include "FLCore/FLCoreRemoteClient.h"
#include "FLCore/FLCoreDALib.h"

#include <ext/Singleton.h>

#include "Structs.hpp"

#include "Deps.hpp"
struct CARGO_INFO;

#ifdef SERVER
#include "plugin.h"
#include "Server.hpp"
#endif

#ifdef CLIENT
#include "Client.hpp"
#endif