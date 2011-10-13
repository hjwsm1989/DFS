/* I want to beacome a hacker!
 * At first, i should try to write as much as possible codes.
 * Many times, i think i should insist on this job!
 * email:hjwsm1989@gmail.com
 */

// From ceph

#ifndef BSDOCFS_MUTEX_H
#define BSDOCFS_MUTEX_H

#include <pthread.h>
#include "include/assert.h"
#include "lockdep.h"

#define LOCKDEP

using namespace bsdocfs;

class Mutex {
private:
  const char *name;
  int id;
  bool recursive;
  bool lockdep;
  bool backtrace;  // gather backtrace on lock acquisition

  pthread_mutex_t _m;
  int nlock;

  // don't allow copying.
  void operator=(Mutex &M) {}
  Mutex( const Mutex &M ) {}

#ifdef LOCKDEP
  void _register() {
    id = lockdep_register(name);
  }
  void _will_lock() { // about to lock
    id = lockdep_will_lock(name, id);
  }
  void _locked() {    // just locked
    id = lockdep_locked(name, id, backtrace);
  }
  void _will_unlock() {  // about to unlock
    id = lockdep_will_unlock(name, id);
  }
#else
  void _register() {}
  void _will_lock() {} // about to lock
  void _locked() {}    // just locked
  void _will_unlock() {}  // about to unlock
#endif

public:
  Mutex(const char *n, bool r = false, bool ld=true, bool bt=false) :
    name(n), id(-1), recursive(r), lockdep(ld), backtrace(bt), nlock(0) {
    if (recursive) {
      // Mutexes of type PTHREAD_MUTEX_RECURSIVE do all the same checks as
      // mutexes of type PTHREAD_MUTEX_ERRORCHECK.
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
      pthread_mutex_init(&_m,&attr);
      pthread_mutexattr_destroy(&attr);
      if (g_lockdep)
		_register();
    }
    else if (lockdep) {
      // If the mutex type is PTHREAD_MUTEX_ERRORCHECK, then error checking
      // shall be provided. If a thread attempts to relock a mutex that it
      // has already locked, an error shall be returned. If a thread
      // attempts to unlock a mutex that it has not locked or a mutex which
      // is unlocked, an error shall be returned.
      pthread_mutexattr_t attr;
      pthread_mutexattr_init(&attr);
      pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
      pthread_mutex_init(&_m, &attr);
      if (g_lockdep)
		_register();
    }
    else {
      // If the mutex type is PTHREAD_MUTEX_NORMAL, deadlock detection
      // shall not be provided. Attempting to relock the mutex causes
      // deadlock. If a thread attempts to unlock a mutex that  it  has not
      // locked or a mutex which is unlocked, undefined behavior results.
      pthread_mutex_init(&_m, NULL);
    }
  }
  ~Mutex() {
    assert(nlock == 0);
    pthread_mutex_destroy(&_m); 
  }

  bool is_locked() {
    return (nlock > 0);
  }

  bool TryLock() {
    int r = pthread_mutex_trylock(&_m);
    if (r == 0) {
      if (lockdep && g_lockdep) _locked();
      nlock++;
    }
    return r == 0;
  }

  void Lock(bool no_lockdep=false) {
    if (lockdep && g_lockdep && !no_lockdep) _will_lock();
    int r = pthread_mutex_lock(&_m);
    if (lockdep && g_lockdep) _locked();
    assert(r == 0);
    nlock++;
  }

  void Unlock() {
    assert(nlock > 0);
    --nlock;
    if (lockdep && g_lockdep) _will_unlock();
    int r = pthread_mutex_unlock(&_m);
    assert(r == 0);
  }

  friend class Cond;


public:
  class Locker {
    Mutex &mutex;

  public:
    Locker(Mutex& m) : mutex(m) {
      mutex.Lock();
    }
    ~Locker() {
      mutex.Unlock();
    }
  };
};


#endif
