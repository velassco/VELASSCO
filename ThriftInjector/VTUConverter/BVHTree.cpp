#include "BVHTree.h"

#include <iostream>
#include <queue>

void BVHTree::build( std::vector<AABB>& primitive_cellBoxes, std::vector<glm::vec3>& primitive_cellVectors, std::vector<glm::u32>& primitive_cellPoints, std::vector<glm::u32>& primitive_cellPointsBegIndices, std::map<glm::u32, glm::u32>& mapCellIndex)
{

	bool dirty = true;
	m_minElementExtent = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_avg_cells_per_leaf = 0;
	m_max_cells_per_leaf = 0;
	m_n_levels = 0;

	std::cout  << "Building BVH tree...";

	std::vector<BVHPrimitiveInfo> buildData;
	buildData.reserve( primitive_cellBoxes.size() );
	for(size_t i = 0; i < primitive_cellBoxes.size(); i++){
		buildData.push_back( BVHPrimitiveInfo( static_cast<int>( i ), primitive_cellBoxes[i] ) );
	}

	  uint32_t totalInteriorNodes = 1, totalLeafNodes = 0;
	  int level = 0;

	std::vector<AABB>      ordered_cellBoxes;
	std::vector<glm::vec3> ordered_cellVectors;
	std::vector<glm::u32>  ordered_cellPoints;
	std::vector<glm::u32>  ordered_cellPointsBegIndices;

	m_root = recursiveBuild(buildData, 0, static_cast<uint32_t>( primitive_cellBoxes.size() ), level, &totalInteriorNodes, &totalLeafNodes,
		primitive_cellBoxes, primitive_cellVectors, primitive_cellPoints, primitive_cellPointsBegIndices,
		ordered_cellBoxes, ordered_cellVectors, ordered_cellPoints, ordered_cellPointsBegIndices, mapCellIndex);

	primitive_cellBoxes.swap(ordered_cellBoxes);
	//primitive_cellVectors.swap(ordered_cellVectors);
	primitive_cellPoints.swap(ordered_cellPoints);
	primitive_cellPointsBegIndices.swap(ordered_cellPointsBegIndices);


	// compute the rerepresentation of depth-first traversal of BVH Tree
	m_interior_nodes.resize( totalInteriorNodes );
	m_leaf_nodes.reserve( totalLeafNodes );
	uint32_t offset = 0;
	flattenBVHTree(m_root, offset);


	std::cout  << "Building BVH tree (total Interior nodes = " << totalInteriorNodes << ", total Leaf nodes = " << totalLeafNodes << ")...Done\n";
	//std::cout  << "Occupied Memory = " << m_occupied_memory  << std::endl;
	std::cout  << "Max Levels = " << m_n_levels << std::endl;

	delete m_root;
}

