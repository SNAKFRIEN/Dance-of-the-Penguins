#include "GLTFData.h"

GLTFData::GLTFData(tinygltf::Model& model, tinygltf::Accessor& accessor)
	:
	accessor(accessor)
{
	tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
	tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

	data = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
	elementSize = DataTypeSize();
	totalSize = accessor.count * elementSize;
	emptySpace = bufferView.byteStride == 0 ? 0 : bufferView.byteStride - elementSize;
	nElements = accessor.count;
}

size_t GLTFData::DataTypeSize()
{
	uint32_t size = 0;

	//Get single element data type size
	switch (accessor.componentType)
	{
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		size = 1;
		break;
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		size = 2;
		break;
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		size = 4;
		break;
	}

	//Based on what the data is representing,
	//multiply
	switch (accessor.type)
	{
	case TINYGLTF_TYPE_MAT2:
		return 4 * size;
		break;
	case TINYGLTF_TYPE_MAT3:
		return 9 * size;
		break;
	case TINYGLTF_TYPE_MAT4:
		return 16 * size;
		break;
	case TINYGLTF_TYPE_SCALAR:
		return size;
		break;
	case TINYGLTF_TYPE_VEC2:
		return 2 * size;
		break;
	case TINYGLTF_TYPE_VEC3:
		return 3 * size;
		break;
	case TINYGLTF_TYPE_VEC4:
		return 4 * size;
		break;
	}

	return 0;
}
