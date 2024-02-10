#ifndef HASH_H
#define HASH_H
#include <functional>
#include "Vector.h"

namespace MATH {
	static void combineHashes(size_t& seed, size_t hash) {
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}

	/*inline  bool equal_to (Vec2 const& v1, Vec2 const& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y);
	}

	inline  bool equal_to (Vec3 const& v1, Vec3 const& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
	}

	inline  bool equal_to (Vec4 const& v1, Vec4 const& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
	}*/

	inline  bool operator == (Vec2 const& v1, Vec2 const& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y);
	}

	inline  bool operator == (Vec3 const& v1, Vec3 const& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
	}

	inline  bool operator == (Vec4 const& v1, Vec4 const& v2) {
		return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
	}
}




/// Custom specializations of std::hash will be injected into namespace std
namespace std {
	using namespace MATH;

	template<> struct hash<Vec2> {
		size_t operator() ( Vec2 const& v) const ;
	};

	template<> struct hash<Vec3> {
		size_t operator() ( Vec3 const& v) const ;
	};

	template<> struct hash<Vec4> {
		size_t operator() (Vec4 const& v) const ;
	};
}


#endif 