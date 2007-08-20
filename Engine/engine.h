#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include "../App/stdafx.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using irr::u8;
using irr::u16;
using irr::u32;
using irr::c8;
using irr::s16;
using irr::s32;
using irr::f32;

using irr::core::array;
using irr::core::vector3df;
using irr::core::vector2df;
using irr::core::rect;
using irr::core::dimension2d;

using std::cout;
using std::cerr;
using std::endl;
using std::ostream;

//#define PROFILER

//foundation layer headers
// all engine headers except misc.h
#include "singleton.h"
#include "functor.h"
#include "ringbuf.h"
#include "serialize.h"
#include "kernel.h"
#include "interpolators.h"
#include "triggers.h"
#include "timers.h"
#include "log.h"
#include "mmanager.h"
#include "profiler.h"
#include "profileloghandler.h"
#include "render.h"
#include "ObjectFactory.h"
#include "fileCache.h"

#endif

