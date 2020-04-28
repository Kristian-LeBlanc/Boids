#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "olcPixelGameEngine.h"
#include <math.h>

class Boid {
	public:
		olc::vf2d position;
		olc::vf2d velocity;
		olc::vf2d acceleration;
		olc::Pixel colour;
		const float maxSpeed = 50.0f;
		const float maxForce = 0.5f;
		float angle;
		Boid();
		Boid(olc::vf2d position, float angle, olc::Pixel colour);
		void update(float f);
		void applyForce(olc::vf2d force);
		olc::vf2d seek(olc::vi2d target);
		olc::vf2d steer(olc::vi2d desired);
	private:
		float calcAngle();
};