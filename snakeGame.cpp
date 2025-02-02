#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>

using namespace sf;
using namespace std;

// Constants
const int WIDTH = 800;      // Window width
const int HEIGHT = 600;     // Window height
const int BLOCK_SIZE = 20;  // Size of each snake segment and food

// Structures
struct Snake {
    vector<Vector2f> body;  // Snake body segments
    Vector2f direction;     // Current movement direction
};

struct Food {
    Vector2f position;      // Food position
};

// Function Declarations
void generateFood(Food& food, const Snake& snake);  // Generate food at a random position
void handleInput(Event& event, Snake& snake);       // Handle keyboard input for snake movement
void updateSnake(Snake& snake, Food& food, int& score, Sound& eatSound); // Update snake position and check collisions
void drawGame(RenderWindow& window, const Snake& snake, const Food& food); // Draw snake and food

int main() {
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Snake Game"); // Create game window
    window.setFramerateLimit(10); // Limit frame rate to control snake speed

    Snake snake;
    snake.body.push_back(Vector2f(WIDTH / 2, HEIGHT / 2)); // Initial snake position
    snake.direction = Vector2f(BLOCK_SIZE, 0); // Initial movement direction (right)

    Food food;
    generateFood(food, snake); // Generate initial food

    int score = 0; // Player score

    // Load sound effects
    SoundBuffer eatBuffer;
    if (!eatBuffer.loadFromFile("eat.wav")) {
        cerr << "Failed to load eat sound effect!" << endl;
        return -1;
    }
    Sound eatSound;
    eatSound.setBuffer(eatBuffer);

    // Main game loop
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close(); // Close window if close button is clicked

            handleInput(event, snake); // Handle keyboard input
        }

        updateSnake(snake, food, score, eatSound); // Update snake position and check collisions

        // Draw everything
        window.clear();
        drawGame(window, snake, food); // Draw snake and food
        window.display();
    }

    return 0;
}

// Function Definitions

/**
 * Generates food at a random position that doesn't overlap with the snake's body.
 * @param food The food object to update.
 * @param snake The snake object to check for collisions.
 */
void generateFood(Food& food, const Snake& snake) {
    bool validPosition;
    do {
        validPosition = true;
        food.position.x = rand() % (WIDTH / BLOCK_SIZE) * BLOCK_SIZE;
        food.position.y = rand() % (HEIGHT / BLOCK_SIZE) * BLOCK_SIZE;

        // Ensure food doesn't spawn on the snake's body
        for (const auto& segment : snake.body) {
            if (food.position == segment) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}

/**
 * Handles keyboard input to change the snake's direction.
 * @param event The SFML event object.
 * @param snake The snake object to update.
 */
void handleInput(Event& event, Snake& snake) {
    if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Up && snake.direction.y == 0)
            snake.direction = Vector2f(0, -BLOCK_SIZE); // Move up
        if (event.key.code == Keyboard::Down && snake.direction.y == 0)
            snake.direction = Vector2f(0, BLOCK_SIZE); // Move down
        if (event.key.code == Keyboard::Left && snake.direction.x == 0)
            snake.direction = Vector2f(-BLOCK_SIZE, 0); // Move left
        if (event.key.code == Keyboard::Right && snake.direction.x == 0)
            snake.direction = Vector2f(BLOCK_SIZE, 0); // Move right
    }
}

/**
 * Updates the snake's position and checks for collisions with food, walls, or itself.
 * @param snake The snake object to update.
 * @param food The food object to check for collisions.
 * @param score The player's score to update.
 * @param eatSound The sound effect to play when food is eaten.
 */
void updateSnake(Snake& snake, Food& food, int& score, Sound& eatSound) {
    // Move snake
    Vector2f newHead = snake.body[0] + snake.direction;
    snake.body.insert(snake.body.begin(), newHead);

    // Check for collision with food
    if (snake.body[0] == food.position) {
        generateFood(food, snake); // Generate new food
        score++; // Increase score
        eatSound.play(); // Play eat sound effect
    } else {
        snake.body.pop_back(); // Remove tail segment if no food is eaten
    }

    // Check for collision with walls or itself
    if (snake.body[0].x < 0 || snake.body[0].x >= WIDTH ||
        snake.body[0].y < 0 || snake.body[0].y >= HEIGHT) {
        cout << "Game Over! Score: " << score << endl;
        window.close(); // End game if snake hits the wall
    }

    for (size_t i = 1; i < snake.body.size(); ++i) {
        if (snake.body[0] == snake.body[i]) {
            cout << "Game Over! Score: " << score << endl;
            window.close(); // End game if snake collides with itself
        }
    }
}

/**
 * Draws the snake and food on the window.
 * @param window The SFML render window.
 * @param snake The snake object to draw.
 * @param food The food object to draw.
 */
void drawGame(RenderWindow& window, const Snake& snake, const Food& food) {
    // Draw snake
    for (const auto& segment : snake.body) {
        RectangleShape rect(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        rect.setPosition(segment);
        rect.setFillColor(Color::Green);
        window.draw(rect);
    }

    // Draw food
    RectangleShape foodRect(Vector2f(BLOCK_SIZE, BLOCK_SIZE));
    foodRect.setPosition(food.position);
    foodRect.setFillColor(Color::Red);
    window.draw(foodRect);
}