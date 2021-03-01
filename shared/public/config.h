#pragma once

#ifdef INKCPP_API
#define INK_ENABLE_UNREAL
#else
#define INK_ENABLE_STL
#define INK_ENABLE_CSTD
#endif

// Only turn on if you have json.hpp and you want to use it with the compiler
// #define INK_EXPOSE_JSON

namespace ink::config {
	/// set limitations which are required to minimize heap allocations.
	/// if required you can set them to -x then, the system will use dynamic
	/// allocation for this type, with an initial size of x.
	static constexpr int limitGlobalVariables = -1;
	static constexpr int limitThreadDepth = 20;
	static constexpr int limitEvalStackDepth = -1;
	static constexpr int limitContainerDepth = 20;
	// temporary variables and callstack;
	static constexpr int limitRuntimeStack = -1;
	// max number of elements in one output (a string is one element)
	static constexpr int limitOutputSize = 200;
	// max number of choices per choice
	static constexpr int maxChoices = 10;
}