BVHBuildNode* BVHTree::recursiveBuild(std::vector<BVHPrimitiveInfo>& buildData, uint32_t start, uint32_t end, int& level, uint32_t *totalInteriorNodes, uint32_t *totalLeafNodes,
									  std::vector<AABB>& primitive_cellBoxes, std::vector<glm::vec3>& primitive_cellVectors, std::vector<glm::u32>& primitive_cellPoints, std::vector<glm::u32>& primitive_cellPointsBegIndices,
									  std::vector<AABB>& ordered_cellBoxes, std::vector<glm::vec3>& ordered_cellVectors, std::vector<glm::u32>& ordered_cellPoints, std::vector<glm::u32>& ordered_cellPointsBegIndices,
									  std::map<glm::u32, glm::u32>& mapCellIndex)
{
	BVHBuildNode* node = new BVHBuildNode();

	level++;
	if(level > m_n_levels)
		m_n_levels = level;

	// compute bounds of all primitives in the BVH node
	AABB bbox;
	for(uint32_t i = start; i < end; i++){
		bbox.extend(buildData[i].bbox);
	}

	uint32_t nPrimitives = end - start;
	if(nPrimitives <= m_nMaxPrimsInNode) {

		// Create Leaf Node
		uint32_t firstPrimitiveOffset = static_cast<uint32_t>( ordered_cellBoxes.size() );
		for(uint32_t i = start; i < end; i++){
			uint32_t primNum = buildData[i].primitiveNumber;

			mapCellIndex[static_cast<int>(ordered_cellBoxes.size()) ] = primNum;

			ordered_cellBoxes.push_back( primitive_cellBoxes[primNum] );
			//ordered_cellVectors.push_back( primitive_cellVectors[primNum] );
			ordered_cellPointsBegIndices.push_back( static_cast<unsigned int>( ordered_cellPoints.size() ) );
			ordered_cellPoints.push_back( primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] ] );
			for(size_t cp_i = 1; cp_i <= primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] ]; cp_i++)
				ordered_cellPoints.push_back( primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] + cp_i ] );
		}
		node->initLeaf(firstPrimitiveOffset, nPrimitives, bbox);
    (*totalLeafNodes)++;
    (*totalInteriorNodes)--;

	} else {
		// compute bound of primitive centroid
		AABB centroidBounds;
		for(uint32_t i = start; i < end; i++)
			centroidBounds.extend(buildData[i].point[0], buildData[i].point[1], buildData[i].point[2]);

		// choose split dimension dim
		int dim;
		float l = centroidBounds.max[0] - centroidBounds.min[0], w = centroidBounds.max[1] - centroidBounds.min[1], h = centroidBounds.max[2] - centroidBounds.min[2];
		if ( l >= w && l >= h )
			dim = 0;
		else if(w >= l && w >= h)
			dim = 1;
		else if(h >= l && h >= w)
			dim = 2;

		// partition primitives into two sets and build children
		uint32_t mid = (start + end) / 2;
		if(centroidBounds.max[dim] == centroidBounds.min[dim]){

			uint32_t firstPrimitiveOffset = static_cast<uint32_t>( ordered_cellBoxes.size() );
			for(uint32_t i = start; i < end; i++){
				uint32_t primNum = buildData[i].primitiveNumber;

				mapCellIndex[static_cast<int>(ordered_cellBoxes.size())] = primNum;

				ordered_cellBoxes.push_back( primitive_cellBoxes[primNum] );
				//ordered_cellVectors.push_back( primitive_cellVectors[primNum] );
				ordered_cellPointsBegIndices.push_back( static_cast<unsigned int>( ordered_cellPoints.size() ) );
				ordered_cellPoints.push_back( primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] ] );
				for(size_t cp_i = 1; cp_i <= primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] ]; cp_i++)
					ordered_cellPoints.push_back( primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] + cp_i ] );
			}
			node->initLeaf(firstPrimitiveOffset, nPrimitives, bbox);
      (*totalLeafNodes)++;
      (*totalInteriorNodes)--;
      level--;

			return node;
		}

		// partition primitives based on splitmethod

		if (m_split_method == SplitMethod::SPLIT_SAH){

			const int nBuckets = 12;

			// Surface Area Heuristics
			BucketInfo buckets[nBuckets];
			for(uint32_t i = start; i < end; i++){
				int b = static_cast<int>( nBuckets * ( ( buildData[i].point[dim] - centroidBounds.min[dim] ) / ( centroidBounds.max[dim] - centroidBounds.min[dim] ) ) );
				if ( b == nBuckets ) b = nBuckets - 1;
				buckets[b].count++;
				buckets[b].bbox.extend(buildData[i].bbox);
			}

			float cost[nBuckets - 1];
			for(int i = 0; i < nBuckets - 1; i++){

				AABB b0, b1;
				int count0 = 0, count1 = 0;
				for(int j = 0; j <= i; j++){
					b0.extend( buckets[j].bbox );
					count0 += buckets[j].count;
				}
				for(int j = i+1; j < nBuckets; j++){
					b1.extend( buckets[j].bbox );
					count1 += buckets[j].count;
				}

				cost[i] = .125f + (count0 * b0.volume() + count1 * b1.volume()) / bbox.volume();
			}

			float minCost = cost[0];
			uint32_t minCostSplit = 0;
			for(int i = 1; i < nBuckets - 1; i++){
				if(cost[i] < minCost){
					minCost = cost[i];
					minCostSplit = i;
				}
			}


			if(nPrimitives > m_nMaxPrimsInNode || minCost < nPrimitives){
				// interior node
				BVHPrimitiveInfo* pmid = std::partition(
					&buildData[start],
					&buildData[end - 1] + 1,
					CompareToBucket(minCostSplit, nBuckets, dim, centroidBounds));

				mid = static_cast<uint32_t>( pmid - &buildData[0] );

			} else {
				// leaf node
				uint32_t firstPrimitiveOffset = static_cast<uint32_t>( ordered_cellBoxes.size() );
				for(uint32_t i = start; i < end; i++){
					uint32_t primNum = buildData[i].primitiveNumber;

					mapCellIndex[static_cast<int>(ordered_cellBoxes.size())] = primNum;

					ordered_cellBoxes.push_back( primitive_cellBoxes[primNum] );
					//ordered_cellVectors.push_back( primitive_cellVectors[primNum] );
					ordered_cellPointsBegIndices.push_back( static_cast<unsigned int>( ordered_cellPoints.size() ) );
					ordered_cellPoints.push_back( primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] ] );
					for(size_t cp_i = 1; cp_i <= primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] ]; cp_i++)
						ordered_cellPoints.push_back( primitive_cellPoints[ primitive_cellPointsBegIndices[ primNum ] + cp_i ] );
				}
				node->initLeaf(firstPrimitiveOffset, nPrimitives, bbox);
        (*totalLeafNodes)++;
        (*totalInteriorNodes)--;
        level--;

				return node;
			}
		} else if(m_split_method == SplitMethod::SPLIT_MIDDLE){

			// Middle method.
			float pmid = 0.5f * (centroidBounds.min[dim] + centroidBounds.max[dim]);
			BVHPrimitiveInfo* midPtr = std::partition(&buildData[start], &buildData[end - 1] + 1, CompareToMid(dim, pmid));
			mid = static_cast<uint32_t>( midPtr - &buildData[0] );

		} else if (m_split_method == SplitMethod::SPLIT_EQUAL_COUNTS) {

			// Equal counts method.
		}

		// build the children recursively.
		node->initInterior(dim,
      recursiveBuild(buildData, start, mid, level, totalInteriorNodes, totalLeafNodes, primitive_cellBoxes, primitive_cellVectors, primitive_cellPoints, primitive_cellPointsBegIndices, ordered_cellBoxes, ordered_cellVectors, ordered_cellPoints, ordered_cellPointsBegIndices, mapCellIndex),
	  recursiveBuild(buildData, mid, end, level, totalInteriorNodes, totalLeafNodes, primitive_cellBoxes, primitive_cellVectors, primitive_cellPoints, primitive_cellPointsBegIndices, ordered_cellBoxes, ordered_cellVectors, ordered_cellPoints, ordered_cellPointsBegIndices, mapCellIndex));

    (*totalInteriorNodes) += 2;
	}

  level--;

  return node;
}

