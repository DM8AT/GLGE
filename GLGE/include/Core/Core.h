/**
 * @file Core.h
 * @author DM8AT
 * @brief a utility file to include the whole library core
 * @version 0.1
 * @date 2025-12-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_
#define _GLGE_CORE_

//include the settings
#include "Settings.h"
//include the common stuff
#include "Common.h"
//include the type info functions
#include "TypeInfo.h"
//add exceptions
#include "Exception.h"
//add events
#include "Event.h"
//add event busses
#include "EventBus.h"
//add rate limiters
#include "RateLimiter.h"
//include profiling
#include "Profiler.h"
//include references
#include "Reference.h"

//include ordered maps
#include "utils/OrderedMap.h"
//add optionals
#include "utils/Optional.h"
//add task queues
#include "utils/TaskQueue.h"

//add instances
#include "Instance.h"
//add base classes
#include "BaseClass.h"
//add the asset system
#include "AssetManager.h"
//add objects and worlds
#include "Object.h"

//add transforms
#include "Transform.h"

#endif