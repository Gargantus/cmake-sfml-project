#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>

using namespace std;

float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

sf::Texture groundTexture;
sf::Texture questionTexture;
sf::Texture brickTexture;
sf::Texture skyTexture;
sf::Texture luigiTexture;
sf::Texture marioTexture;

float maxSpeed = 10.f;
float smooth = 0.2f;
float deceleration = 10;
float cameraXSpeed = 0;
float deltaTime;

std::vector<sf::Sprite> tiles;

float cameraX = 960.f;
float cameraY = 540.f;
float pushWindow = 300.f;
float defaultCameraPos = 500.f;
float lastCameraPos = defaultCameraPos;

//Render window
unsigned screenWidth = 1920;
unsigned screenHeight = 1080;

//Main camera
bool useMainCamera = true;

class Player {
	public:
	sf::Sprite sprite;
	float speed;
	sf::Vector2f input;
	sf::Vector2f velocity;

	sf::Keyboard::Key leftButton;
	sf::Keyboard::Key rightButton;
	sf::Keyboard::Key upButton;
	sf::Keyboard::Key downButton;
	sf::View view;
	sf::FloatRect viewRect;
	float cameraPos = defaultCameraPos;
	float cameraSpeed = 0;


	Player(sf::Keyboard::Key leftKey, sf::Keyboard::Key rightKey,
		sf::Keyboard::Key upKey, sf::Keyboard::Key downKey,
		sf::Texture* texture, sf::Vector2f startPos, sf::FloatRect viewport)
	: sprite(*texture)
	{
		this->leftButton = leftKey;
		this->rightButton = rightKey;
		this->upButton = upKey;
		this->downButton = downKey;
		sprite.setPosition(startPos);
		sf::Vector2f viewSize(viewport.size.x * screenWidth,
							  viewport.size.y * screenHeight);
		view = sf::View({defaultCameraPos,screenHeight/2.f}, viewSize);
		view.setViewport(viewport);
	}
};

void handleMovement(Player& player);
void generateTiles();
int loadTextures();
void handleCamera(Player& player);

int main()
{
	loadTextures();
	generateTiles();

	sf::RenderWindow window( sf::VideoMode( { screenWidth, screenHeight } ), "Mario" );
	sf::Clock clock;

	Player mario(sf::Keyboard::Key::A, sf::Keyboard::Key::D,
				 sf::Keyboard::Key::W, sf::Keyboard::Key::S,
				 &marioTexture, sf::Vector2f(300, 300),
				 sf::FloatRect({0.f, 0.f}, {0.5f, 1.f}));

	Player luigi(sf::Keyboard::Key::Left, sf::Keyboard::Key::Right,
				 sf::Keyboard::Key::Up, sf::Keyboard::Key::Down,
				 &luigiTexture, sf::Vector2f(400, 300),
				 sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f}));


	sf::View mainView(sf::Vector2f(cameraX, cameraY),
					  sf::Vector2f((float)screenWidth, (float)screenHeight));
	mainView.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));
	mainView.setCenter(sf::Vector2f(defaultCameraPos,screenHeight/2.f));


	while (window.isOpen())
	{
		deltaTime = clock.restart().asSeconds();

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();

			if (const auto* key = event->getIf<sf::Event::KeyPressed>())
				if (key->code == sf::Keyboard::Key::Tab)
					useMainCamera = !useMainCamera;
		}

		handleMovement(mario);
		handleMovement(luigi);
		handleCamera(mario);
		handleCamera(luigi);

		window.clear(sf::Color::Magenta);

		if (useMainCamera) {
			window.setView(mainView);
			for (auto& tile : tiles)
				window.draw(tile);
			window.draw(mario.sprite);
			window.draw(luigi.sprite);
		}
		else {
			for (Player* p : { &mario, &luigi }) {
				window.setView(p->view);
				for (auto& tile : tiles)
					window.draw(tile);
				window.draw(mario.sprite);
				window.draw(luigi.sprite);
			}
		}

		lastCameraPos = std::min(mario.cameraPos, luigi.cameraPos)+460;
		mainView.setCenter({lastCameraPos,screenHeight/2.f});

		if (luigi.sprite.getPosition().x > lastCameraPos+460) {
			luigi.view.setViewport(sf::FloatRect({0.5f, 0.0f}, {0.5f, 1.f}));
			mario.view.setViewport(sf::FloatRect({0.f, 0.0f}, {0.5f, 1.f}));
			useMainCamera = false;
		} else if (mario.sprite.getPosition().x > lastCameraPos+460) {
			luigi.view.setViewport(sf::FloatRect({0.f, 0.0f}, {0.5f, 1.f}));
			mario.view.setViewport(sf::FloatRect({0.5f, 0.0f}, {0.5f, 1.f}));
			useMainCamera = false;
		} else {
			useMainCamera = true;
		}

		cout<<luigi.sprite.getPosition().x<<endl;
		cout<<lastCameraPos<<endl;

		window.display();
	}
}

void handleMovement(Player& player) {
	///Mario
	//Get keyboard input
	player.input.x = 0;
	player.input.y = 0;
	if (sf::Keyboard::isKeyPressed(player.leftButton)) {

		player.input += sf::Vector2f(-1, 0);
		cout<<"Left key pressed"<<endl;
	}
	if (sf::Keyboard::isKeyPressed(player.rightButton)) {
		player.input += sf::Vector2f(1, 0);
		cout<<"Right key pressed"<<endl;
	}
	if (sf::Keyboard::isKeyPressed(player.downButton)) {
		player.input += sf::Vector2f(0, 1);
		cout<<"Down key pressed"<<endl;
	}
	if (sf::Keyboard::isKeyPressed(player.upButton)) {
		player.input += sf::Vector2f(0, -1);
		cout<<"Up key pressed"<<endl;
	}
	//Acceleration
	player.speed = maxSpeed * (1 - smooth) + player.speed * smooth;
	player.velocity += player.speed * player.input;
	player.sprite.move(player.velocity * deltaTime);
	//Deceleration
	if (player.input == sf::Vector2f(0, 0)) {
		player.velocity -= (1-smooth)*deceleration*player.velocity*deltaTime;
	}
}

void handleCamera(Player& player) {
	if (player.sprite.getPosition().x>player.cameraPos-pushWindow) {
		player.cameraSpeed = lerp(player.cameraSpeed,player.velocity.x,0.1f);
		player.cameraSpeed = std::clamp(player.cameraSpeed,0.f,FLT_MAX);
		player.cameraPos+=player.cameraSpeed*deltaTime;
	}
	player.view.setCenter(sf::Vector2f(player.cameraPos, cameraY));
}

void generateTiles () {
	// Load level
	float tileXOffset = 0.f;
	float tileYOffset = 0.f;
	std::ifstream levelFile("src/levels/level2.txt");
	std::string lineText;
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
}

int loadTextures() {
	//Load ground sprite
	if (!groundTexture.loadFromFile("src/sprites/Ground.png"))
		return -1;
	//Load question sprite
	if (!questionTexture.loadFromFile("src/sprites/Question.png"))
		return -1;
	//Load brick texture
	if (!brickTexture.loadFromFile("src/sprites/Brick.png"))
		return -1;
	//Load sky texture
	if (!skyTexture.loadFromFile("src/sprites/Sky.png"))
		return -1;
	//Load luigi texture
	if (!luigiTexture.loadFromFile("src/sprites/Luigi.png"))
		return -1;
	//Load mario sprite
	if (!marioTexture.loadFromFile("src/sprites/Mario.png"))
		return -1;
	return 0;
}