uint32_t BVHTree::flattenBVHTree(BVHBuildNode* node, uint32_t& offset) {

  LinearBVHInteriorNode* interiorNode = &m_interior_nodes[offset];
  uint32_t myoffset  = offset;

  interiorNode->splitAxis = node->splitAxis;
  interiorNode->leftBound = node->child[0]->bbox;
  interiorNode->rightBound = node->child[1]->bbox;

  if(node->child[0]->nPrimitives > 0){

    LinearBVHLeafNode leafNode;
    leafNode.lowIdx  = node->child[0]->firstPrimOffset;
    leafNode.highIdx = node->child[0]->firstPrimOffset + node->child[0]->nPrimitives;

    if ( node->child[0]->nPrimitives > ( static_cast<uint32_t>( m_max_cells_per_leaf ) ) )
      m_max_cells_per_leaf = static_cast<int>(node->child[0]->nPrimitives);

    m_avg_cells_per_leaf += node->child[0]->nPrimitives;

    m_leaf_nodes.push_back(leafNode);

    interiorNode->children[0] = -(int)(m_leaf_nodes.size());

  } else {

    offset++;
    interiorNode->children[0] = flattenBVHTree(node->child[0], offset);

  }


  if(node->child[1]->nPrimitives > 0){

    LinearBVHLeafNode leafNode;
    leafNode.lowIdx  = node->child[1]->firstPrimOffset;
    leafNode.highIdx = node->child[1]->firstPrimOffset + node->child[1]->nPrimitives;

    if (node->child[0]->nPrimitives > (static_cast<uint32_t>(m_max_cells_per_leaf)))
      m_max_cells_per_leaf = static_cast<int>(node->child[0]->nPrimitives);

    m_avg_cells_per_leaf += node->child[1]->nPrimitives;

    m_leaf_nodes.push_back(leafNode);
    interiorNode->children[1] = -(int)(m_leaf_nodes.size());

  } else {

    offset++;
    interiorNode->children[1] = flattenBVHTree(node->child[1], offset);

  }

  return myoffset;
}

