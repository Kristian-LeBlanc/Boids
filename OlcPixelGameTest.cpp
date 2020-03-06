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
	static const int boidCount = 1;
	Boid boids[boidCount];
	Boid b;
	float pi = 3.14159f;
	float turnSpeed = pi / 2;
	
	Example() {
		std::default_random_engine generate;
		std::uniform_int_distribution<int> width(0, ScreenWidth());
		std::uniform_int_distribution<int> height(0, ScreenHeight());
		std::uniform_real_distribution<float> angle(0, 2*pi);
		std::uniform_int_distribution<int> byte(25, 255);
		
		sAppName = "Boids";
		for (int i = 0; i < boidCount; i++) {
			boids[i] = Boid(width(generate), height(generate), (angle(generate)), 50.0f, olc::Pixel(byte(generate), 0, byte(generate)));
		}
		b = Boid(100.0f, 100.0f, 2*pi, 50.0f, olc::GREEN);
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		/* Controls for test boid*/
		// Steering
		// Turn left
		if (GetKey(olc::A).bHeld)
			b.angle -= 1.0f * fElapsedTime;
		// Turn Right
		else if (GetKey(olc::D).bHeld)
			b.angle += 1.0f * fElapsedTime;
		// Move
		if (GetKey(olc::W).bHeld) {
			b.position.x += sin(b.angle) * b.speed * fElapsedTime;
			b.position.y -= cos(b.angle) * b.speed * fElapsedTime;
		}
		
		Clear(olc::BLACK);

		// Boid Logic
		for (int i = 0; i < boidCount; i++) {
			// Avoid Other Boids
			/*
			{
				for (int ii = 0; ii < boidCount; ii++) {
					if (isInDistance(boids[ii].position, boids[i].position, 20)) {
						float angle = findAngle({ boids[i].position.x, boids[i].position.y },
							{ boids[ii].position.x, boids[ii].position.y });
						if (angle < boids[i].angle) {
							boids[i].angle += -turnSpeed * fElapsedTime;
						}
						else if (angle > boids[i].angle) {
							boids[i].angle += turnSpeed * fElapsedTime;
						}
					}
				}
			}
			*/
			
			
			// Go in the same direction as other boids
			{
				//boids[i].angle += turnSpeed * Align(boids[i].angle, boids[ii].angle) * fElapsedTime;
				float angle = findAngle({ boids[i].position.x, boids[i].position.y }, { (float)GetMouseX(),(float)GetMouseY() });
				
				/*while (true) {
					float delta = angle - boids[i].angle;
					if (abs(delta) > pi)	break;
					if (delta > 0)			angle += pi;
					if (delta < 0)			angle -= pi;
					else break;
				}*/
				
				//DrawString({ 0,0 }, std::to_string(angle - boids[i].angle));
				//DrawString({ (int)boids[i].position.x,(int)boids[i].position.y + 10 }, "cAngle: " + std::to_string(angle), boids[i].colour);
				boids[i].angle += turnSpeed * Align(boids[i].angle, angle) * fElapsedTime;
				
				/*for (int ii = 0; ii < boidCount; ii++) {
					if (i == ii) continue;
					if (!isInDistance(boids[i].position, boids[ii].position, 20)) continue;
					boids[i].angle += turnSpeed * Align(boids[i].angle, boids[ii].angle) * fElapsedTime;
				}*/
			}
			
			// Fly in the center of other boids
			/*
			{
				if (boidCount == 1) continue;
				float avgX = 0;
				float avgY = 0;
				for (int ii = 0; ii < boidCount; ii++) {
					if (i == ii) continue;
					if (!isInDistance(boids[i].position, boids[ii].position, 20)) continue;
					avgX += boids[ii].position.x;
					avgY += boids[ii].position.y;
					//boids[i].angle += turnSpeed * Align(boids[i].angle, boids[ii].angle) * fElapsedTime;
				}
				avgX = avgX / boidCount - 1;
				avgX = avgY / boidCount - 1;
				float angle = findAngle({ boids[i].position.x,boids[i].position.y }, { avgX,avgY });
				if (angle < boids[i].angle) {
					boids[i].angle += -turnSpeed * fElapsedTime;
				}
				else if (angle > boids[i].angle) {
					boids[i].angle += turnSpeed * fElapsedTime;
				}
			}
			*/
			
			// Update boid collection position
			//boids[i].position.x += sin(boids[i].angle) * boids[i].speed * fElapsedTime;
			//boids[i].position.y -= cos(boids[i].angle) * boids[i].speed * fElapsedTime;
			SetBoundry(boids[i].position.x, boids[i].position.y);
			DrawBoid(boids[i]);
			DrawString({ (int)boids[i].position.x,(int)boids[i].position.y }, "Angle: " + std::to_string(boids[i].angle),boids[i].colour);
		}
		// Update main boid position
		SetBoundry(b.position.x, b.position.y);
		//DrawBoid(b);
		//DrawString({ (int)b.position.x,(int)b.position.y }, "Angle: " + std::to_string(b.angle),b.colour);
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

	float findAngle(olc::vd2d pointA, olc::vd2d pointB) {
		return atan2(pointB.x - pointA.x, pointA.y - pointB.y);
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
		/*float deltaAng = angleB - angleA;
		float d = abs(-angleA + (angleB - (2 * pi)));
		DrawString({ 0,0 }, std::to_string(deltaAng));
		DrawString({ 0,10 }, std::to_string(d));*/
		if (angleB > angleA)
			return 1;
		if (angleB < angleA)
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
		return olc::PixelGameEngine::Draw((int)fx, (int)fy, p);
	}

	
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
	//if (demo.Construct(1920/4, 1080/4, 4, 4))
	//if (demo.Construct(480, 270, 4, 4))
		demo.Start();
	return 0;
}
