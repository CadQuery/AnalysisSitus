#pragma once

//-----------------------------------------------------------------------------

// OCCT includes
#include <Standard.hxx>

#define EXPORT_MACRO Standard_EXPORT

//-----------------------------------------------------------------------------

#undef MEMTRACKER
#if defined MEMTRACKER
#pragma message("===== warning: MEMTRACKER is enabled")

#ifdef _WIN32

// Standard includes
#include <vector>

//! A single checkpoint for memory tracking.
struct t_memCheckPoint
{
  std::string name; //!< Domain-specific name of a checkpoint (kept for future).
  int         mem;  //!< Memory consumed by the process at a checkpoint.

  t_memCheckPoint() : mem(0) {} //!< Default ctor.

  //! Ctor with initialization.
  t_memCheckPoint(const std::string& _name,
                  const int          _mem)
  : name(_name), mem(_mem) {}
};

//! Singleton tracker.
class IMemTracker
{
public:
  virtual void AddCheckPoint        ()                            = 0;
  virtual void GetCheckPoint        (const int, t_memCheckPoint&) = 0;
  virtual int  GetNumCheckPoints    () const                      = 0;
  virtual int  GetProcessMiB        ()                            = 0;
};

EXPORT_MACRO IMemTracker& GetTracker();

//-----------------------------------------------------------------------------

#define MEMCHECK \
{ \
  IMemTracker& TRACKER = GetTracker(); \
  TRACKER.AddCheckPoint(); \
}

#define MEMCHECK_DUMP(filename) \
{ \
  IMemTracker& TRACKER = GetTracker(); \
  FILE* __pFile = fopen(filename, "w"); \
  for ( int __k = 0; __k < TRACKER.GetNumCheckPoints(); ++__k ) \
  { \
    t_memCheckPoint __cp; \
    TRACKER.GetCheckPoint(__k, __cp); \
    fprintf(__pFile, " %d %d", __k, __cp.mem); \
  } \
  fclose(__pFile); \
}

#else // _WIN32
  #define MEMCHECK_COUNT_MIB(varname)
#endif

#else

#define MEMCHECK
#define MEMCHECK_DUMP(filename)

#endif // MEMTRACKER
