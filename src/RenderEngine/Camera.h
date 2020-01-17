#pragma once
#include "DataStructures/Linear_Algebra.h"

//
//	Camera class and camera transfomations
//


namespace CRAB {

	struct float2 {
		float x, y;
	};


	struct Camera
	{

		Vector4Df position;
		Vector4Df view;
		Vector4Df up;

		float2 resolution;
		float2 dimensions;

		float n;//near plane

		Camera() {

		}

		Camera(const Vector4Df &_position, const Vector4Df &_view, const Vector4Df &_up) : 
			position(_position), view(_view), up(_up) {
			resolution = { 512.0f, 512.0f };
			dimensions = float2{ 1.0f, 1.0f };
			n = 1.0f;
			up.normalized();
		}

		Camera(const Vector4Df &_position, const Vector4Df &_view, const Vector4Df &_up, const float2 &_resolution,
			const float2 &_dimensions, const float _n) :
			position(_position), view(_view), up(_up), resolution(_resolution), dimensions(_dimensions), n(_n) {
			up.normalized();
		}

		void Transform(const Matrix4 &m)
		{
			view = m * view;
			up = m * up;
			position = m * position;
		}

		void Transform(const Matrix4 &m, const Vector4Df reference)
		{
			Transform(translate(Vector4Df{ -reference.x, -reference.y, -reference.z, 1.0f }));

			Transform(m);

			Transform(translate(Vector4Df{ reference.x, reference.y, reference.z, 1.0f }));
		}

	};


	inline Matrix4 ToWorld(const Vector4Df &position, const Vector4Df &lookat, const Vector4Df &vup) {
		//const Vector4Df &z = (lookat - position).to_unitary();
		//const Vector4Df &x = cross(vup, z).to_unitary();
		//const Vector4Df &y = cross(z, x);
		const Vector4Df& z = (position - lookat).normalized();
		const Vector4Df& x = cross(vup, z).normalized();
		const Vector4Df& y = cross(z, x);

		return Matrix4{
			x.x, y.x, z.x, position.x,
					  
			x.y, y.y, z.y, position.y,
					  
			x.z, y.z, z.z, position.z,

			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}
	inline Matrix4 ToWorld(const Camera &cam) {
		return ToWorld(cam.position, cam.view, cam.up);
	}
	inline Matrix4 ToCamera(const Vector4Df &position, const Vector4Df &lookat, const Vector4Df &vup) {
		//const Vector4Df &z = (lookat - position).to_unitary();
		//const Vector4Df &x = cross(vup, z).to_unitary();
		//const Vector4Df &y = cross(z, x);
		
		const Vector4Df& z = (position - lookat).normalized();
		const Vector4Df& x = cross(vup, z).normalized();
		const Vector4Df& y = cross(z, x);

		return Matrix4{
			x.x, x.y, x.z, -dot(x, position),

			y.x, y.y, y.z, -dot(y, position),

			z.x, z.y, z.z, -dot(z, position),

			0.0f, 0.0f, 0.0f, 1.0f,
		};
	}
	inline Matrix4 ToCamera(const Camera &cam) {
		return ToCamera(cam.position, cam.view, cam.up);
	}

}
