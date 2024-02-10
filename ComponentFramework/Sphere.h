#ifndef SPHERE_H
#define SPHERE_H
#include <iostream>
namespace  MATH {
	struct Sphere : public Vec3 {
		float r;

		/// Just a little utility to populate a Shere
		inline void set(float x_, float y_, float z_, float r_) {
			x = x_; y = y_; z = z_, r = r_;
		}

		explicit Sphere(float s = 0.0f) { 
			x = s; y = s; z = s; r = s; 
		}

		Sphere(float x, float y, float z, float r) {
			set(x, y, z, r);
		}

		Sphere(const Sphere& s) {
			set(s.x, s.y, s.z, s.r);
		}

		void print() {
			printf("%f %f %f %f\n", x, y, z, r);
		}
	};


	

}
#endif