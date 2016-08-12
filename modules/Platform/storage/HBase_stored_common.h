#pragma once

bool getColumnQualifierStringFromRow( std::string &retValue, const TRowResult &rowResult, 
				      const char *columnFamily, const char *columnQualifier);
void SplitBinaryMeshInChunks( std::vector< std::string> &lst_chunks_data, const std::string &binary_mesh);
int getColumnQualifierStringListFromRow( std::vector< std::string> &retLstValues, 
					 const TRowResult &rowResult, 
					 const char *columnFamily, const char *columnQualifierPrefix,
					 const int numStringsToRetrieve);
