#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <math.h>
#include <string.h>
#include <random>

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
	static const int boidCount = 300;
	Boid boids[boidCount];
	//Boid b;
	float pi = 3.14159f;
	
	Example() {
		std::default_random_engine generate;
		std::uniform_int_distribution<int> width(0, ScreenWidth());
		std::uniform_int_distribution<int> height(0, ScreenHeight());
		std::uniform_real_distribution<float> angle(0, 2*pi);
		std::uniform_int_distribution<int> byte(0, 255);
		
		sAppName = "Boids";
		for (int i = 0; i < boidCount; i++) {
			boids[i] = Boid(width(generate), height(generate), (angle(generate)), 50.0f, olc::Pixel(0,byte(generate), byte(generate)));
		}
		//b = Boid(100.0f, 100.0f, 2*pi, 50.0f, olc::GREEN);
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		/* Controls for test boid
		// Steering
		// Turn left
		if (GetKey(olc::A).bHeld)
			b.angle -= 1.0f * fElapsedTime;
		// Turn Right
		else if (GetKey(olc::D).bHeld)
			b.angle += 1.0f * fElapsedTime;
		//if (b.angle > pi) b.angle = -pi;
		//if (b.angle < -pi) b.angle = pi;
		// Move
		if (GetKey(olc::W).bHeld) {
			b.position.x += sin(b.angle) * b.speed * fElapsedTime;
			b.position.y -= cos(b.angle) * b.speed * fElapsedTime;
		}
		*/
		Clear(olc::BLACK);

		// Boid Logic
		for (int i = 0; i < boidCount; i++) {
			// Avoid Other Boids
				
			// Go in the same direction as other boids
			{
				/* WIP: Follow Mouse */
				//if (isInDistance(boids[i].position, GetMouseX(), GetMouseY(), 30)) {
					//float angle = atan2(boids[i].position.y - GetMouseY(), boids[i].position.x - GetMouseX()) + (pi / 2);
					//DrawString({ 0,0 }, "Angle: " + std::to_string(angle), olc::GREEN);
					//DrawString({ GetMouseX(),GetMouseY() }, "Pos: " + std::to_string(GetMouseX()) + "," + std::to_string(GetMouseY()), olc::GREEN);
					//boids[i].angle += Align(b.angle, angle) * fElapsedTime;
				//}

				//if (isInDistance(boids[i].position, b.position)) {
				//	boids[i].angle += Align(b.angle, boids[i].angle) * fElapsedTime;// * 10;
				//}

				for (int ii = 0; ii < boidCount; ii++) {
					if (i == ii) continue;
					if (!isInDistance(boids[i].position, boids[ii].position)) continue;
					boids[i].angle += Align(boids[ii].angle, boids[i].angle) * fElapsedTime;
				}
			}
			// Fly in the center of other boids
			
			// Update boid collection position
			boids[i].position.x += sin(boids[i].angle) * boids[i].speed * fElapsedTime;
			boids[i].position.y -= cos(boids[i].angle) * boids[i].speed * fElapsedTime;
			SetBoundry(boids[i].position.x, boids[i].position.y);
			DrawBoid(boids[i]);
		}
		// Update main boid position
		//SetBoundry(b.position.x, b.position.y);
		//DrawBoid(b);
		return true;
	}

	void DrawBoid(Boid& b, bool showRay = false) {
		// Initial points
		float x[4] = { 0.0f, -5.0f, 5.0f, 0.0f},
			  y[4] = { -10.0f, 5.0f, 5.0f, -20.0f};
		// Transformed points
		float tx[4], ty[4];
		// Rotate and Translate
		for (int i = 0; i < 4; i++) {
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
		if (showRay) DrawLine(tx[0], ty[0], tx[3], ty[3] , b.colour);
		
	}

	void SetBoundry(float& x, float& y) {
		if (x >= ScreenWidth())	
			x = 0;
		else if (x < 0)				
			x = ScreenWidth();
		if (y >= ScreenHeight())
			y = 0;
		else if (y < 0)				
			y = ScreenHeight();
	}
	
	int Align(float& angleA, float& angleB) {
		if (angleA > angleB)
			return 1;
		if (angleA < angleB)
			return -1;
		return 0;
	}

	bool isInDistance(olc::v2d_generic<float>& a, int x, int y, int distance = 15) {
		float deltaX = abs(a.x - x);
		float deltaY = abs(a.y - y);
		return (deltaX <= distance && deltaY <= distance) ? true : false;
	}

	bool isInDistance(olc::v2d_generic<float>& a, olc::v2d_generic<float>& b, int distance = 15) {
		float deltaX = abs(a.x - b.x);
		float deltaY = abs(a.y - b.y);
		return (deltaX <= distance && deltaY <= distance) ? true : false;
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
	//if (demo.Construct(1920, 1080, 4, 4))
	//if (demo.Construct(480, 270, 4, 4))
		demo.Start();
	return 0;
}
