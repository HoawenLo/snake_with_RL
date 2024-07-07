#include "../include/game.h"

/*
    Function: elementInDeque

    Description: Checks if a cell (element) is within a deque. This is important
        to check collision, such as if the snake's head is on the game boundary
        or if the snake's head has collided with itself.

    Arguments:
        (Vector2) element: The element to check.
        (std::deque<Vector2>) deque: The deque to iterate over to check if the element
            is within it.
     
    Returns:
        (bool) Returns true if element is within deque, else return false.

    Code explanation:

    Code:

    for (int i = 0; i < deque.size(); i++) {}

    Explanation:

    Iterate through the deque.

    Code: 

    if (Vector2Equals(deque[i], element)) {
            return true;
        }
    } return false;

    Explanation:

    If the element has the same Vector2 position as the iterated deque element
    return true else return false.

*/ 
bool elementInDeque(Vector2 element, std::deque<Vector2> deque) {
    for (int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    } return false;
}

/*
    Class: Snake

    Component: Constructor

    Name: Snake

    Description: The constructor which sets up the attributes of the Snake, the initial body
        position, direction the snake is facing and a bool to determine whether to add segments
        to the body if food is collided with.

    Arguments:
        (GameParams) params: The game parameters. Holds the parameters for the snake
            body's initial position, the initial direction it is facing, the add_segment initial
            value which is false and other parameters such as window properties.
     
    Returns:
        None
*/ 
Snake::Snake(const GameParams& params)
    : body(params.body)
    , direction(params.direction)
    , add_segment(params.add_segment)
    , params(params)
{}

/*
    Class: Snake

    Component: Method

    Name: draw

    Description: Draw the snake.

    Arguments:
        None
     
    Returns:
        None

    Code:

    for (int i = 0; i < body.size(); i++) {
        float x = body[i].x;
        float y = body[i].y;

    Explanation:

    Iterate through the snake body and extract its coordinates.

    Code:

    Rectangle segment = Rectangle {params.offset + x * params.cellSize, params.offset + y * params.cellSize, (float)params.cellSize, (float)params.cellSize};

    Explanation:

    Set up a rectangle segment for each component of the snake body. The offset is due 
    to the game area not being at the edge of the window. 

    Code:

    DrawRectangleRounded(segment, 0.5, 6, params.darkGreen);

    Explanation:

    Draw the rectangle with raylib method, where segment is the rectangle, 0.5 the roundness
    of the corners of the rectangle, 6 is the number of parts to draw the corner - the more 
    parts the smoother the corner and params.darkGreen is colour of the rectangle.
*/ 
void Snake::draw() {
    for (int i = 0; i < body.size(); i++) {
        float x = body[i].x;
        float y = body[i].y;
        Rectangle segment = Rectangle {params.offset + x * params.cell_size, params.offset + y * params.cell_size, (float)params.cell_size, (float)params.cell_size};
        DrawRectangleRounded(segment, 0.5, 6, params.dark_green);
    }
}

/*
    Class: Snake

    Component: Method

    Name: update

    Description: Used to move body segments from back to front to simulate the snake moving.

    Arguments:
        None
     
    Returns:
        None

    Code Explanation:

    Code:

    body.push_front(Vector2Add(body[0], direction));

    Explanation:

    Use deque method to push a coordinate value to the coordinate point in front of
    the snake head - which is calculated from the addition of the coordinate point
    of the snake head and the direction it is moving. If add_segment is true, hence
    the snake collided with a food, there is no need to remove the last body component
    as the snake's body needs to increase in size when colliding with food. Otherwise
    pop the final part.
*/ 
void Snake::update() {
    body.push_front(Vector2Add(body[0], direction));
    if (add_segment == true) {
        add_segment = false;
    } else {
        body.pop_back();
    }
}

/*
    Class: Snake

    Component: Method

    Name: reset

    Description: Reset the snake's position to its original position after colliding with itself
        or the edge.

    Arguments:
        None
     
    Returns:
        None

    Code Explanation:

    Code:

    body = body;
    direction = direction;

    Explanation:

    Set the body and direction to the initial coordinate values.

*/
void Snake::reset() {
    body = params.body;
    direction = params.direction;
}

