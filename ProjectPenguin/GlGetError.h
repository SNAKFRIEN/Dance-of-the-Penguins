#pragma once

//REMOVE use the proper error check method
#define GL_ERROR_CHECK()\
{\
	int error = glGetError();\
	if (error != GL_NO_ERROR)\
	{\
		std::stringstream errorMessage;\
		errorMessage << "GL error: 0x" << std::hex << error << "\n" << __FILE__ << " " << std::dec <<__LINE__;\
		throw std::exception(errorMessage.str().c_str());\
	}\
}