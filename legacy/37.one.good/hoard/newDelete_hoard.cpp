//#include "msoi/utils/msoiAllocator.h"

#ifndef CUSTOM_DLLNAME
#define CUSTOM_DLLNAME hoardDllMain
#endif

#include <new>

// The undef below ensures that any pthread_* calls get strong
// linkage.  Otherwise, our versions here won't replace them.  It is
// IMPERATIVE that this line appear before any files get included.

#undef __GXX_WEAK__ 

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN

#pragma warning(disable:4355)

// Maximize the degree of inlining.
#pragma inline_depth(255)

// Turn inlining hints into requirements.
#define inline __forceinline
#pragma warning(disable:4273)
#endif

#if HOARD_NO_LOCK_OPT
// Disable lock optimization.
volatile int anyThreadCreated = 1;
#else
// The normal case. See heaplayers/spinlock.h.
volatile int anyThreadCreated = 0;
#endif

namespace Hoard {

	/// The maximum amount of memory that each TLAB may hold, in bytes.
	enum { MAX_MEMORY_PER_TLAB = 256 * 1024 };

	/// The maximum number of threads supported (sort of).
	enum { MaxThreads = 64 };

	/// The maximum number of heaps supported.
	enum { NumHeaps = 8 };

	/// Size, in bytes, of the largest object we will cache on a
	/// thread-local allocation buffer.
	enum { LargestSmallObject = 256 };

	// HOARD_MMAP_PROTECTION_MASK defines the protection flags used for
	// freshly-allocated memory. The default case is that heap memory is
	// NOT executable, thus preventing the class of attacks that inject
	// executable code on the heap.
	// 
	// While this is not recommended, you can define HL_EXECUTABLE_HEAP as
	// 1 in heaplayers/hldefines.h if you really need to (i.e., you're
	// doing dynamic code generation into malloc'd space).

#if HL_EXECUTABLE_HEAP
#define HOARD_MMAP_PROTECTION_MASK (PROT_READ | PROT_WRITE | PROT_EXEC)
#else
#define HOARD_MMAP_PROTECTION_MASK (PROT_READ | PROT_WRITE)
#endif

}

#include "ansiwrapper.h"
#include "cpuinfo.h"
#include "hoard.h"
#include "heapmanager.h"
#include "tlab.h"

//
// The base Hoard heap.
//

namespace Hoard {

	class HoardHeapType :
		public HeapManager<TheLockType, HoardHeap<MaxThreads, NumHeaps> > {
	};

	// Just an abbreviation.
	typedef HoardHeapType::SuperblockType::Header TheHeader;

	//
	// The thread-local 'allocation buffers' (TLABs), which is a bit of a
	// misnomer since these are actually separate heaps in their own
	// right.
	//

	typedef ThreadLocalAllocationBuffer<HL::bins<TheHeader, SUPERBLOCK_SIZE>::NUM_BINS,
		HL::bins<TheHeader, SUPERBLOCK_SIZE>::getSizeClass,
		HL::bins<TheHeader, SUPERBLOCK_SIZE>::getClassSize,
		LargestSmallObject,
		MAX_MEMORY_PER_TLAB,
		HoardHeapType::SuperblockType,
		SUPERBLOCK_SIZE,
		HoardHeapType> TLABBase;

	class TLAB : public TLABBase {
	public:
		TLAB (HoardHeapType * h)
			: TLABBase (h)
		{}

	};

}

using namespace Hoard;

typedef TLAB TheCustomHeapType;

/// Maintain a single instance of the main Hoard heap.

inline static HoardHeapType * getMainHoardHeap (void) {
	// This function is C++ magic that ensures that the heap is
	// initialized before its first use. First, allocate a static buffer
	// to hold the heap.
	static double thBuf[sizeof(HoardHeapType) / sizeof(double) + 1];

	// Now initialize the heap into that buffer.
	static HoardHeapType * th = new (thBuf) HoardHeapType;
	return th;
}

// Compute the version of gcc we're compiling with (if any).
#define GCC_VERSION (__GNUC__ * 10000 \
	+ __GNUC_MINOR__ * 100 \
	+ __GNUC_PATCHLEVEL__)

#include "userealtls.cpp"


//////////////////////////////////////////////////////////////////////////
void operator delete[](void *p) throw()
{
	TheCustomHeapType * theCustomHeap = getCustomHeap();
	theCustomHeap->free (p);
}

//////////////////////////////////////////////////////////////////////////
void operator delete(void *p) throw()
{
	TheCustomHeapType * theCustomHeap = getCustomHeap();
	theCustomHeap->free (p);
}

//////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
void *operator new[](size_t s) throw(...)
#else
void *operator new[](std::size_t s) throw(std::bad_alloc)
#endif
{
	TheCustomHeapType * theCustomHeap = getCustomHeap();
	void * p = theCustomHeap->malloc (s);
	return p;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
void *operator new(size_t s) throw(...)
#else
void *operator new(std::size_t s) throw(std::bad_alloc)
#endif
{
	TheCustomHeapType * theCustomHeap = getCustomHeap();
	void * p = theCustomHeap->malloc (s);
	return p;
}
