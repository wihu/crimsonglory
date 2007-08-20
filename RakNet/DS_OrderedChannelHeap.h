/// \file
/// \brief \b [Internal] Ordered Channel Heap .  This is a heap where you add to it on multiple ordered channels, with each channel having a different weight.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __RAKNET_ORDERED_CHANNEL_HEAP_H
#define __RAKNET_ORDERED_CHANNEL_HEAP_H

#include "DS_Heap.h"
#include "DS_Map.h"
#include "DS_Queue.h"
#include "Export.h"
#include <assert.h>
#include "Rand.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)=defaultMapKeyComparison<channel_key_type> >
	class RAK_DLL_EXPORT OrderedChannelHeap
	{
	public:
		static void IMPLEMENT_DEFAULT_COMPARISON(void) {DataStructures::defaultMapKeyComparison<channel_key_type>(channel_key_type(),channel_key_type());}

		OrderedChannelHeap();
		~OrderedChannelHeap();
		void Push(const channel_key_type &channelID, const heap_data_type &data);
		void PushAtHead(const unsigned index, const channel_key_type &channelID, const heap_data_type &data);
		heap_data_type Pop(const unsigned startingIndex=0);
		heap_data_type Peek(const unsigned startingIndex) const;
		void AddChannel(const channel_key_type &channelID, const float weight);
		void RemoveChannel(channel_key_type channelID);
		void Clear(void);
		heap_data_type& operator[] ( const unsigned int position ) const;
		unsigned ChannelSize(const channel_key_type &channelID);
		unsigned Size(void) const;

		struct QueueAndWeight
		{
			DataStructures::Queue<float> randResultQueue;
			float weight;
		};

		struct HeapChannelAndData
		{
			HeapChannelAndData() {}
			HeapChannelAndData(const channel_key_type &_channel, const heap_data_type &_data) : data(_data), channel(_channel) {}
			heap_data_type data;
			channel_key_type channel;
		};

	protected:
		DataStructures::Map<channel_key_type, QueueAndWeight*, channel_key_comparison_func> map;
		DataStructures::Heap<float, HeapChannelAndData, true> heap;
	};

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
		OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::OrderedChannelHeap()
	{
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
		OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::~OrderedChannelHeap()
	{
		Clear();
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
	void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Push(const channel_key_type &channelID, const heap_data_type &data)
	{
		PushAtHead(MAX_UNSIGNED_LONG, channelID, data);
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
	void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::PushAtHead(const unsigned index, const channel_key_type &channelID, const heap_data_type &data)
	{
		// If an assert hits here then this is an unknown channel.  Call AddChannel first.
		QueueAndWeight *queueAndWeight=map.Get(channelID);
		float maxRange, minRange, rnd;
		if (queueAndWeight->randResultQueue.Size()==0)
		{
			maxRange=1.0f;
			minRange=0.0f;
		}
		else if (index >= queueAndWeight->randResultQueue.Size())
		{
			maxRange=queueAndWeight->randResultQueue[queueAndWeight->randResultQueue.Size()-1]*.99999f;
			minRange=0.0f;
		}
		else
		{
			if (index==0)
				maxRange=1.0f;
			else if (index >= queueAndWeight->randResultQueue.Size())
				maxRange=queueAndWeight->randResultQueue[queueAndWeight->randResultQueue.Size()-1]*.99999f;
			else
				maxRange=queueAndWeight->randResultQueue[index-1]*.99999f;

			minRange=maxRange=queueAndWeight->randResultQueue[index]*1.00001f;
		}
		
#ifdef _DEBUG
		assert(maxRange!=0.0f);
#endif
		rnd=frandomMT() * (maxRange - minRange);
		if (rnd==0.0f)
			rnd=maxRange/2.0f;

		if (index >= queueAndWeight->randResultQueue.Size())
			queueAndWeight->randResultQueue.Push(rnd);
		else
			queueAndWeight->randResultQueue.PushAtHead(rnd, index);

		heap.Push(rnd*queueAndWeight->weight, HeapChannelAndData(channelID, data));
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
	heap_data_type OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Pop(const unsigned startingIndex)
	{
		assert(startingIndex < heap.Size());

		QueueAndWeight *queueAndWeight=map.Get(heap[startingIndex].channel);
		if (startingIndex!=0)
		{
			// Ugly - have to count in the heap how many nodes have the same channel, so we know where to delete from in the queue
			unsigned indiceCount=0;
			unsigned i;
			for (i=0; i < startingIndex; i++)
				if (channel_key_comparison_func(heap[i].channel,heap[startingIndex].channel)==0)
					indiceCount++;
			queueAndWeight->randResultQueue.Del(indiceCount);
		}
		else
		{
			queueAndWeight->randResultQueue.Pop();
		}
		return heap.Pop(startingIndex).data;
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
	heap_data_type OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Peek(const unsigned startingIndex) const
	{
		HeapChannelAndData heapChannelAndData = heap.Peek(startingIndex);
		return heapChannelAndData.data;
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
	void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::AddChannel(const channel_key_type &channelID, const float weight)
	{
		QueueAndWeight *qaw = new QueueAndWeight;
		qaw->weight=weight;
		map.SetNew(channelID, qaw);
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
		void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::RemoveChannel(channel_key_type channelID)
	{
		if (map.Has(channelID))
		{
			unsigned i;
			i=map.GetIndexAtKey(channelID);
			delete map[i];
			map.RemoveAtIndex(i);
		}
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
		unsigned OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Size(void) const
	{
		return heap.Size();
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
		heap_data_type& OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::operator[]( const unsigned int position ) const
	{
		return heap[position].data;
	}


	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
		unsigned OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::ChannelSize(const channel_key_type &channelID)
	{
		QueueAndWeight *queueAndWeight=map.Get(channelID);
		return queueAndWeight->randResultQueue.Size();
	}

	template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
		void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Clear(void)
	{
		unsigned i;
		for (i=0; i < map.Size(); i++)
			delete map[i];
		map.Clear();
		heap.Clear();
	}
}

#endif