/*
    Class: Food

    Component: Constructor

    Name: Food

    Description: The constructor for the food class sets up the food position.

    Arguments:
        (GameParams) params: The game parameters. Holds the parameters for the snake
            body's initial position and other parameters such as window properties.
     
    Returns:
        None

    Code Explanation:

    Code:

    position = GenerateRandomPos(snakeBody);

    Explanation:

    Generate the position of the food, avoiding any positions which are the components of the snake
    body.

*/
Food::Food(std::deque<Vector2> snake_body, const GameParams& params) 
    : position(position)
    , params(params) 
{
position = generateRandomPos(snake_body);
}

/*
    Class: Food

    Component: Method

    Name: generateRandomCell

    Description: Generate a random cell to position the food.

    Arguments:
        None
        
    Returns:
        (Vector2) A Vector2 value, which represents the generated random coordinate.

    Code Explanation:

    Code:

    float x = GetRandomValue(0, params.cell_count - 1);
    float y = GetRandomValue(0, params.cell_count - 1);

    Explanation:

    Generate random value within the game area.

*/
Vector2 Food::generateRandomCell() {
    float x = GetRandomValue(0, params.cell_count - 1);
    float y = GetRandomValue(0, params.cell_count - 1);
    return Vector2{x, y};
}

/*
    Class: Food

    Component: Method

    Name: generateRandomPos

    Description: Generate a random cell to position of the food. If the position generated
        is within the snake body, generate a new one until a position is generated outside 
        the snake body.

    Arguments:
        None
        
    Returns:
        (Vector2) The position of the food.

    Code Explanation:

    Code:

    Vector2 position = generateRandomCell();

    Explanation:

    Generate random value within the game area.

    Code:

    while(elementInDeque(position, snake_body)) {
        position = generateRandomCell();
    }

    Explanation:

    Generate a random position until a position is generate that is outside the 
    snake's body.
*/
Vector2 Food::generateRandomPos(std::deque<Vector2> snake_body) {
    Vector2 position = generateRandomCell();
    while(elementInDeque(position, snake_body)) {
        position = generateRandomCell();
    } return position;
}

/*
    Class: Food

    Component: Method

    Name: draw

    Description: Draw the food position.

    Arguments:
        None
        
    Returns:
        None

    Code Explanation:

    Code:

    DrawRectangle(params.offset + position.x * params.cell_size, params.offset + position.y * params.cell_size, params.cell_size, params.cell_size, params.white);

    Explanation:

    Draw a white rectangle representing the food at generated position.

*/
void Food::draw() {
    DrawRectangle(params.offset + position.x * params.cell_size, params.offset + position.y * params.cell_size, params.cell_size, params.cell_size, params.white);
}

/*
    Class: Game

    Component: Constructor

    Name: Game

    Description: Setup the variables for the game class.

    Arguments:
        (GameParams) params: The game parameters. Holds the parameters for the snake
            body's initial position other parameters such as window properties.
        
    Returns:
        None

    Code Explanation:

    Code:

    Game::Game(bool game_running, int score, const GameParams& params) 
        : snake(params)
        , food(snake.body, params)
        , game_running(game_running)
        , score(score)
        , params(params)
        , last_update_time(last_update_time)
    {}

    Explanation:

    The constructor takes the arguments and initialises the game_running bool,
    the score, and game parameters. Creates the snake and food object, passing the
    parameters to them. 
*/
Game::Game(bool game_running, int score, const GameParams& params, float last_update_time) 
    : snake(params)
    , food(snake.body, params)
    , game_running(game_running)
    , score(score)
    , params(params)
    , last_update_time(last_update_time)
{}


