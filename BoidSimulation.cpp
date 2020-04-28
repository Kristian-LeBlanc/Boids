// OLC Pixel Game Engine
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
// My Includes
#include "boid.h"
// Standerd Libray
#include <vector>
#include <math.h>
#include <random>

class Example : public olc::PixelGameEngine {
public:
	std::vector<Boid> flock;
	float pi = 3.14159f;
	bool applyAlignment;
	bool applySeperation;
	bool applyCohesion;
	bool chaseMouse;

	bool OnUserCreate() override {
		sAppName = "Boids";
		int boidCount = 100;
		flock.reserve(boidCount);
		
		applyAlignment = false;
		applySeperation = false;
		applyCohesion = false;
		chaseMouse = false;

		// Set RNG
		std::default_random_engine generate;
		std::uniform_int_distribution<int> width(0, ScreenWidth());
		std::uniform_int_distribution<int> height(0, ScreenHeight());
		std::uniform_real_distribution<float> angle(-pi, pi);
		std::uniform_int_distribution<int> byte(25, 255);

		// Populate Flock
		for (int i = 0; i < boidCount; i++) {
			olc::vf2d pos = { (float)width(generate), (float)height(generate) };
			olc::Pixel colour = olc::Pixel(byte(generate), byte(generate), byte(generate));
			Boid b(pos, angle(generate), colour);
			flock.push_back(b);
		}
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		// User Input
		if (GetKey(olc::Q).bPressed)	return false;
		if (GetKey(olc::Z).bPressed)	applyAlignment = !applyAlignment;
		if (GetKey(olc::X).bPressed)	applySeperation = !applySeperation;
		if (GetKey(olc::C).bPressed)	applyCohesion = !applyCohesion;
		if (GetKey(olc::V).bPressed)	chaseMouse = !chaseMouse;
		
		Clear(olc::BLACK);
		olc::vf2d mouse = { (float)GetMouseX(),(float)GetMouseY() };
		
		// Boid Logic
		for (int i = 0; i < flock.size(); i++) {
			//Mouse Track
			if (chaseMouse) {
				olc::vf2d m = flock[i].seek(mouse) *= 2;
				flock[i].applyForce(m);
				Draw(0, 3);
			}
			
			// Alignment
			if (applyAlignment) {
				olc::vf2d a = Align(flock, i) *= 0.85;
				flock[i].applyForce(a);
				Draw(0, 0, olc::RED);
			}
			
			// Seperation
			if (applySeperation) {
				olc::vf2d s = Seperate(flock, i) *= 1.5;
				flock[i].applyForce(s);
				Draw(0, 1, olc::GREEN);
			}
			
			// Cohesion
			if (applyCohesion) {
				olc::vf2d c = Cohesion(flock, i) *= 0.85;
				flock[i].applyForce(c);
				Draw(0, 2, olc::BLUE);
			}
			
			// Update boid collection position
			flock[i].update(fElapsedTime);
			SetBoundry(flock[i].position.x, flock[i].position.y);
			DrawBoid(flock[i]);
		}
		return true;
	}

	olc::vf2d Align(std::vector<Boid> flock, int current) {
		// Average Velocity
		olc::vf2d average = { 0,0 };
		int total = 0;
		for (int i = 0; i < flock.size(); i++) {
			if (i == current) continue;
			if (isInDistance(flock[current].position, flock[i].position)) {
				average += flock[i].velocity;
				total++;
			}
		}
		if (total != 0) average /= total;
		// Set Magnitude
		if (average.mag() != 0) {
			average = average.norm();
			average *= flock[current].maxSpeed;
		}
		return flock[current].steer(average);
	}

	olc::vf2d Seperate(std::vector<Boid> flock, int current) {
		// Average Position
		olc::vf2d sum = { 0,0 };
		int total = 0;
		for (int i = 0; i < flock.size(); i++) {
			if (i == current) continue;
			if (isInDistance(flock[current].position, flock[i].position)) {
				float dist = Distance(flock[i].position, flock[current].position);
				if (0 < dist) {
					olc::vf2d diff =  flock[current].position - flock[i].position;
					diff /= dist;
					sum += diff;
					total++;
				}
			}
		}
		if (total != 0) sum /= total;
		// Set Magnitude
		if (sum.mag() != 0) {
			sum = sum.norm();
			sum *= flock[current].maxSpeed;
		}
		return flock[current].steer(sum);
		
	}
	
	olc::vf2d Cohesion(std::vector<Boid> flock, int current) {
		// Average Position
		olc::vf2d sum = { 0,0 };
		int total = 0;
		for (int i = 0; i < flock.size(); i++) {
			if (i == current) continue;
			if (isInDistance(flock[current].position, flock[i].position)) {
				float dist = Distance(flock[i].position, flock[current].position);
				if (0 < dist) {
					olc::vf2d diff = flock[i].position - flock[current].position;
					diff /= dist;
					sum += diff;
					total++;
				}
			}
		}
		if (total != 0) sum /= total;
		// Set Magnitude
		if (sum.mag() != 0) {
			sum = sum.norm();
			sum *= flock[current].maxSpeed;
		}
		return flock[current].steer(sum);
	}

	void DrawBoid(Boid& b) {
		// Initial points
		float x[4] = { 0.0f, -5.0f, 5.0f},
			  y[4] = { -10.0f, 5.0f, 5.0f};
		// Transformed points
		float tx[4], ty[4];
		
		// Rotate and Translate
		for (int i = 0; i < 3; i++) {
			tx[i] = x[i] * cos(b.angle) - y[i] * sin(b.angle);
			tx[i] += b.position.x;
			ty[i] = y[i] * cos(b.angle) + x[i] * sin(b.angle);
			ty[i] += b.position.y;
		}
		
		// Draw
		DrawTriangle(
			tx[0], ty[0],
			tx[1], ty[1],
			tx[2], ty[2],b.colour);
	}

	void SetBoundry(float& x, float& y) {
		if (x >= ScreenWidth())		x -= ScreenWidth();
		if (x < 0)					x += ScreenWidth();
		if (y >= ScreenHeight())	y -= ScreenHeight();
		if (y < 0)					y += ScreenHeight();
	}
	
	float Distance(olc::vf2d& a, olc::vf2d& b) {
		return abs(sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y),2)));
	}

	bool isInDistance(olc::vf2d& a, olc::vf2d& b, int distance = 50) {
		return Distance(a, b) < distance;
	}

	virtual bool Draw(int32_t x, int32_t y, olc::Pixel p = olc::WHITE){
		float fx = x, fy = y;
		SetBoundry(fx, fy);
		return olc::PixelGameEngine::Draw((int)fx, (int)fy, p);
	}
};

int main()
{
	Example demo;
	int scale = 4;
	if (demo.Construct(1920/scale, 1080/scale, scale,scale, true))	demo.Start();
	return 0;
}