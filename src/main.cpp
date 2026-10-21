#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>

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

	//Load ground sprite
	sf::Texture groundTexture;
    if (!groundTexture.loadFromFile("src/sprites/Ground.png"))
		return -1;
	//Load question sprite
	sf::Texture questionTexture;
	if (!questionTexture.loadFromFile("src/sprites/Question.png"))
		return -1;
	//Load brick texture
	sf::Texture brickTexture;
	if (!brickTexture.loadFromFile("src/sprites/Brick.png"))
		return -1;
	//Load sky texture
	sf::Texture skyTexture;
	if (!skyTexture.loadFromFile("src/sprites/Sky.png"))
		return -1;

	// Load level
	std::ifstream levelFile("src/levels/level1.txt");
	if (!levelFile.is_open()) {
		// Handle error: file failed to open
	}

	std::vector<sf::Sprite> tiles;
	std::string lineText;

	float tileXOffset = 0.f;
	float tileYOffset = 0.f;

	while (std::getline(levelFile, lineText)) {
		// Strip trailing carriage return '\r' if present (Windows-style line endings)
		if (!lineText.empty() && lineText.back() == '\r') {
			lineText.pop_back();
		}

		int lineLength = lineText.length();
		for (int i = 0; i < lineLength; i++) {
			// Pick the correct texture based on the character
			const sf::Texture* currentTexture = &skyTexture;

			switch (lineText[i]) {
				case 'q':
					currentTexture = &questionTexture;
				break;
				case 'b':
					currentTexture = &brickTexture;
				break;
				case 'g':
					currentTexture = &groundTexture;
				break;
				default:
					currentTexture = &skyTexture;
				break;
			}

			sf::Sprite s(*currentTexture);
			s.setPosition(sf::Vector2f(tileXOffset, tileYOffset));
			tiles.push_back(s);

			tileXOffset += 48.f;
		}
		tileXOffset = 0.f;
		tileYOffset += 48.f;
	}

	// Close the file
	levelFile.close();

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

		for (unsigned i = 0; i < tiles.size(); i++) {
			window.draw(tiles[i]);
		}

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
		//cout<<"Mouse Position: "<<dogTargetPos.x<<" "<<dogTargetPos.y<<endl;
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
