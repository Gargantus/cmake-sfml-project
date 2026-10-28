#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>

using namespace std;

float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

int main()
{
	//Render window
	unsigned screenWidth = 1920;
	unsigned screenHeight = 1080;
	sf::RenderWindow window( sf::VideoMode( { screenWidth, screenHeight } ), "Mario" );

	//Start clock
	sf::Clock clock;

	//Add mario texture
	sf::Texture marioTexture;
	if (!marioTexture.loadFromFile("src/sprites/Mario.png"))
		return -1;
	sf::Sprite marioSprite(marioTexture);
	marioSprite.setPosition({100, 800});

	float marioMaxSpeed = 10.f;
	float smooth = 0.2f;
	float deceleration = 10;
	float cameraXSpeed = 0;

	float marioSpeed = 0;
	sf::Vector2f marioInput;
	sf::Vector2f marioVelocity;

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
	//Plik poziomu wygenerowany z pomocą AI!
	std::ifstream levelFile("src/levels/level2.txt");

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

	//Set camera
	float cameraX = 960.f;
	float cameraY = 540.f;
	float pushWindow = 300.f;
	sf::View view({cameraX, cameraY}, {cameraX*2, cameraY*2});
	window.setView(view);

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

		for (unsigned i = 0; i < tiles.size(); i++) {
			window.draw(tiles[i]);
		}

		window.draw(marioSprite);

		window.display();

		///Mario
		//Get keyboard input
		marioInput.x = 0;
		marioInput.y = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {

			marioInput += sf::Vector2f(-1, 0);
			cout<<"Left key pressed"<<endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
			marioInput += sf::Vector2f(1, 0);
			cout<<"Right key pressed"<<endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
			marioInput += sf::Vector2f(0, 1);
			cout<<"Down key pressed"<<endl;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
			marioInput += sf::Vector2f(0, -1);
			cout<<"Up key pressed"<<endl;
		}
		//Acceleration
		marioSpeed = marioMaxSpeed * (1 - smooth) + marioSpeed * smooth;
		marioVelocity += marioSpeed * marioInput;
		marioSprite.move(marioVelocity * deltaTime);
		//Deceleration
		if (marioInput == sf::Vector2f(0, 0)) {
			marioVelocity -= (1-smooth)*deceleration*marioVelocity*deltaTime;
		}

		if (marioSprite.getPosition().x>cameraX-pushWindow) {
			cameraXSpeed = lerp(cameraXSpeed,marioVelocity.x,0.1f);
			cameraXSpeed = std::clamp(cameraXSpeed,0.f,FLT_MAX);
			cameraX+=cameraXSpeed*deltaTime;
		}

		view.setCenter(sf::Vector2f(cameraX, cameraY));
		window.setView(view);
	}

}