#if 0
#define EVALUATION
bool BVHTree::getBoundingPrimitiveIndex(const UnstructDataset* const datasetPtr, const float point[3], PrimitiveIndex& cell_index) {
#ifdef EVALUATION
  uint32_t n_checks = 0;
#endif

	bool found = false;
	uint32_t idx = 0;
	int todoOffset = 0, nodeNum = 0;
	int todo[64];

	while(true){

    // if the current node is an interior node
    if(nodeNum >= 0){

      const LinearBVHInteriorNode* node = &m_interior_nodes[nodeNum];

      const bool insideLeftChild   = node->leftBound.contains(point/*, node->splitAxis*/);
      const bool insideRightChilld = node->rightBound.contains(point/*, node->splitAxis*/);
#ifdef EVALUATION
      n_checks += 2;
#endif
      if(insideLeftChild && insideRightChilld){
        todo[todoOffset++] = node->children[1];
        nodeNum = node->children[0];

      } else if(insideLeftChild && !insideRightChilld){
        nodeNum = node->children[0];
      } else if(insideRightChilld && !insideLeftChild){
        nodeNum = node->children[1];
      } else {
        if(todoOffset == 0) break;
        nodeNum = todo[--todoOffset];
      }

    } else {

      const LinearBVHLeafNode* node = &m_leaf_nodes[-nodeNum - 1];

      // inside leaf node
      for(int i = node->lowIdx; i < node->highIdx; i++){
#ifdef EVALUATION
        n_checks++;
#endif
				if( datasetPtr->insideCell( i, glm::vec3( point[0], point[1], point[2] ) ) ){
          cell_index = i;
#ifdef EVALUATION
          m_n_traverses++;
          m_avg_check_per_traverse = (m_avg_check_per_traverse * (m_n_traverses - 1) + n_checks) / m_n_traverses;
#endif
          return true;
				}
      }

      if(found || todoOffset == 0) break;
			nodeNum = todo[--todoOffset];

    }
  }

#ifdef EVALUATION
  m_n_traverses++;
  m_avg_check_per_traverse = (m_avg_check_per_traverse * (m_n_traverses - 1) + n_checks) / m_n_traverses;
#endif
	return false;
}
#endif

glm::vec3 BVHTree::getElementExtents() const {

  // TODO WRONG!
  glm::vec3 extents = m_interior_nodes[0].leftBound.max - m_interior_nodes[0].leftBound.min;
  return extents * 0.01f;

}
