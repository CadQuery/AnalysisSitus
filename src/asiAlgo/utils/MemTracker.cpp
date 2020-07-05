#include <MemTracker.h>

#if defined MEMTRACKER

// OCCT includes
#include <Standard_Mutex.hxx>

// OS-dependent includes
#include "windows.h"
#include "psapi.h"

//! Default implementation of the tracker interface.
class MemTracker : public IMemTracker
{
public:

  std::vector<t_memCheckPoint> checkPoints; //!< All collected CPs.
  Standard_Mutex               MUTEX;

  virtual void AddCheckPoint()
  {
    MUTEX.Lock();
    this->checkPoints.push_back( t_memCheckPoint( "", this->GetProcessMiB() ) );
    MUTEX.Unlock();
  }

  virtual void GetCheckPoint(const int id, t_memCheckPoint& info)
  {
    info = this->checkPoints[id];
  }

  virtual int GetNumCheckPoints() const
  {
    return int( this->checkPoints.size() );
  }

  virtual int GetProcessMiB()
  {
    MUTEX.Lock();
    PROCESS_MEMORY_COUNTERS PMC;
    GetProcessMemoryInfo( GetCurrentProcess(), &PMC, sizeof(PMC) );
    SIZE_T physUsedBytes = PMC.WorkingSetSize;
    const int MiB = (int) ( physUsedBytes / (1024 * 1024) );
    MUTEX.Unlock();
    return MiB;
  }
};

IMemTracker& GetTracker()
{
  static MemTracker tracker;
  return tracker;
}

#endif
