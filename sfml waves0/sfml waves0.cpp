#include <chrono>
#include <thread>

#include <SFML/Graphics.hpp>

using namespace sf;
using std::this_thread::sleep_for;

const int Width = 200;
const int Height = 300;

const double Seconds_per_frame = 0.1;
const double rigidity_coef = 1.1;

const double Y_scale = 10;

//r = (self.c * dt/np.gradient(self.x))**2
const double r = 0.5;


/*
class Point {
private:
	int x;
	
	double y_change;
public:
	double y;
	double y_speed;

	Point(int x = -666, double y_init = 0.0, double speed = 0.0) {
		this->x = x;
		this->y = y_init;
		this->y_speed = speed;
		this->y_change = 0;
		this->y_speed = 0;
	}
	~Point() {};

	void SetX(int x) {
		this->x = x;
	}

	int GetX() {
		return this->x;
	}

	//for ordinary points with 2 neighbors
	void Compute(const Point &prev_point, const Point& next_point) {
		this->y_change = y_speed * Seconds_per_frame;
		this->y_speed += (prev_point.y + next_point.y - 2 * this->y) * rigidity_coef * Seconds_per_frame;

		//y_speed *= 0.99;
	}

	//for boundary points with 1 neighbor
	void Compute(const Point& point) {
		this->y_change = y_speed * Seconds_per_frame;
		this->y_speed += (point.y - this->y) * rigidity_coef * Seconds_per_frame;
	}

	//for one oscillating point
	void Compute(int y0) {
		this->y_change = y_speed * Seconds_per_frame;
		this->y_speed -= (this->y - y0) * rigidity_coef * Seconds_per_frame;
	}

	void ApplyChanging() {
		this->y += this-> y_change;
		//this->y_change = 0;
	}
};

class Field {
public:
	Point points[Width];

	Field() {
		for (int i = 0; i < Width; i++) {
			points[i].SetX(i);
		}
	}
	~Field() {};

	void ComputeFrame() {
		
		for (int i = 1; i < Width - 1; i++) {
			points[i].Compute(points[i-1], points[i + 1]);
		}
		//points[0].Compute(points[1]);
		//points[Width - 1].Compute(points[Width - 2]);
		

		/*
		for (int i = 0; i < Width; i++) {
			points[i].Compute(0.0);
		}
		
		for (int i = 0; i < Width ; i++) {
			points[i].ApplyChanging();
		}
	}
};
*/

class Point {
private:
	int x;
	double y_change;
public:
	double y;
	double y_prev;

	Point(int x = -666, double y_init = 0.0) {
		this->x = x;
		this->y = y_init;
		this->y_prev = y_init;
	}
	~Point() {};

	void SetX(int x) {
		this->x = x;
	}

	int GetX() {
		return this->x;
	}

	//for ordinary points with 2 neighbors
	void Compute(const Point& prev_point, const Point& next_point) {
		this->y_change = 2 * this->y - this->y_prev + r * (prev_point.y + next_point.y - 2 * this->y);
		this->y_prev = this->y;
		//this->y_prev = this->y;
	}

	void ApplyChanging() {
		this->y = this->y_change;
		//this->y_change = 0;
	}
};

class Field {
public:
	Point points[Width];

	Field() {
		for (int i = 0; i < Width; i++) {
			points[i].SetX(i);
		}
	}
	~Field() {};

	void ComputeFrame() {

		for (int i = 1; i < Width - 1; i++) {
			points[i].Compute(points[i - 1], points[i + 1]);
		}
		//points[0].Compute(points[1]);
		//points[Width - 1].Compute(points[Width - 2]);

		for (int i = 1; i < Width - 1; i++) {
			points[i].ApplyChanging();
		}
	}
};

int main()
{
	Field field;

	for (int x = 0; x < Width; x++) {
		field.points[x].y = 0.0;
		field.points[x].y_prev = 0.0;
	}

	for (int x = 0; x < 200; x++) {
		field.points[x].y = -sin(x * 3.14 / 100) * 3.5;
		field.points[x].y_prev = -sin(x * 3.14 / 100) * 3.5;
	}

	//field.points[0].y_speed = 100.0;

	RenderWindow window(VideoMode(Width, Height), "Wave simulation");
	window.setFramerateLimit(60);


	Uint8* pixels = new Uint8[4 * Width * Height];

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		for (int x = 0; x < Width; x++) {
			for (int y = 0; y < Height; y++) {
				pixels[(x + y*Width) * 4] = 255; //r
				pixels[(x + y * Width) * 4 + 1] = 255; //g
				pixels[(x + y * Width) * 4 + 2] = 255; //b
				pixels[(x + y * Width) * 4 + 3] = 255; //a
			}
		}

		for (int x = 0; x < Width; x++) {
			int y = (int) (field.points[x].y * Y_scale) + Height/2;

			pixels[(x + y * Width) * 4] = 0;
			pixels[(x + y * Width) * 4 + 1] = 0;
			pixels[(x + y * Width) * 4 + 2] = 0;
			pixels[(x + y * Width) * 4 + 3] = 255;
		}

		Image img;
		img.create(Width, Height, pixels);
		Texture texture;
		texture.loadFromImage(img);
		Sprite sprite;
		sprite.setTexture(texture, true);
		window.draw(sprite);

		window.display();

		//sleep_for(std::chrono::microseconds((long long) Seconds_per_frame * 1000000));
		//sleep_for(std::chrono::milliseconds(100));
		field.ComputeFrame();
	}

	delete[] pixels;
	return 0;
}

