#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;

int main()
{
	//Render window
	unsigned screenWidth = 1920;
	unsigned screenHeight = 1080;
	sf::RenderWindow window( sf::VideoMode( { screenWidth, screenHeight } ), "Cat And Dog" );

	//Start clock
	sf::Clock clock;

	//Add cat texture
	sf::Texture catTexture;
	if (!catTexture.loadFromFile("src/sprites/cat.png"))
		return -1;
	sf::Sprite catSprite(catTexture);
	catSprite.setPosition({50, 200});

	//Add dog texture
	sf::Texture dogTexture;
	if (!dogTexture.loadFromFile("src/sprites/dog.png"))
		return -1;
	sf::Sprite dogSprite(dogTexture);
	dogSprite.setPosition({525, 200});

	float catMaxSpeed = 1.f;
	float smooth = 0.9f;
	float deceleration = 10;

	float catSpeed = 0;
	sf::Vector2f catInput;
	sf::Vector2f catVelocity;

	float dogSpeed = 0;
	float dogMaxSpeed = 500.f;
	sf::Vector2f dogInput;
	sf::Vector2f dogVelocity;

	while ( window.isOpen() )
	{
		sf::Time elapsed = clock.restart();
		float deltaTime = elapsed.asSeconds();

		while ( const std::optional event = window.pollEvent() )
		{
			if ( event->is<sf::Event::Closed>() )
				window.close();
		}

		//Draw
		window.clear(sf::Color::Magenta);
		window.draw(catSprite);
		window.draw(dogSprite);
		window.display();

		///Cat
		//Get keyboard input
		catInput.x = 0;
		catInput.y = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {

			catInput += sf::Vector2f(-1, 0);
			cout<<"Left key pressed"<<endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
			catInput += sf::Vector2f(1, 0);
			cout<<"Right key pressed"<<endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
			catInput += sf::Vector2f(0, 1);
			cout<<"Down key pressed"<<endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
			catInput += sf::Vector2f(0, -1);
			cout<<"Up key pressed"<<endl;
		}
		//Acceleration
		catSpeed = catMaxSpeed * (1 - smooth) + catSpeed * smooth;
		catVelocity += catSpeed * catInput;
		catSprite.move(catVelocity * deltaTime);
		//Deceleration
		if (catInput == sf::Vector2f(0, 0)) {
			catVelocity -= (1-smooth)*deceleration*catVelocity*deltaTime;
		}

		///Dog
		sf::Vector2f dogTargetPos = (sf::Vector2f)sf::Mouse::getPosition(window) - sf::Vector2f(110, 110);
		cout<<"Mouse Position: "<<dogTargetPos.x<<" "<<dogTargetPos.y<<endl;
		sf::Vector2f dogDirection = dogTargetPos - dogSprite.getPosition();

		float distance = std::sqrt(dogDirection.x * dogDirection.x + dogDirection.y * dogDirection.y);
		if (distance > 0.001f) {
			dogInput = dogDirection / distance;
		}

		//Slowdown when near mouse
		float slowdownRadius = 200.f;
		float targetSpeed = dogMaxSpeed;

		if (distance < slowdownRadius) {
			targetSpeed = dogMaxSpeed * (distance / slowdownRadius);
		}

		if (distance < 2.f) {
			targetSpeed = 0.f;
		}
		//Dog acceleration
		sf::Vector2f targetVelocity = dogInput * targetSpeed;
		dogVelocity = targetVelocity * (1.f - smooth) + dogVelocity * smooth;
		dogSprite.move(dogVelocity * deltaTime);
	}
}
