#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Boid {
public:
	olc::v2d_generic<int> position;
	int angle;
	int speed;
	Boid() {
		position = { 0,0 };
		this->angle = 0;
		this->speed = 0;
	}
	Boid(int x, int y, int angle, int speed) {
		position = {x,y};
		this->angle = angle;
		this->speed = speed;
	}
};

class Example : public olc::PixelGameEngine
{
public:
	Boid b;
	float time = 0.0f;
	Example()
	{
		sAppName = "Example";
		b = Boid(100, 100, 0, 2);
	}

	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		time += fElapsedTime;
		if (time > 0.0166f) { // update every once every 60th of a second.
			time = 0;
			b.position.y -= b.speed;
			if (b.position.y > 240) 
				b.position.y = 0;
			if (b.position.y < 0) 
				b.position.y = 240;
		}
		
		// called once per frame
		clearScreen(olc::BLACK);
		drawBoid(b);
		return true;
	}

	void clearScreen(olc::Pixel p) {
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, p);
	};

	void drawBoid(Boid b, bool showRay = false) {
		int x = b.position.x;
		int y = b.position.y;
		DrawTriangle(x, y, x-5, y+10, x+5, y+10, olc::RED);
		Draw(x, y, olc::RED);
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 1, 1))
		demo.Start();
	return 0;
}