#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

class Ball;
sf::Texture dogTexture;
float deltaTime;
unsigned int windowWidth = 1920;
unsigned int windowHeight = 1080;
int ballNumber = 10;
float ballMargin = 300.f;
std::vector<Ball> ballList;
bool separation = true;
bool bouncing = true;

class Ball {
	sf::Vector2f velocity;
	sf::Sprite ballSprite;
	float ballDiameter;
	public:
	Ball(float x, float y): ballSprite(dogTexture) {
		const float speed = 500.f;
		float angle = (rand() / (float)RAND_MAX) * 2.f * 3.14159265f;
		velocity = sf::Vector2f(std::cos(angle), std::sin(angle)) * speed;
		ballDiameter = dogTexture.getSize().x;
		ballSprite.setPosition({x,y});
	}
	void Step() {
		ballSprite.move(velocity * deltaTime);

		if (ballSprite.getPosition().x < 0)
		{
			velocity.x = -velocity.x;
			ballSprite.setPosition({0,GetPos().y});
		}
		if (ballSprite.getPosition().x+ballDiameter > windowWidth)
		{
			velocity.x = -velocity.x;
			ballSprite.setPosition({(float)windowWidth-ballDiameter,GetPos().y});
		}
		if (ballSprite.getPosition().y < 0)
		{
			velocity.y = -velocity.y;
			ballSprite.setPosition({GetPos().x,0});
		}
		if (ballSprite.getPosition().y+ballDiameter > windowHeight)
		{
			velocity.y = -velocity.y;
			ballSprite.setPosition({GetPos().x,(float)windowHeight-ballDiameter});
		}
	}
	sf::Sprite GetSprite() {
		return ballSprite;
	}
	sf::Vector2f GetPos() {
		return ballSprite.getPosition();
	}
	void SetVel(sf::Vector2f vel) {
		velocity = vel;
	}
	sf::Vector2f GetVel() {
		return velocity;
	}
	void Collide(Ball &ball) {
		sf::Vector2f diff = ball.GetPos() - GetPos();
		float dist = diff.length();
		if (dist >= ball.ballDiameter || dist == 0) {return;}
		sf::Vector2f n = diff / dist;
		float s = (GetVel() - ball.GetVel()).dot(n);
		if (s <= 0) {
			return;
		}
		SetVel(GetVel() - (s*n));
		ball.SetVel(GetVel() + (s*n));
	}
	void Separate(Ball &ball) {
		sf::Vector2f diff = ball.GetPos() - GetPos();
		float dist = diff.length();
		if (dist >= ballDiameter || dist == 0.f) return;

		sf::Vector2f n = diff / dist;
		float overlap = ballDiameter - dist;

		ballSprite.move(-n * (overlap / 2.f));
		ball.ballSprite.move(n * (overlap / 2.f));
	}
};

int main()
{
	srand(time(0));
	//Render window
	sf::RenderWindow window( sf::VideoMode( { windowWidth, windowHeight } ), "Balls" );

	//Start clock
	sf::Clock clock;

	//Add dog texture
	if (!dogTexture.loadFromFile("src/sprites/dog.png"))
		return -1;

	for (int i = 0; i < ballNumber; i++) {
		float startX = 0;
		float startY = 0;

		while (startX < ballMargin || startX > windowWidth-ballMargin || startY < ballMargin || startY > windowHeight-ballMargin) {
			startX = rand()%windowWidth;
			startY = rand()%windowHeight;
		}
		ballList.push_back(Ball(rand()%windowWidth,rand()%windowHeight));
	}

	while ( window.isOpen() )
	{

		if (isKeyPressed(sf::Keyboard::Key::Q)) {
			separation = !separation;
		}
		if (isKeyPressed(sf::Keyboard::Key::E)) {
			bouncing = !bouncing;
		}
		sf::Time elapsed = clock.restart();
		deltaTime = elapsed.asSeconds();

		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

		//Draw
		window.clear(sf::Color::Black);

		for (int i = 0; i < ballNumber; i++) {
			for (int i = 0; i < ballNumber; i++)
				for (int j = i + 1; j < ballNumber; j++) {
					if (separation)
						ballList[i].Separate(ballList[j]);
					if (bouncing)
						ballList[i].Collide(ballList[j]);
				}
			ballList[i].Step();
			window.draw(ballList[i].GetSprite());
		}

		window.display();
	}
}