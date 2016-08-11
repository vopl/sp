#ifndef _NULLLOCK_H_
#define _NULLLOCK_H_


extern volatile int anyThreadCreated;

namespace HL {

class NullLockType {
public:
  
  NullLockType (void)
  {}
  
  ~NullLockType (void)
  {}

  inline void lock (void) {}
  inline void unlock (void) {}


};

}

#endif // _NULLLOCK_H_
