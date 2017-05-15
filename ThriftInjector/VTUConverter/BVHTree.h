#ifndef __BVH_TREE_H__
#define __BVH_TREE_H__

#include <algorithm>
#include <vector>
#include <stdint.h>
#include <map>

#include "AABB.h"

#ifdef WIN32
#	define BVH_TREE_INLINE	__forceinline
#else
#	define BVH_TREE_INLINE inline
#endif

struct BVHPrimitiveInfo {
	BVHPrimitiveInfo() {
	}

	BVHPrimitiveInfo(int pn, const AABB& bb) {
		primitiveNumber = pn;
		point[0] = (bb.min[0] + bb.max[0]) / 2.0f;	point[1] = (bb.min[1] + bb.max[1]) / 2.0f;	point[2] = (bb.min[2] + bb.max[2]) / 2.0f;
		bbox = bb;
	}

	BVHPrimitiveInfo(const BVHPrimitiveInfo& c): primitiveNumber(c.primitiveNumber), bbox(c.bbox) {
		point[0] = c.point[0];
		point[1] = c.point[1];
		point[2] = c.point[2];
	}

	int primitiveNumber;
	float point[3];
	AABB bbox;
};

struct BVHBuildNode {
	BVHBuildNode() {
		child[0] = child[1] = 0;
	}

  ~BVHBuildNode() {
    delete child[0];
    delete child[1];
  }

	BVHBuildNode(const BVHBuildNode& c) {
		child[0] = c.child[0];
		child[1] = c.child[1];
		splitAxis = c.splitAxis;
		firstPrimOffset = c.firstPrimOffset;
		nPrimitives = c.nPrimitives;
	}

	void initLeaf(uint32_t first, uint32_t n, const AABB& bound) {
		firstPrimOffset = first;
		nPrimitives = n;
		bbox = bound;

		child[0] = child[1] = 0;
	}

	void initInterior(uint32_t axis, BVHBuildNode* l_child, BVHBuildNode* r_child) {
		splitAxis = axis;
		child[0] = l_child;	child[1] = r_child;
		nPrimitives = 0;
		bbox.extend(l_child->bbox);
		bbox.extend(r_child->bbox);
	}

	AABB bbox;
	BVHBuildNode* child[2];
	uint32_t splitAxis, firstPrimOffset, nPrimitives;
};

struct LinearBVHInteriorNode {
  // Bounding Box around the left and right child
  AABB leftBound, rightBound;

	// If negative, the child is a leaf node
	// - 1 -> 0 (-index + 1)
	// - 2 -> 1 (-index + 1)
	int	children [2];

	// children [0] == boundsFirst
	// children [1] == boundsSecond

	uint8_t	splitAxis;
	// splitAxis: x = 0, y = 1, z = 2
};

struct LinearBVHLeafNode {
  // offsets into the point array
  // This leaf includes voxels [low, high)
  int lowIdx, highIdx;
};

struct BucketInfo {
	BucketInfo() {
		count = 0;
	}

	int count;
	AABB bbox;
};

class BVHTree 
{
public:

	enum SplitMethod {
		SPLIT_MIDDLE,
		SPLIT_EQUAL_COUNTS,
		SPLIT_SAH
	};

	BVH_TREE_INLINE BVHTree() {};
	BVH_TREE_INLINE ~BVHTree();

	BVH_TREE_INLINE void init(const unsigned int& maxPrimsInNode, const SplitMethod& split_method);
	virtual void build( 
		std::vector<AABB>& cellBoxes, std::vector<glm::vec3>& cellVectors, 
		std::vector<glm::u32>& cellPoints, 
		std::vector<glm::u32>& cellPointsBegIndices,
		std::map<glm::u32, glm::u32>& mapCellIndex);
	//virtual bool getBoundingPrimitiveIndex(const UnstructDataset* const datasetPtr, const float point[3], PrimitiveIndex& cell_index);

	virtual glm::vec3 getElementExtents() const;
	std::vector< LinearBVHLeafNode     > m_leaf_nodes;


private:
	BVHBuildNode* recursiveBuild(std::vector<BVHPrimitiveInfo>& buildData, uint32_t start, uint32_t end, int& level, uint32_t *totalInteriorNodes, uint32_t *totalLeafNodes,
		std::vector<AABB>& primitive_cellBoxes, std::vector<glm::vec3>& primitive_cellVectors, std::vector<glm::u32>& primitive_cellPoints, std::vector<glm::u32>& primitive_cellPointsBegIndices,
		std::vector<AABB>& ordered_cellBoxes, std::vector<glm::vec3>& ordered_cellVectors, std::vector<glm::u32>& ordered_cellPoints, std::vector<glm::u32>& ordered_cellPointsBegIndices,
		std::map<glm::u32, glm::u32>& mapCellIndex);

	uint32_t flattenBVHTree(BVHBuildNode* node, uint32_t& offset);

	BVHBuildNode* m_root;
	SplitMethod  m_split_method;
	unsigned int m_nMaxPrimsInNode;
	uint32_t m_total_interior_nodes;
	uint32_t m_total_leaf_nodes;

	std::vector< LinearBVHInteriorNode > m_interior_nodes;
	//std::vector< BoundingBox > m_bounding_boxes;

	glm::vec3 m_minElementExtent;

	// Peformance
	int m_max_cells_per_leaf;
	float m_avg_cells_per_leaf;
	uint32_t m_n_non_empty_leaf;
	int m_n_levels;
	uint64_t m_occupied_memory;

};

// Comparison structs
struct CompareToMid {
	CompareToMid(int d, float m)  {	  dim = d; mid = m;	}

	int dim;
	float mid;

	bool operator()(const BVHPrimitiveInfo & a) const {
		return a.point[dim] < mid;
	}

};

struct CompareToBucket {
	CompareToBucket(int split, int num, int d, const AABB& bbox): centroidBounds(bbox) {
		splitBucket = split;
		nBuckets = num;
		dim = d;
	}

	bool operator()(const BVHPrimitiveInfo& p) const {
		int b = static_cast<int>( nBuckets * ( ( p.point[dim] - centroidBounds.min[dim] ) / ( centroidBounds.max[dim] - centroidBounds.min[dim] ) ) );
		if (b == nBuckets) b = nBuckets - 1;
		return b <= splitBucket;
	}

	int splitBucket, nBuckets, dim;
	const AABB& centroidBounds;
};



BVH_TREE_INLINE void BVHTree::init(const unsigned int& maxPrimsInNode, const SplitMethod& split_method) {
	m_nMaxPrimsInNode  = maxPrimsInNode; //std::min(maxPrimsInNode, 255u);
	m_split_method     = split_method;
	m_total_interior_nodes  = 0;
	m_total_leaf_nodes      = 0;
	m_minElementExtent = glm::vec3(0.0f);
}

BVH_TREE_INLINE BVHTree::~BVHTree() {
}


#endif