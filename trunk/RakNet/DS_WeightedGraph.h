/// \file
/// \brief \b [Internal] Weighted graph.  I'm assuming the indices are complex map types, rather than sequential numbers (which could be implemented much more efficiently).
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

#ifndef __WEIGHTED_GRAPH_H
#define __WEIGHTED_GRAPH_H

#include "DS_OrderedList.h"
#include "DS_Map.h"
#include "DS_Heap.h"
#include "DS_Queue.h"
#include "DS_Tree.h"
#include <assert.h>
#ifdef _DEBUG
#include <stdio.h>
#endif

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	class RAK_DLL_EXPORT WeightedGraph
	{
	public:
		static void IMPLEMENT_DEFAULT_COMPARISON(void) {DataStructures::defaultMapKeyComparison<node_type>(node_type(),node_type());}

		WeightedGraph();
		~WeightedGraph();
		WeightedGraph( const WeightedGraph& original_copy );
		WeightedGraph& operator= ( const WeightedGraph& original_copy );
		void AddNode(node_type node);
		void RemoveNode(node_type node);
		void AddConnection(node_type node1, node_type node2, weight_type weight);
		void RemoveConnection(node_type node1, node_type node2);
		bool HasConnection(node_type node1, node_type node2);
		void Print(void);
		void Clear(void);
		bool GetShortestPath(DataStructures::List<node_type> &path, node_type startNode, node_type endNode, weight_type INFINITE_WEIGHT);
		bool GetSpanningTree(DataStructures::Tree<node_type> &outTree, DataStructures::List<node_type> *inputNodes, node_type startNode, weight_type INFINITE_WEIGHT );
		unsigned GetNodeCount(void) const;
		unsigned GetConnectionCount(unsigned nodeIndex) const;
		void GetConnectionAtIndex(unsigned nodeIndex, unsigned connectionIndex, node_type &outNode, weight_type &outWeight) const;
		node_type GetNodeAtIndex(unsigned nodeIndex) const;

	protected:
		void ClearDijkstra(void);
		void GenerateDisjktraMatrix(node_type startNode, weight_type INFINITE_WEIGHT);

		DataStructures::Map<node_type, DataStructures::Map<node_type, weight_type> *> adjacencyLists;

		// All these variables are for path finding with Dijkstra
		struct  
		{
			bool isValid;
			node_type rootNode;
			DataStructures::OrderedList<node_type, node_type> costMatrixIndices;
			weight_type *costMatrix;
			node_type *leastNodeArray;
		} dijkstra;

		struct NodeAndParent
		{
			DataStructures::Tree<node_type>*node;
			DataStructures::Tree<node_type>*parent;
		};
	};

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::WeightedGraph()
	{
		dijkstra.isValid=false;
		dijkstra.costMatrix=0;
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::~WeightedGraph()
	{
		Clear();
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::WeightedGraph( const WeightedGraph& original_copy )
	{
		adjacencyLists=original_copy.adjacencyLists;
		
		dijkstra.isValid=original_copy.isValid;
		if (dijkstra.isValid)
		{
			dijkstra.rootNode=original_copy.rootNode;
			dijkstra.costMatrixIndices(original_copy.costMatrixIndices);
			dijkstra.costMatrix = new weight_type[dijkstra.costMatrixIndices.Size() * dijkstra.costMatrixIndices.Size()];
			dijkstra.leastNodeArray = new node_type[dijkstra.costMatrixIndices.Size()];
			memcpy(dijkstra.costMatrix, original_copy.costMatrix, dijkstra.costMatrixIndices.Size() * dijkstra.costMatrixIndices.Size() * sizeof(weight_type));
			memcpy(dijkstra.leastNodeArray, original_copy.leastNodeArray, dijkstra.costMatrixIndices.Size() * sizeof(weight_type));
		}
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	WeightedGraph<node_type, weight_type, allowUnlinkedNodes>& WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::operator=( const WeightedGraph& original_copy )
	{
		adjacencyLists=original_copy.adjacencyLists;

		dijkstra.isValid=original_copy.isValid;
		if (dijkstra.isValid)
		{
			dijkstra.rootNode=original_copy.rootNode;
			dijkstra.costMatrixIndices(original_copy.costMatrixIndices);
			dijkstra.costMatrix = new weight_type[dijkstra.costMatrixIndices.Size() * dijkstra.costMatrixIndices.Size()];
			dijkstra.leastNodeArray = new node_type[dijkstra.costMatrixIndices.Size()];
			memcpy(dijkstra.costMatrix, original_copy.costMatrix, dijkstra.costMatrixIndices.Size() * dijkstra.costMatrixIndices.Size() * sizeof(weight_type));
			memcpy(dijkstra.leastNodeArray, original_copy.leastNodeArray, dijkstra.costMatrixIndices.Size() * sizeof(weight_type));
		}

		return *this;
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::AddNode(node_type node)
	{
		adjacencyLists.SetNew(node, new DataStructures::Map<node_type, weight_type>);
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::RemoveNode(node_type node)
	{
		unsigned i;
		DataStructures::Queue<node_type> removeNodeQueue;

		removeNodeQueue.Push(node);
		while (removeNodeQueue.Size())
		{
			delete adjacencyLists.Pop(removeNodeQueue.Pop());

			// Remove this node from all of the other lists as well
			for (i=0; i < adjacencyLists.Size(); i++)
			{
				adjacencyLists[i]->Delete(node);

#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
				if (allowUnlinkedNodes==false && adjacencyLists[i]->Size()==0)
					removeNodeQueue.Push(adjacencyLists.GetKeyAtIndex(i));
			}
		}

		ClearDijkstra();
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	bool WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::HasConnection(node_type node1, node_type node2)
	{
		if (node1==node2)
			return false;
		if (adjacencyLists.Has(node1)==false)
			return false;
		return adjacencyLists.Get(node1)->Has(node2);
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::AddConnection(node_type node1, node_type node2, weight_type weight)
	{
		if (node1==node2)
			return;

		if (adjacencyLists.Has(node1)==false)
			AddNode(node1);
		adjacencyLists.Get(node1)->Set(node2, weight);
		if (adjacencyLists.Has(node2)==false)
			AddNode(node2);
		adjacencyLists.Get(node2)->Set(node1, weight);
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::RemoveConnection(node_type node1, node_type node2)
	{
		adjacencyLists.Get(node2)->Delete(node1);
		adjacencyLists.Get(node1)->Delete(node2);

#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
		if (allowUnlinkedNodes==false) // If we do not allow unlinked nodes, then if there are no connections, remove the node
		{
			if (adjacencyLists.Get(node1)->Size()==0)
				RemoveNode(node1); // Will also remove node1 from the adjacency list of node2
			if (adjacencyLists.Has(node2) && adjacencyLists.Get(node2)->Size()==0)
				RemoveNode(node2);
		}

		ClearDijkstra();
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::Clear(void)
	{
		unsigned i;
		for (i=0; i < adjacencyLists.Size(); i++)
			delete adjacencyLists[i];
		adjacencyLists.Clear();

		ClearDijkstra();
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		bool WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::GetShortestPath(DataStructures::List<node_type> &path, node_type startNode, node_type endNode, weight_type INFINITE_WEIGHT)
	{
		path.Clear();
		if (startNode==endNode)
		{
			path.Insert(startNode);
			path.Insert(endNode);
			return true;
		}

		if (dijkstra.isValid==false || dijkstra.rootNode!=startNode)
		{
			ClearDijkstra();
			GenerateDisjktraMatrix(startNode, INFINITE_WEIGHT);
		}
		
		// return the results
		bool objectExists;
		unsigned col,row;
		weight_type currentWeight;
		DataStructures::Queue<node_type> outputQueue;
		col=dijkstra.costMatrixIndices.GetIndexFromKey(endNode, &objectExists);
		if (dijkstra.costMatrixIndices.Size()<2)
		{
			return false;
		}
		if (objectExists==false)
		{
			return false;
		}
		node_type vertex;
		row=dijkstra.costMatrixIndices.Size()-2;
		if (row==0)
		{
			path.Insert(startNode);
			path.Insert(endNode);
			return true;
		}
		currentWeight=dijkstra.costMatrix[row*adjacencyLists.Size() + col];
		if (currentWeight==INFINITE_WEIGHT)
		{
			// No path
			return true;
		}
		vertex=endNode;
		outputQueue.PushAtHead(vertex);
		row--;
		while (1)
		{
			while (dijkstra.costMatrix[row*adjacencyLists.Size() + col] == currentWeight)
			{
				if (row==0)
				{
					path.Insert(startNode);
					for (col=0; outputQueue.Size(); col++)
						path.Insert(outputQueue.Pop());
					return true;
				}
				--row;
			}

			vertex=dijkstra.leastNodeArray[row];
			outputQueue.PushAtHead(vertex);
			if (row==0)
				break;
			col=dijkstra.costMatrixIndices.GetIndexFromKey(vertex, &objectExists);
			currentWeight=dijkstra.costMatrix[row*adjacencyLists.Size() + col];
		}

		path.Insert(startNode);
		for (col=0; outputQueue.Size(); col++)
			path.Insert(outputQueue.Pop());
		return true;
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		node_type WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::GetNodeAtIndex(unsigned nodeIndex) const
	{
		return adjacencyLists.GetKeyAtIndex(nodeIndex);
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	unsigned WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::GetNodeCount(void) const
	{
		return adjacencyLists.Size();
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	unsigned WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::GetConnectionCount(unsigned nodeIndex) const
	{
        return adjacencyLists[nodeIndex]->Size();
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::GetConnectionAtIndex(unsigned nodeIndex, unsigned connectionIndex, node_type &outNode, weight_type &outWeight) const
	{
		outWeight=adjacencyLists[nodeIndex]->operator[](connectionIndex);
		outNode=adjacencyLists[nodeIndex]->GetKeyAtIndex(connectionIndex);
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
	bool WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::GetSpanningTree(DataStructures::Tree<node_type> &outTree, DataStructures::List<node_type> *inputNodes, node_type startNode, weight_type INFINITE_WEIGHT )
	{
		// Find the shortest path from the start node to each of the input nodes.  Add this path to a new WeightedGraph if the result is reachable
		DataStructures::List<node_type> path;
		DataStructures::WeightedGraph<node_type, weight_type, allowUnlinkedNodes> outGraph;
		bool res;
		unsigned i,j;
		for (i=0; i < inputNodes->Size(); i++)
		{
			res=GetShortestPath(path, startNode, (*inputNodes)[i], INFINITE_WEIGHT);
			if (res)
			{
				for (j=0; j < path.Size()-1; j++)
				{
					// Don't bother looking up the weight
 					outGraph.AddConnection(path[j], path[j+1], INFINITE_WEIGHT);
				}
			}
		}

		// Copy the graph to a tree.
		DataStructures::Queue<NodeAndParent> nodesToProcess;
		DataStructures::Tree<node_type> *current;
		DataStructures::Map<node_type, weight_type> *adjacencyList;
		node_type key;
		NodeAndParent nap, nap2;
		outTree.DeleteDecendants();
		outTree.data=startNode;
		current=&outTree;
		if (outGraph.adjacencyLists.Has(startNode)==false)
			return false;
		adjacencyList = outGraph.adjacencyLists.Get(startNode);

		for (i=0; i < adjacencyList->Size(); i++)
		{
			nap2.node=new DataStructures::Tree<node_type>;
			nap2.node->data=adjacencyList->GetKeyAtIndex(i);
			nap2.parent=current;
			nodesToProcess.Push(nap2);
			current->children.Insert(nap2.node);
		}

		while (nodesToProcess.Size())
		{
			nap=nodesToProcess.Pop();
			current=nap.node;
			adjacencyList = outGraph.adjacencyLists.Get(nap.node->data);

			for (i=0; i < adjacencyList->Size(); i++)
			{
				key=adjacencyList->GetKeyAtIndex(i);
				if (key!=nap.parent->data)
				{
					nap2.node=new DataStructures::Tree<node_type>;
					nap2.node->data=key;
					nap2.parent=current;
					nodesToProcess.Push(nap2);
					current->children.Insert(nap2.node);
				}				
			}
		}

		return true;
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::GenerateDisjktraMatrix(node_type startNode, weight_type INFINITE_WEIGHT)
	{
		dijkstra.costMatrix = new weight_type[adjacencyLists.Size() * adjacencyLists.Size()];
		dijkstra.leastNodeArray = new node_type[adjacencyLists.Size()];

		node_type currentNode;
		unsigned col, row, row2, openSetIndex;
		node_type adjacentKey;
		unsigned adjacentIndex;
		weight_type edgeWeight, currentNodeWeight, adjacentNodeWeight;
		DataStructures::Map<node_type, weight_type> *adjacencyList;
		DataStructures::Heap<weight_type, node_type, false> minHeap;
		DataStructures::Map<node_type, weight_type> openSet;

		for (col=0; col < adjacencyLists.Size(); col++)
		{
			// This should be already sorted, so it's a bit inefficient to do an insertion sort, but what the heck
			dijkstra.costMatrixIndices.Insert(adjacencyLists.GetKeyAtIndex(col),adjacencyLists.GetKeyAtIndex(col));
		}
		for (col=0; col < adjacencyLists.Size() * adjacencyLists.Size(); col++)
			dijkstra.costMatrix[col]=INFINITE_WEIGHT;
		currentNode=startNode;
		row=0;
		currentNodeWeight=0;
		dijkstra.rootNode=startNode;

		// Clear the starting node column
		adjacentIndex=adjacencyLists.GetIndexAtKey(startNode);
		for (row2=0; row2 < adjacencyLists.Size(); row2++)
			dijkstra.costMatrix[row2*adjacencyLists.Size() + adjacentIndex]=0;

		while (row < adjacencyLists.Size()-1)
		{
			adjacencyList = adjacencyLists.Get(currentNode);
			// Go through all connections from the current node.  If the new weight is less than the current weight, then update that weight.
			for (col=0; col < adjacencyList->Size(); col++)
			{
				edgeWeight=(*adjacencyList)[col];
				adjacentKey=adjacencyList->GetKeyAtIndex(col);
				adjacentIndex=adjacencyLists.GetIndexAtKey(adjacentKey);
				adjacentNodeWeight=dijkstra.costMatrix[row*adjacencyLists.Size() + adjacentIndex];

				if (currentNodeWeight + edgeWeight < adjacentNodeWeight)
				{
					// Update the weight for the adjacent node
					for (row2=row; row2 < adjacencyLists.Size(); row2++)
						dijkstra.costMatrix[row2*adjacencyLists.Size() + adjacentIndex]=currentNodeWeight + edgeWeight;
					openSet.Set(adjacentKey, currentNodeWeight + edgeWeight);
				}
			}

			// Find the lowest in the open set
			minHeap.Clear();
			for (openSetIndex=0; openSetIndex < openSet.Size(); openSetIndex++)
				minHeap.Push(openSet[openSetIndex], openSet.GetKeyAtIndex(openSetIndex));

			/*
			unsigned i,j;
			for (i=0; i < adjacencyLists.Size()-1; i++)
			{
				for (j=0; j < adjacencyLists.Size(); j++)
				{
					printf("%2i ", dijkstra.costMatrix[i*adjacencyLists.Size() + j]);
				}
				printf("Node=%i", dijkstra.leastNodeArray[i]);
				printf("\n");
			}
			*/

			if (minHeap.Size()==0)
			{
				// Unreachable nodes
				dijkstra.isValid=true;
				return;
			}

			currentNodeWeight=minHeap.PeekWeight(0);
			dijkstra.leastNodeArray[row]=minHeap.Pop(0);
			currentNode=dijkstra.leastNodeArray[row];
			openSet.Delete(currentNode);		
			row++;
		}

		/*
#ifdef _DEBUG
		unsigned i,j;
		for (i=0; i < adjacencyLists.Size()-1; i++)
		{
			for (j=0; j < adjacencyLists.Size(); j++)
			{
				printf("%2i ", dijkstra.costMatrix[i*adjacencyLists.Size() + j]);
			}
			printf("Node=%i", dijkstra.leastNodeArray[i]);
			printf("\n");
		}
#endif
		*/

		dijkstra.isValid=true;
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::ClearDijkstra(void)
	{
		if (dijkstra.isValid)
		{
			dijkstra.isValid=false;
			delete [] dijkstra.costMatrix;
			delete [] dijkstra.leastNodeArray;
			dijkstra.costMatrixIndices.Clear();
		}
	}

	template <class node_type, class weight_type, bool allowUnlinkedNodes>
		void WeightedGraph<node_type, weight_type, allowUnlinkedNodes>::Print(void)
	{
#ifdef _DEBUG
		unsigned i,j;
		for (i=0; i < adjacencyLists.Size(); i++)
		{
			//printf("%i connected to ", i);
			printf("%s connected to ", adjacencyLists.GetKeyAtIndex(i).ToString());

			if (adjacencyLists[i]->Size()==0)
				printf("<Empty>");
			else
			{
				for (j=0; j < adjacencyLists[i]->Size(); j++)
				//	printf("%i (%.2f) ", adjacencyLists.GetIndexAtKey(adjacencyLists[i]->GetKeyAtIndex(j)), (float) adjacencyLists[i]->operator[](j) );
					printf("%s (%.2f) ", adjacencyLists[i]->GetKeyAtIndex(j).ToString(), (float) adjacencyLists[i]->operator[](j) );
			}

			printf("\n");
		}
#endif
	}
}

#endif
