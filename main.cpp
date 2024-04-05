#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdio>
#include <cstring>
#include <queue>
#include <random>
#include <time.h>
#include <vector>

#define ROWS 48
#define COLS 27
#define SCALE 30
#define SPEED 0.08

size_t width = SCALE * ROWS;
size_t height = SCALE * COLS;

static bool gameOver = 0;

struct Position {
    int x, y;

    Position() : x(0), y(0) {}

    Position(int x, int y) : x(x), y(y) {}

    void applyTransform(Position pos) {
        x += pos.x;
        y += pos.y;
    }

    bool operator==(const Position &other) const {
        return x == other.x && y == other.y;
    }
};

enum Direction { Up = 1, Down = -1, Left = 2, Right = -2, None = 0 };

struct Snake {
    Position head;

    std::vector<Position> body;

    Direction direction;

    // For rotations
    std::queue<Direction> queue;
    bool moved = 0;

    Snake(Position startPos, Direction startDir)
        : head(startPos), body{}, direction(startDir), queue{} {
        // Comment on smth
    }
};

// Function to generate a random integer in the range [min, max]
int getRandomIntInRange(int min, int max) {
    static std::random_device rd;  // Only initialized once
    static std::mt19937 gen(rd()); // Only initialized once
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

void checkDeath(Snake *snake) {
    for (Position bone : snake->body) {
        if (bone == snake->head)
            gameOver = 1;
    }
}

void checkApple(Snake *snake, Position *apple) {
    // If eats apple
    if (snake->head.x == apple->x && snake->head.y == apple->y) {
        Position last;
        if (!snake->body.empty()) {
            last = Position(snake->body.front());
        } else {
            last = Position(snake->head);
        }
        snake->body.push_back(Position(last.x, last.y));

        // Update apple position
        apple->x = getRandomIntInRange(0, ROWS - 1);
        apple->y = getRandomIntInRange(0, COLS - 1);
    }
}

void moveSnake(Snake *snake, Position *apple) {
    snake->moved = 0;

    Position transform;
    Direction dir;

    if (snake->queue.empty()) {
        dir = snake->direction;
    } else {
        dir = snake->queue.front();
        snake->queue.pop();
    }

    switch (dir) {
    case Up:
        transform = Position(0, -1);
        break;

    case Left:
        transform = Position(-1, 0);
        break;

    case Down:
        transform = Position(0, 1);
        break;

    case Right:
        transform = Position(1, 0);
        break;

    case None:
        transform = Position(0, 0);
        break;
    }

    // Body catchup
    for (int i = snake->body.size() - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    if (!snake->body.empty()) {
        snake->body[0] = snake->head;
    }

    snake->head.applyTransform(transform);
}

void Tick(Snake *snake, Position *apple) {

    moveSnake(snake, apple);
    checkDeath(snake);
    checkApple(snake, apple);

    // Bound check
    if (snake->head.x < 0)
        snake->head.x = ROWS - 1;
    else if (snake->head.y < 0)
        snake->head.y = COLS - 1;
    else if (snake->head.y >= COLS)
        snake->head.y = 0;
    else if (snake->head.x >= ROWS)
        snake->head.x = 0;
}

void keyPressed(sf::Event *event, Snake *snake) {

    Direction dir = None;

    switch (event->key.code) {

    // W
    case sf::Keyboard::W:
        if (snake->direction + Up != 0)
            dir = Up;
        break;
    case sf::Keyboard::Up:
        if (snake->direction + Up != 0)
            dir = Up;
        break;

    // A
    case sf::Keyboard::A:
        if (snake->direction + Left != 0)
            dir = Left;
        break;
    case sf::Keyboard::Left:
        if (snake->direction + Left != 0)
            dir = Left;
        break;

    // S
    case sf::Keyboard::S:
        if (snake->direction + Down != 0)
            dir = Down;
        break;
    case sf::Keyboard::Down:
        if (snake->direction + Down != 0)
            dir = Down;
        break;

    // D
    case sf::Keyboard::D:
        if (snake->direction + Right != 0)
            dir = Right;
        break;
    case sf::Keyboard::Right:
        if (snake->direction + Right != 0)
            dir = Right;
        break;
    }

    if (dir == None)
        return; // Guard clause

    if (!snake->queue.empty() || snake->moved) {
        snake->queue.push(snake->direction);
    }
    snake->direction = dir;
    snake->moved = 1;
}

void drawGraphics(sf::RenderWindow *window, const Snake *snake,
                  const Position *apple) {
    // Colors
    sf::Color snakeHeadColor(0, 200, 0);
    sf::Color snakeBodyColor(0, 155, 0);
    sf::Color appleColor(200, 0, 0);

    // Setup shapes
    sf::RectangleShape appleShape(sf::Vector2f(SCALE, SCALE));
    appleShape.setFillColor(appleColor);

    sf::RectangleShape snakeHeadShape(sf::Vector2f(SCALE, SCALE));
    snakeHeadShape.setFillColor(snakeHeadColor);

    sf::RectangleShape snakeBodyShape(sf::Vector2f(SCALE, SCALE));
    snakeBodyShape.setFillColor(snakeBodyColor);

    // Clear the screen
    window->clear();

    // Draw apple
    appleShape.setPosition(apple->x * SCALE, apple->y * SCALE);
    window->draw(appleShape);

    // Draw Snake

    // Head
    snakeHeadShape.setPosition(snake->head.x * SCALE, snake->head.y * SCALE);
    window->draw(snakeHeadShape);

    // Body
    for (const Position &bone : snake->body) {
        snakeBodyShape.setPosition(bone.x * SCALE, bone.y * SCALE);
        window->draw(snakeBodyShape);
    }

    window->display();
}

int main() {
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(width, height), "Snake!");

    sf::Clock clock;
    float timer = 0;

    Snake snake(Position(ROWS / 4, COLS / 2), None);
    Position apple(ROWS / 2, COLS / 2);

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        sf::Event event;
        while (window.pollEvent(event)) {

            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                keyPressed(&event, &snake);
                break;
            }
        }

        if (timer > SPEED && !gameOver) {
            timer = 0;
            Tick(&snake, &apple);
        }

        ////// draw  ///////
        drawGraphics(&window, &snake, &apple);
    }

    return 0;
}
