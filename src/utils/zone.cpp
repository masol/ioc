////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2012-2013 by authors (see AUTHORS.txt)                  //
//                                                                        //
//  This file is part of IOC.                                             //
//                                                                        //
//  IOC is free software; you can redistribute it and//or modify it under //
//  the terms of the IOC Public License(LGPL License) as published by     //
//  masol.lee.                                                            //
//                                                                        //
//  IOC is distributed in the hope that it will be useful,but WITHOUT     //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY    //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                  //
//                                                                        //
//  You should have received a copy of the IOC License along with this    //
//  program.  If not, see <http://www.masols.com/licenses>.               //
//                                                                        //
//  IOC website: http://www.masols.com                                    //
////////////////////////////////////////////////////////////////////////////

#include "utils/macro.h"
#include "utils/zone.h"
#include <boost/thread/tss.hpp>
#include <stdlib.h>


namespace ioc {
/// @brief Segments代表了每个从系统请求的内存块。每个块拥有开始地址（开始地址被编码在this指针中）、字节长度。
/// 全部Segments被以LIFO结构链在一起。最新创建的内存块可以通过Segment::head()获取。每个segment使用malloc来分配，使用free来释放.

class Segment {
public:
	Segment* next() const {
		return next_;
	}
	void clear_next() {
		next_ = NULL;
	}
	int size() const {
		return size_;
	}
	int capacity() const {
		return size_ - sizeof(Segment);
	}
	Address start() const {
		return address(sizeof(Segment));
	}
	Address end() const {
		return address(size_);
	}
	static Segment* head() {
		return head_;
	}
	static void set_head(Segment* head) {
		head_ = head;
	}
	// Creates a new segment, sets it size, and pushes it to the front
	// of the segment chain. Returns the new segment.
	static Segment* New(Zone &zone,int size) {
		Segment* result = reinterpret_cast<Segment*>(malloc(size));
		zone.adjust_segment_bytes_allocated(size);
		if (result != NULL) {
			result->next_ = head_;
			result->size_ = size;
			head_ = result;
		}
		return result;
	}
	// Deletes the given segment. Does not touch the segment chain.
	static void Delete(Zone& zone,Segment* segment, int size) {
		zone.adjust_segment_bytes_allocated(-size);
		free(segment);
	}
	static int bytes_allocated() {
		return bytes_allocated_;
	}
private:
	// Computes the address of the nth byte in this segment.
	Address address(int n) const {
		return Address(this) + n;
	}
	static Segment* head_;
	static int bytes_allocated_;
	Segment* next_;
	int size_;
};

Segment* Segment::head_ = NULL;
int Segment::bytes_allocated_ = 0;

bool
Zone::isInZone(void* ptr)
{
	bool bInZone = false;
	Segment* current = Segment::head();
	while (current != NULL) {
		if( reinterpret_cast<byte*>(ptr) > reinterpret_cast<byte*>(current) && reinterpret_cast<byte*>(ptr) < (reinterpret_cast<byte*>(current)  + current->size()) )
		{
			bInZone = true;
			break;
		}
		current = current->next();
	}
	return bInZone;
}


void Zone::DeleteAll() {
#ifdef _DEBUG
	// Constant byte value used for zapping dead memory in debug mode.
	static const unsigned char kZapDeadByte = 0xcd;
#endif
	// Find a segment with a suitable size to keep around.
	Segment* keep = Segment::head();
	while (keep != NULL && keep->size() > kMaximumKeptSegmentSize) {
		keep = keep->next();
	}
	// Traverse the chained list of segments, zapping (in debug mode)
	// and freeing every segment except the one we wish to keep.
	Segment* current = Segment::head();
	while (current != NULL) {
		Segment* next = current->next();
		if (current == keep) {
			// Unlink the segment we wish to keep from the list.
			current->clear_next();
		} else {
			int size = current->size();
#ifdef _DEBUG
			// Zap the entire current segment (including the header).
			memset(current, kZapDeadByte, size);
#endif
			Segment::Delete(*this,current, size);
		}
		current = next;
	}
	// If we have found a segment we want to keep, we must recompute the
	// variables 'position' and 'limit' to prepare for future allocate
	// attempts. Otherwise, we must clear the position and limit to
	// force a new segment to be allocated on demand.
	if (keep != NULL) {
		Address start = keep->start();
		position_ = RoundUp(start, kAlignment);
		limit_ = keep->end();
#ifdef DEBUG
		// Zap the contents of the kept segment (but not the header).
		memset(start, kZapDeadByte, keep->capacity());
#endif
	} else {
		position_ = limit_ = 0;
	}
	// Update the head segment to be the kept segment (if any).
	Segment::set_head(keep);
}

Address Zone::NewExpand(int size) {
	// Make sure the requested size is already properly aligned and that
	// there isn't enough room in the Zone to satisfy the request.
	IOC_ASSERT(size == RoundDown(size, kAlignment));
	IOC_ASSERT(position_ + size > limit_);
	// Compute the new segment size. We use a 'high water mark'
	// strategy, where we increase the segment size every time we expand
	// except that we employ a maximum segment size when we delete. This
	// is to avoid excessive malloc() and free() overhead.
	Segment* head = Segment::head();
	int old_size = (head == NULL) ? 0 : head->size();
	static const int kSegmentOverhead = sizeof(Segment) + kAlignment;
	int new_size = kSegmentOverhead + size + (old_size << 1);
	if (new_size < kMinimumSegmentSize) {
		new_size = kMinimumSegmentSize;
	} else if (new_size > kMaximumSegmentSize) {
		// Limit the size of new segments to avoid growing the segment size
		// exponentially, thus putting pressure on contiguous virtual address space.
		// All the while making sure to allocate a segment large enough to hold the
		// requested size.
		new_size = Max(kSegmentOverhead + size, kMaximumSegmentSize);
	}
	Segment* segment = Segment::New(*this,new_size);
	if (segment == NULL) {
		IOC_FATAL("OUT OF MEMORY");
		return NULL;
	}
	// Recompute 'top' and 'limit' based on the new segment.
	Address result = RoundUp(segment->start(), kAlignment);
	position_ = result + size;
	limit_ = segment->end();
	IOC_ASSERT(position_ <= limit_);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ZoneStorage implement.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static	void	freeStorageObject(ZoneStorage* object)
{
	//this will not happen.
	IOC_UNREACHABLE();
	//delete object;
}

#ifdef IOC_NOTHREAD_SAFE
	ZoneStorage&
	ZoneStorage::getStorage(void) {
		static	ZoneStorage*									lv_Storage;
		if(!lv_Storage) {
			lv_Storage = new ZoneStorage();
		}
		return *lv_Storage;
	}
#else
	static inline boost::thread_specific_ptr<ZoneStorage>&		get_lv_tlsStorage(void) {
		static	boost::thread_specific_ptr<ZoneStorage>*		lv_tlsStorage;
		if(lv_tlsStorage == NULL) {
			lv_tlsStorage = new boost::thread_specific_ptr<ZoneStorage>(freeStorageObject);
		}
		return *lv_tlsStorage;
	}
	ZoneStorage&
	ZoneStorage::getStorage(void) {
		static	ZoneStorage*									lv_Storage;
		boost::thread_specific_ptr<ZoneStorage>& tls = get_lv_tlsStorage();
		if(!tls.get())
			tls.reset(new ZoneStorage());
		return *tls.get();
	}
#endif

} //end namespace ioc.
