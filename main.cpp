#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <time.h>

using namespace sf;
using namespace std;

class SnakeWorld {
	static const int width = 10;
	static const int height = 10;

private:
	int field[height][width];

public:
	static const int cellSize = 80;

	bool isDead = false;
	vector<vector<int>> body;

	SnakeWorld() {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				field[i][j] = 0;
			}
		}

		body.push_back(vector<int>{height/2, width/2});
		body.push_back(vector<int>{height/2, width/2+1});
		body.push_back(vector<int>{height/2, width/2+2});
	}

	bool hasFood() {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				if (field[i][j] == 2) return true;
			}
		}

		return false;
	}

	void placeFood() {
		while (true) {
			int foodX = rand() / ((RAND_MAX + 1u) / width);
			int foodY = rand() / ((RAND_MAX + 1u) / height);

			if (field[foodX][foodY] == 0) {
				field[foodX][foodY] = 2;
				break;
			}
		}
	}

	void update(Keyboard::Key dir = Keyboard::A) {
		int lastX = body.back()[0];
		int lastY = body.back()[1];

		for (vector<int>& cell : body) {
			field[cell[1]][cell[0]] = 0;
		}

		for (int i = body.size()-1; i > 0; --i) { // place every cell on next's pos
			body[i][0] = body[i-1][0];
			body[i][1] = body[i-1][1];
			field[body[i][1]][body[i][0]] = 1;
		}

		if (dir == Keyboard::A) { // place head based on dir
			if (body[0][0] - 1 > -1) {
				body[0][0] -= 1;
			}
			else {
				body[0][0] = width - 1;
			}
		}
		else if (dir == Keyboard::W) {
			if (body[0][1] - 1 > -1) {
				body[0][1] -= 1;
			}
			else {
				body[0][1] = height - 1;
			}
		}
		else if (dir == Keyboard::D) {
			if (body[0][0] + 1 < width) {
				body[0][0] += 1;
			}
			else {
				body[0][0] = 0;
			}
		}
		else if (dir == Keyboard::S) {
			if (body[0][1] + 1 < height) {
				body[0][1] += 1;
			}
			else {
				body[0][1] = 0;
			}
		}

		if (field[body[0][1]][body[0][0]] == 2) { // add cell if food
			body.push_back(vector<int>{lastX, lastY});
			field[lastY][lastX] = 1;
		}

		if (field[body[0][1]][body[0][0]] == 1 || (body[0][0] == lastX && body[0][1] == lastY)) { // check collision
			isDead = true;
		}

		field[body[0][1]][body[0][0]] = 1;
	}

	void draw(RenderWindow& window, Keyboard::Key dir) {
		// draw head
		CircleShape head = CircleShape();

		head.setRadius(cellSize / 2);
		if (isDead) head.setFillColor(Color::Color(100, 100, 100, 255));
		else head.setFillColor(Color::Color(0, 80, 0, 255));
		head.setPosition(body[0][0]*cellSize, body[0][1] * cellSize);

		window.draw(head);

		// draw neck
		RectangleShape neck = RectangleShape();
		neck.setFillColor(Color::Color(0, 80, 0, 255));
		neck.setPosition(cellSize * body[0][0], body[0][1] * cellSize);
		if (dir == Keyboard::A) {
			neck.setSize(Vector2f(cellSize/2, cellSize));
			neck.setPosition(cellSize * body[0][0] + cellSize/2, body[0][1] * cellSize);
		}
		else if (dir == Keyboard::W) {
			neck.setSize(Vector2f(cellSize, cellSize / 2));
			neck.setPosition(cellSize * body[0][0], body[0][1] * cellSize + cellSize / 2);
		}
		else if (dir == Keyboard::D) {
			neck.setSize(Vector2f(cellSize / 2, cellSize));
		}
		else if (dir == Keyboard::S) {
			neck.setSize(Vector2f(cellSize, cellSize / 2));
		}

		window.draw(neck);

		// draw body
		for (int i = 1; i < body.size(); ++i) {
			RectangleShape rec = RectangleShape();

			rec.setSize(Vector2f(cellSize, cellSize));
			if (isDead) rec.setFillColor(Color::Color(100, 100, 100, 255));
			else rec.setFillColor(Color::Color(0, 80, 0, 255));
			rec.setPosition(body[i][0] * cellSize, body[i][1] * cellSize);

			window.draw(rec);
		}

		// draw food
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (field[i][j] == 2) {
					CircleShape cir = CircleShape();

					cir.setRadius(cellSize/2);
					cir.setFillColor(Color::Red);
					cir.setPosition(j * cellSize, i * cellSize);

					window.draw(cir);
				}
			}
		}
	}
};

int main() {
	srand(time(0));

	RenderWindow window(VideoMode(800, 800), "Snake");
	window.setFramerateLimit(6);

	SnakeWorld snakeWorld;
	Keyboard::Key movement = Keyboard::A; // default movement dir

	RectangleShape grass = RectangleShape(Vector2f(800, 800));
	grass.setFillColor(Color::Color(100, 230, 100, 200));
	grass.setPosition(0, 0);

	while (window.isOpen()) {

		if (!snakeWorld.isDead) window.clear();
		
		window.draw(grass);
		Event event;

		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) window.close();
			else if (event.type == Event::KeyPressed) { // save movement dir
				movement = event.key.code;
			}
		}

		if (!snakeWorld.isDead) snakeWorld.update(movement); // freeze if dead

		if (!snakeWorld.hasFood()) snakeWorld.placeFood();
		snakeWorld.draw(window, movement);
		window.display();
	}

	return 0;
}