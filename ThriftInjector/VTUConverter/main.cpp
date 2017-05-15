
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <ctime>
#include <functional>

#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataSet.h>
#include <vtkDataArray.h>
#include <vtkIdList.h>
#include <vtkUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkTetra.h>

#include "BVHTree.h"

#include "AABB.h"

//#define WRITE_VTU

struct Tetrahedron {
	uint32_t indices[4];
};

struct Vec3 {
	Vec3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	union {
		struct { float x, y, z; };
		float Coords[3];
	};
};

struct Result
{
	std::string name;
	std::vector<std::string> componentNames;
	std::vector<std::vector<float>> values;
};

vtkSmartPointer<vtkUnstructuredGrid> WriteVTU(
	const char * filename, 
	std::vector<Vec3>& SimPoints, 
	std::vector<Tetrahedron>& SimTets, 
	std::vector<Result>& SimResults, 
	std::vector<int> addedIndices, 
	std::map<glm::u32, glm::u32> vtuIndexMap,
	int partition);

int main(int argc, char *argv[])
{
	std::string filename = "ts_small_trailtest.vtu";

	std::string rawname = filename.substr(0, filename.find_last_of("."));

	int partitionCounter = 0;
	int timestep = 1;

	std::cout << "============================== " << std::endl;
	std::cout << "Converting     " << filename << std::endl;



	auto reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
	reader->SetFileName(filename.c_str());
	reader->Update();
	
	vtkDataSet * block0 = reader->GetOutputAsDataSet();
	vtkCellData * cellData = block0->GetCellData();
	vtkPointData * pointData = block0->GetPointData();
	
	vtkIdType numCells = block0->GetNumberOfCells();
	vtkIdType numPoints = block0->GetNumberOfPoints();
	
	std::vector<vtkDataArray*> dataArrays;

	std::cout << "Number of cells: " << numCells << std::endl;
	std::cout << "Number of points: " << numPoints << std::endl;
	std::cout << "============================== " << std::endl;
	std::cout << "Extracting Points.... " << std::endl;
	
	// BVH Data
	std::vector<glm::vec3> points;
	std::vector<Result> results;

	std::vector<glm::u32> cellPoints;
	std::vector<glm::u32> cellPointsBegIndices;
	std::vector<AABB> cellBoxes;
	
	// Construct cell boxes 
	std::cout << "Construct cell boxes....\n";
	for (vtkIdType i = 0; i < numCells; i++) {
		vtkSmartPointer < vtkIdList > cellPointIds = vtkSmartPointer < vtkIdList > ::New();
		block0->GetCellPoints(i, cellPointIds);

		AABB box;
		for (vtkIdType j = 0; j < cellPointIds->GetNumberOfIds(); j++) {
			double point[3];
			block0->GetPoint(cellPointIds->GetId(j), point);
			box.extend((float)point[0], (float)point[1], (float)point[2]);
		}
		cellBoxes.push_back(box);
	}

	// Import point data 

	double dbgPt[3];
	block0->GetPoint(0, dbgPt);
	std::cout << "POINT ======= " <<  dbgPt[0] << " " << dbgPt[1] << " " << dbgPt[2] << std::endl;
	return 0;

	std::cout << "Import point data....\n";
	for (vtkIdType i = 0; i < numPoints; i++) {
		double point[3];
		block0->GetPoint(i, point);
		points.push_back(glm::vec3((float)point[0], (float)point[1], (float)point[2]));
	}

	Result partitionResult;
	partitionResult.name = "PartitionID";
	partitionResult.componentNames.push_back("PartitionID");
	partitionResult.values.resize(numPoints);
	results.push_back(partitionResult);

	// Import cell data 
	for (int i = 0; i < pointData->GetNumberOfArrays(); i++)
	{
		Result r;

		auto dataName = pointData->GetArrayName(i);
		auto dataArray = pointData->GetVectors(dataName);
		vtkIdType numTuples = dataArray->GetNumberOfTuples();

		r.name = dataName;

		std::cout << "Extracting " << dataName << std::endl;
		std::cout << "Number of tuples: " << numTuples << " -> components: " << dataArray->GetNumberOfComponents() << std::endl;

		for (vtkIdType id = 0; id < dataArray->GetNumberOfComponents(); id++)
		{
			if (dataArray->HasAComponentName())
			{
				std::string cName = dataArray->GetComponentName(id);

				r.componentNames.push_back(cName);
			}
			else if (dataArray->GetNumberOfComponents() > 1)
			{
				std::stringstream compName;
				compName << "Comp" << id;
				r.componentNames.push_back(compName.str());
			}
			else
			{
				r.componentNames.push_back(dataName);
			}

		}

		std::vector<std::vector<float>> resArray;

		for (int i = 0; i < numTuples; i++)
		{
			std::vector<float> res;
			double *vals = dataArray->GetTuple(i);
			
			for (int k = 0; k < dataArray->GetNumberOfComponents(); k++)
			{
				res.push_back(vals[k]);
			}

			resArray.push_back(res);
		}

		r.values = resArray;
		results.push_back(r);
	}


	// Import cell points 
	std::cout << "Import cell points....\n";
	cellPointsBegIndices.resize(block0->GetNumberOfCells());
	for (vtkIdType i = 0; i < numCells; i++) {
		vtkSmartPointer < vtkIdList > cellPointIds = vtkSmartPointer < vtkIdList > ::New();
		block0->GetCellPoints(i, cellPointIds);

		cellPointsBegIndices[i] = static_cast < unsigned int >(cellPoints.size());
		cellPoints.push_back(cellPointIds->GetNumberOfIds());
		for (vtkIdType j = 0; j < cellPointIds->GetNumberOfIds(); j++) {
			cellPoints.push_back(cellPointIds->GetId(j));
		}
	}

	BVHTree bvh;
	bvh.init(1000000, BVHTree::SplitMethod::SPLIT_SAH);

	std::map<glm::u32, glm::u32> mapCellIndex;

	std::vector<glm::vec3> dump;
	bvh.build(cellBoxes, dump, cellPoints, cellPointsBegIndices, mapCellIndex);

	std::cout << "==============================" << std::endl;
	
	for (int i = 0; i < bvh.m_leaf_nodes.size(); i++)
	{
		auto leaf = bvh.m_leaf_nodes[i];
		std::vector<int> addedIndexes;

#ifdef WRITE_VTU
		std::string fName = rawname + "_" + std::to_string(i) + ".vtu";
		std::vector<Vec3> vtuPoints;
		std::vector<Tetrahedron> vtuCells;
		std::vector<Result> vtuResults;
		std::map<glm::u32, glm::u32> vtuIndex;
#endif // WRITE_VTU

		std::stringstream outMshStr, outResStr;

		outMshStr << rawname << "_" << i << ".post.msh";
		outResStr << rawname << "_" << i << ".post.res";

		std::cout << "Writing output Mesh:   " << outMshStr.str() << std::endl;
		std::cout << "Writing output Result: " << outResStr.str() << std::endl;

		ofstream outmsh;
		outmsh.open(outMshStr.str());

		outmsh << "MESH " << "\"" << rawname << "\"" << " dimension 3 ElemType Tetrahedra Nnode 4\n";

		std::stringstream coordString;
		std::stringstream elemString;

		outmsh << "Coordinates\n";
		elemString << "Elements\n";

		for (int j = leaf.lowIdx; j < leaf.highIdx; j++)
		{
			glm::u32 cellIdx = mapCellIndex[j];

			vtkSmartPointer < vtkIdList > cellPointIds = vtkSmartPointer < vtkIdList > ::New();
			block0->GetCellPoints(cellIdx, cellPointIds);

			elemString << cellIdx << " ";

#ifdef WRITE_VTU
			Tetrahedron t;
#endif // WRITE_VTU

			for (vtkIdType k = 0; k < cellPointIds->GetNumberOfIds(); k++)
			{
				int cellPntID = cellPointIds->GetId(k);


				if (std::find(addedIndexes.begin(), addedIndexes.end(), cellPntID) == addedIndexes.end()) 
				{
					// Index hasn't been added yet
					addedIndexes.push_back(cellPntID);
					glm::vec3 point = points[cellPntID];
					outmsh << cellPntID << " " << point[0] << " " << point[1] << " " << point[2] << "\n";

					results[0].values[cellPntID].push_back(i);

#ifdef WRITE_VTU
					vtuIndex[cellPntID] = (int) vtuPoints.size();
					vtuPoints.push_back(Vec3(point.x, point.y, point.z));

#endif // WRITE_VTU

				}

				elemString << cellPntID << " ";

#ifdef WRITE_VTU
				t.indices[k] = vtuIndex[cellPntID];
#endif // WRITE_VTU

			}
			
#ifdef WRITE_VTU
			vtuCells.push_back(t);
#endif // WRITE_VTU

			elemString << "\n";

		}

		outmsh << "End Coordinates\n\n";
		elemString << "End Elements\n";

		outmsh << elemString.str();

		outmsh.close();

		// Results 
		ofstream outres;
		outres.open(outResStr.str());

		outres << "GiD Post Results File 1.0\n";
		outres << "\n";
		outres << "# encoding utf-8\n";

		//// Add Partition ID
		//outres << "Result \"" << "PartitionID" << "\" " << "\"" << rawname << "\" " << timestep << " Scalar OnNodes\n";
		//outres << "ComponentNames \"ID\"\n";
		//outres << "Values\n";
		//for (int m = 0; m < addedIndexes.size(); m++)
		//{
		//	int idx = addedIndexes[m];
		//	outres << idx << " " << i << "\n";
		//}
		//outres << "End Values\n\n";

		// Add containing results
		for (auto it = results.begin(); it != results.end(); ++it)
		{
			auto result = (*it);

			std::string dataName = result.name;

			outres << "Result \"" << dataName << "\" " << "\"" << rawname << "\" " << timestep;

			if (result.componentNames.size() > 1)
				outres << " Vector ";
			else
				outres << " Scalar ";

			outres << "OnNodes\n";

			outres << "ComponentNames ";

			auto names = result.componentNames;
			int iterateCounter = 0;

			for (auto nit = names.begin(); nit != names.end(); ++nit)
			{
				outres << "\"" << (*nit) << "\"";
				if (iterateCounter + 1 != names.size())
					outres << ", ";

				iterateCounter++;
			}

			outres << "\n";
			outres << "Values\n";

			for (int m = 0; m < addedIndexes.size(); m++)
			{
				int idx = addedIndexes[m];
				outres << idx << " ";
				std::vector<float> vals = result.values[idx];
				if (dataName == "PartitionID")
				{
					outres << i << " ";
				}
				else
				{
					for (int n = 0; n < vals.size(); n++)
					{
						outres << vals[n] << " ";
					}
				}
				outres << "\n";
			}

			outres << "End Values\n\n";
		}

		std::cout << "-------------------------" << std::endl;

		outres.close();

#ifdef WRITE_VTU
		WriteVTU(fName.c_str(), vtuPoints, vtuCells, results, addedIndexes, vtuIndex, i);
#endif // WRITE_VTU

	}


	//Result dbg = results[0];

	//for (int z = 0; z < dbg.values.size(); z++)
	//{
	//	if (dbg.values[z].size() > 1 || dbg.values[z].size() == 0)
	//	{
	//		std::cout << "pos: " << z << " size: " << dbg.values[z].size() << std::endl;
	//		for (size_t y = 0; y < dbg.values[z].size(); y++)
	//		{
	//			std::cout << dbg.values[z][y] << " ";
	//		}
	//		std::cout << std::endl;
	//	}
	//}

	std::cout << "============================== " << std::endl;
	std::cout << "Finished!" << std::endl;

	return 0;
}

