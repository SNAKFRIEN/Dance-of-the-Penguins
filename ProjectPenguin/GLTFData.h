#pragma once

#include "tiny_gltf.h"

//This class uses all the data supplied by an accessor to turn its buffer data into something usable
class GLTFData
{
public:
	GLTFData(tinygltf::Model& model, tinygltf::Accessor& accessor);
	//REPLACE: try implementing this template function in cpp file to see if that actually makes a difference
	template<typename T>
	const T* GetElement(size_t index)
	{
		assert(data != nullptr);
		assert(sizeof(T) == elementSize);	//Specified type must be of same size as internal format
		assert(index < nElements);

		return reinterpret_cast<const T*>(reinterpret_cast<size_t>(data)
			+ index * emptySpace
			+ index * elementSize);
	}
private:
	size_t DataTypeSize();
private:
	tinygltf::Accessor& accessor;
	const unsigned char* data;	//Pointer to first byte of data
	size_t elementSize;
	size_t totalSize;	//Size of all elements combined in bytes
	size_t emptySpace;	//Empty space between elements
	size_t nElements;
};