/*
    Class: Game

    Component: Method

    Name: eventTriggered

    Description: Trigger an event once the time interval is reached. Ensure that game logic
        it set at a specific pace regardless of frame rate. Allows simulation of calmer framerate.

    Arguments:
        (float) interval: The interval in which events should be triggered.
     
    Returns:
        (bool) Returns true if interval is reached hence event can be triggers, 
            else return false. 

    Code explanation:

    Code:

    double current_time = GetTime();

    Explanation:

    Raylib method to get current time.

    Code: 

    if (current_time - Game::last_update_time >= interval) {
        Game::last_update_time = current_time;
        return true;
    } else {
        return false;
    }
}

    Explanation:

    Check that the current time has reached an interval. If it is interval, return
    true else return false. The last_update_time value keeps track of the last time
    an update occurred.
*/ 

bool Game::eventTriggered(float interval) {
    double current_time = GetTime();

    if (current_time - Game::last_update_time >= interval) {
        Game::last_update_time = current_time;
        return true;
    } else {
        return false;
    }
}

/*
    Class: Game

    Component: Constructor

    Name: draw

    Description: Draw food and snake.

    Arguments:
        None
        
    Returns:
        None
*/
void Game::draw() {
    food.draw();
    snake.draw();
}

/*
    Class: Game

    Component: Method

    Name: checkCollisions

    Description: Check collisions with food, edges or tail.

    Arguments:
        None
        
    Returns:
        None
*/
void Game::checkCollisions() {
    if(game_running) {
        checkCollisionWithFood();
        checkCollisionWithEdges();
        checkCollisionWithTail();
    }
}

/*
    Class: Game

    Component: Method

    Name: checkCollisionWithFood

    Description: Check if the snake as collided with the food.

    Arguments:
        None
        
    Returns:
        None

    Code Explanation:

    Code:

    if (Vector2Equals(snake.body[0], food.position))

    Explanation

    Check if the snake head has the same position as food.

    Code:

    food.position = food.generateRandomPos(snake.body);

    Explanation

    Generate new food position.

    Code:

    snake.add_segment = true;

    Explanation:

    Set the add_segment bool to true to allow the snake body to grow.

    Code:

    score++;

    Explanation:

    Increment score.
*/
void Game::checkCollisionWithFood() {
    if (Vector2Equals(snake.body[0], food.position)) {
        food.position = food.generateRandomPos(snake.body);
        snake.add_segment = true;
        score++;
    }
}

/*
    Class: Game

    Component: Method

    Name: checkCollisionWithEdges

    Description: Check if the snake as collided with the edge of the game boundary.

    Arguments:
        None
        
    Returns:
        None

    Code Explanation:

    Code:

    if (snake.body[0].x == params.cell_count || snake.body[0].x == -1) {
        gameOver();
    }
    if (snake.body[0].y == params.cell_count || snake.body[0].y == -1) {
        gameOver();
    }

    Explanation:

    Check if snake head is outside cell_count, hence outside game boundary. If
    it is, run the game over function.
*/
void Game::checkCollisionWithEdges() {
    if (snake.body[0].x == params.cell_count || snake.body[0].x == -1) {
        gameOver();
    }
    if (snake.body[0].y == params.cell_count || snake.body[0].y == -1) {
        gameOver();
    }
}

/*
    Class: Game

    Component: Method

    Name: checkCollisionWithTail

    Description: Check if the snake as collided with its own tail

    Arguments:
        None
        
    Returns:
        None

    Code Explanation:

    Code:

    std::deque<Vector2> headless_body = snake.body;
    headless_body.pop_front();

    Explanation:

    Remove the head from the body.

    Code:

    if (elementInDeque(snake.body[0], headless_body)) {
        gameOver();
    }

    Explanation:

    If the snake's head is within the same coordinates of the snake's body,
    run gameOver method.
*/
void Game::checkCollisionWithTail() {
    std::deque<Vector2> headless_body = snake.body;
    headless_body.pop_front();
    if (elementInDeque(snake.body[0], headless_body)) {
        gameOver();
    }
}

/*
    Class: Game

    Component: Method

    Name: gameOver

    Description: Reset the score, the snake's position and set game to inactive.

    Arguments:
        None
        
    Returns:
        None

    Code Explanation:

    Code:

    snake.reset();
    game_running = false;
    score = 0;

    Explanation:

    Reset the snake's position. Set the game to inactive. Reset score to zero.
*/
void Game::gameOver() {
    snake.reset();
    game_running = false;
    score = 0;
}