vtkSmartPointer<vtkUnstructuredGrid> WriteVTU(
	const char * filename, 
	std::vector<Vec3>& SimPoints, 
	std::vector<Tetrahedron>& SimTets, 
	std::vector<Result>& SimResults, 
	std::vector<int> addedIndices, 
	std::map<glm::u32, glm::u32> vtuIndexMap,
	int partition)
{
	// Make a tetrahedron.
	int numberOfVerticesPerElement = 4;//static_cast<int>(SimPoints.size());

	vtkSmartPointer< vtkPoints > points =
		vtkSmartPointer< vtkPoints > ::New();

	for (auto& p : SimPoints)
		points->InsertNextPoint(p.x, p.y, p.z);

	vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
		vtkSmartPointer<vtkUnstructuredGrid>::New();
	unstructuredGrid->SetPoints(points);

	for (auto& t : SimTets) {

		vtkSmartPointer<vtkTetra> tetra =
			vtkSmartPointer<vtkTetra>::New();

		for (int i = 0; i < numberOfVerticesPerElement; ++i)
			tetra->GetPointIds()->SetId(i, t.indices[i]);

		unstructuredGrid->InsertNextCell(tetra->GetCellType(), tetra->GetPointIds());
	}

	//vtkSmartPointer<vtkFloatArray> pressure =
	//	vtkSmartPointer<vtkFloatArray>::New();
	//pressure->SetNumberOfComponents(1);
	//pressure->SetName("pressure");



	for (size_t i = 0; i < SimResults.size(); ++i) {
		vtkSmartPointer<vtkFloatArray> result = vtkSmartPointer<vtkFloatArray>::New();

		result->SetName(SimResults[i].name.c_str());

		if (SimResults[i].name == "PartitionID")
		{
			result->SetNumberOfComponents(1);
			for (int j = 0; j < unstructuredGrid->GetNumberOfPoints(); ++j)
			{
				float fPart = partition;
				result->InsertNextTupleValue(&fPart);
			}
				
		}
		else
		{
			result->SetNumberOfComponents((int) SimResults[i].values[0].size());

			// TODO: Add correct Results
			for (int k = 0; k < addedIndices.size(); k++)
			{
				auto adjIdx = vtuIndexMap[addedIndices[k]];
				for (int j = 0; j < SimResults[i].values[0].size(); j++)
				{
					result->InsertComponent(adjIdx, j, SimResults[i].values[addedIndices[k]][j]);

				}

			}
		}

	/*	for (int k = 0; k < addedIndices.size(); k++)
		{
			auto adjIdx = vtuIndexMap[addedIndices[k]];
			result->InsertNextTupleValue(SimResults[i].values[addedIndices[k]].data());

		}*/

		unstructuredGrid->GetPointData()->AddArray(result);
	}


	//}
	//vtkSmartPointer<vtkFloatArray> velocity =
	//	vtkSmartPointer<vtkFloatArray>::New();
	//velocity->SetNumberOfComponents(3);
	//velocity->SetName("velocity");

	//std::mt19937_64::result_type seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	//auto real_rand = std::bind(std::uniform_real_distribution<float>(0, 10),
	//	std::mt19937_64(seed));


	//float vel[3] = { 1, 1, 1 };
	//float p = 0;
	//for (int i = 0; i < unstructuredGrid->GetNumberOfPoints(); ++i)
	//{
	//	
	//		vel[j] = SimResults[i][i];
	//	}

	//	p = real_rand() * 1000;
	//	pressure->InsertNextTupleValue(&p);
	//	vel[0] = ;// result real_rand();
	//	vel[1] = real_rand();
	//	vel[2] = real_rand();
	//	velocity->InsertNextTupleValue(vel);
	//}
	//unstructuredGrid->GetPointData()->AddArray(pressure);
	//unstructuredGrid->GetPointData()->AddArray(velocity);
	unstructuredGrid->GetPointData()->SetActiveScalars(SimResults[0].name.c_str());

	vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer =
		vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
	writer->SetFileName(filename);
	writer->SetInputData(unstructuredGrid);
	writer->Write();

	return unstructuredGrid;
}