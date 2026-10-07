#include <SFML/Graphics.hpp>

int main()
{
	//Render window
	sf::RenderWindow window( sf::VideoMode( { 800, 600 } ), "Cat And Dog" );

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

	float dogSpeed = 100;

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

		//Move dog
		dogSprite.move(sf::Vector2f(dogSpeed*deltaTime, 0));
	}
}
