#include <MemTracker.h>

#if defined MEMTRACKER

// OS-dependent includes
#include "windows.h"
#include "psapi.h"

//! Default implementation of the tracker interface.
class MemTracker : public IMemTracker
{
public:

  int                          currentCheckPoint; //!< Current CP.
  std::vector<t_memCheckPoint> checkPoints;       //!< All collected CPs.

  virtual void SetCurrentCheckPoint(const int id)
  {
    this->currentCheckPoint = id;
    while ( this->checkPoints.size() <= this->currentCheckPoint )
      this->checkPoints.push_back( t_memCheckPoint() );
  }

  virtual int GetCurrentCheckPoint() const
  {
    return this->currentCheckPoint;
  }

  virtual void AddCheckPoint(const t_memCheckPoint& info)
  {
    this->checkPoints.push_back(info);
  }

  virtual void GetCheckPoint(const int id, t_memCheckPoint& info)
  {
    info = this->checkPoints[id];
  }

  virtual int GetNumCheckPoints() const
  {
    return int( this->checkPoints.size() );
  }

  virtual void SetCurrentMem(const int mem)
  {
    this->checkPoints[this->currentCheckPoint].mem = mem;
  }

  virtual void AddCurrentMem(const int mem)
  {
    this->checkPoints[this->currentCheckPoint].mem += mem;
  }

  virtual int GetProcessMiB() const
  {
    PROCESS_MEMORY_COUNTERS PMC;
    GetProcessMemoryInfo( GetCurrentProcess(), &PMC, sizeof(PMC) );
    SIZE_T physUsedBytes = PMC.WorkingSetSize;
    return (int) ( physUsedBytes / (1024 * 1024) );
  }
};

IMemTracker& GetTracker()
{
  static MemTracker tracker;
  return tracker;
}

#endif
