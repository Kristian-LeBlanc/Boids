#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

class Boid {
public:
	olc::v2d_generic<float> position;
	float angle;
	float speed;
	olc::Pixel colour;
	Boid() {
		position = { 0.0f,0.0f };
		this->angle = 0.0f;
		this->speed = 0.0f;
		colour = olc::RED;
	}
	Boid(float x, float y, float angle, float speed, olc::Pixel colour) {
		position = {x,y};
		this->angle = angle;
		this->speed = speed;
		this->colour = colour;
	}
};

class Example : public olc::PixelGameEngine {
public:
	//Boid b;
	int boidCount = 20;
	Boid boids[20];
	int random = 0;
	Example()
	{
		sAppName = "Boids";
		for (int i = 0; i < boidCount; i++) {
			boids[i] = Boid(100.0f, 100.0f, (rand() % 360), 20.0f, olc::RED);
		}
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		/*
		// Steering
		// Turn left
			b.angle -= 1.0f * fElapsedTime;
		// Turn Right
			b.angle += 1.0f * fElapsedTime;
		// Move
			b.position.x += sin(b.angle) * b.speed * fElapsedTime;
			b.position.y -= cos(b.angle) * b.speed * fElapsedTime;
		*/
		
		Clear(olc::BLACK);

		// Boid Logic
		for (int i = 0; i < boidCount; i++) {
			boids[i].position.x += sin(boids[i].angle) * boids[i].speed * fElapsedTime;
			boids[i].position.y -= cos(boids[i].angle) * boids[i].speed * fElapsedTime;
			SetBoundry(boids[i].position.x, boids[i].position.y);
			DrawBoid(boids[i]);
		}

		// Print Example
		//DrawString({ 0,200 }, "r: " + std::to_string(b.angle), olc::GREEN);
		return true;
	}

	void DrawBoid(Boid b, bool showRay = false) {
		// Initial points
		float x[3] = { 0.0f, -5.0f, 5.0f, },
			  y[3] = { -10.0f, 5.0f, 5.0f, };
		// Transformed points
		float tx[3], ty[3];
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
		if (x > ScreenWidth())	x = 0;
		if (x < 0)				x = ScreenWidth();
		if (y > ScreenHeight())	y = 0;
		if (y < 0)				y = ScreenHeight();
	}
	
	virtual bool Draw(int32_t x, int32_t y, olc::Pixel p = olc::WHITE){
		float fx = x, fy = y;
		SetBoundry(fx, fy);
		return olc::PixelGameEngine::Draw(fx, fy, p);
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}
