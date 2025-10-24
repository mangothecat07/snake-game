#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <cstdlib>
#include <ctime>

// Grid cell size
const int CELL = 10;

// Snake segment
struct Segment {
    int x, y;
};

// Snake class
class Snake {
public:
    std::vector<Segment> body;
    int direction; // 0=Up,1=Right,2=Down,3=Left

    Snake(int startX, int startY) {
        body.push_back({startX, startY});
        direction = 1; // initially moving right
    }
};

// Game class
class Game {
    
private:
    sf::RenderWindow window;
    Snake snake;


    // Food
    Segment food;
    int score = 0;
    bool isLost = false;
    sf::Font font;



    // Create an sf::Text object with a string and a font

public:

    Game() : window(sf::VideoMode({640, 480}), "Snake Game"), snake(100, 100) {
        window.setFramerateLimit(15);
        srand(static_cast<unsigned>(time(nullptr)));
        spawnFood();
        if (!font.openFromFile("C:\\Windows\\Fonts\\Arial.ttf")) {
            throw std::runtime_error("Failed to open font");
        }
    
        sf::Text statusText(font,"Start", 24);
        sf::Text scoreText(font,"Score: 0", 24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(sf::Vector2f(10.f, 10.f));
        
        
    }

    void spawnFood() {
        // Random position aligned to grid
        food.x = (rand() % (window.getSize().x / CELL)) * CELL;
        food.y = (rand() % (window.getSize().y / CELL)) * CELL;
    }
    
    void handleEvents() {
        std::optional<sf::Event> eventOpt;
        while ((eventOpt = window.pollEvent())) {
            sf::Event event = *eventOpt;

            if (event.is<sf::Event::Closed>()) {
                window.close();
            }

            if (const sf::Event::KeyPressed* key = event.getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Up && snake.direction != 2) snake.direction = 0;
                else if (key->code == sf::Keyboard::Key::Right && snake.direction != 3) snake.direction = 1;
                else if (key->code == sf::Keyboard::Key::Down && snake.direction != 0) snake.direction = 2;
                else if (key->code == sf::Keyboard::Key::Left && snake.direction != 1) snake.direction = 3;
            }
            const sf::Event::KeyPressed* key = event.getIf<sf::Event::KeyPressed>();
            if (isLost && key) {
                if (key->code == sf::Keyboard::Key::R) {
                    // Reset snake
                    snake.body = { {100, 100} };
                    snake.direction = 1;
                    score = 0;
                    isLost = false;
                    spawnFood();
                }
            }
        }
    }

    void update() {
        if (isLost) return; // Stop snake movement while lost
        // Move body segments
        for (int i = static_cast<int>(snake.body.size()) - 1; i > 0; i--) {
            snake.body[i] = snake.body[i - 1];
        }
    
        // Move head
        switch (snake.direction) {
            case 0: snake.body[0].y -= CELL; break; // Up
            case 1: snake.body[0].x += CELL; break; // Right
            case 2: snake.body[0].y += CELL; break; // Down
            case 3: snake.body[0].x -= CELL; break; // Left
        }
    
        // Collision with food
        bool ateFood = (snake.body[0].x == food.x && snake.body[0].y == food.y);
        if (ateFood) {
            spawnFood();
            // Add a new segment at the tail (same spot)
            snake.body.push_back(snake.body.back());
            sf::Text scoreText(font,"Score: 0", 24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(sf::Vector2f(10.f, 40.f));


            // Inside your game loop or update function:
            score += 1;  // e.g., when snake eats food
            scoreText.setString("Score: " + std::to_string(score));

            // Then in draw():
            window.draw(scoreText);  // Draw updated text

        }
        int maxX = window.getSize().x;
        int maxY = window.getSize().y;
    

        // Collision with self — only check if we did NOT just eat
        if (!ateFood) {
            for (size_t i = 0; i < snake.body.size(); i++) {  // start from 0 to include head or any body
                for (size_t j = i + 1; j < snake.body.size(); j++) {
                    if (snake.body[i].x == snake.body[j].x && snake.body[i].y == snake.body[j].y) {
                        isLost = true;
                    }
                }
            }
        }
        if (snake.body[0].x < 0) snake.body[0].x = maxX - CELL;
        if (snake.body[0].x >= maxX) snake.body[0].x = 0;
        if (snake.body[0].y < 0) snake.body[0].y = maxY - CELL;
        if (snake.body[0].y >= maxY) snake.body[0].y = 0;
    }
    

    void draw() {
        window.clear(sf::Color::Black);

        // Draw snake
        sf::RectangleShape rect(sf::Vector2f(CELL - 1.f, CELL - 1.f));
        rect.setFillColor(sf::Color::White);
        for (auto &s : snake.body) {
            rect.setPosition(sf::Vector2f(static_cast<float>(s.x), static_cast<float>(s.y)));
            window.draw(rect);
        }

        // Draw food
        sf::RectangleShape foodRect(sf::Vector2f(CELL - 1.f, CELL - 1.f));
        foodRect.setFillColor(sf::Color::Red);
        foodRect.setPosition(sf::Vector2f(static_cast<float>(food.x), static_cast<float>(food.y)));
        window.draw(foodRect);


        //Draw Score
        sf::Text scoreText(font,"Score: " + std::to_string(score),  24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(sf::Vector2f(10.f, 10.f));
        window.draw(scoreText);
    
        // Draw status if lost
        if (isLost) {
            sf::Text statusText(font,"You Lost! Press R to Restart",  24);
            statusText.setFillColor(sf::Color::Red);
            statusText.setPosition(sf::Vector2f(10.f, 40.f));
            window.draw(statusText);

        }
        
       

        
        window.display();
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            draw();
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
