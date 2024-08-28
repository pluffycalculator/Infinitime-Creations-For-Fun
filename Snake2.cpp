#include "Snake.h"
#include <lvgl/lvgl.h>
#include <cstdio>
#include <cstdlib>

using namespace Pinetime::Applications::Screens;

void snakeMoveTask(lv_task_t* task) {
    Snake* snake = static_cast<Snake*>(task->user_data);
    snake->moveSnake();
}

Snake::Snake() : currentInterval(1000) {

    // Initialize styles for snake and food
    lv_style_init(&snakeStyle);
    lv_style_set_line_width(&snakeStyle, LV_STATE_DEFAULT, cellSize);
    lv_style_set_line_color(&snakeStyle, LV_STATE_DEFAULT, LV_COLOR_LIME);  // Green snake

    lv_style_init(&foodStyle);
    lv_style_set_line_width(&foodStyle, LV_STATE_DEFAULT, cellSize);
    lv_style_set_line_color(&foodStyle, LV_STATE_DEFAULT, LV_COLOR_RED);  // Red food

    // Create the snake line object
    snakeLine = lv_line_create(lv_scr_act(), nullptr);
    lv_obj_add_style(snakeLine, LV_LINE_PART_MAIN, &snakeStyle);

    // Create the food line object
    foodObj = lv_line_create(lv_scr_act(), nullptr);
    lv_obj_add_style(foodObj, LV_LINE_PART_MAIN, &foodStyle);

    movementTask = lv_task_create(snakeMoveTask, currentInterval, LV_TASK_PRIO_MID, this);

    // Initial placement of snake head
    grid[nRows / 2][nCols / 2].type = SnakeTile::Type::SnakeHead;
    snakeBody.push_back({nRows / 2, nCols / 2});

    // Place initial food
    placeFood();

    // Render the initial grid
    updateGridDisplay();
}

Snake::~Snake() {
    lv_obj_clean(lv_scr_act());
    lv_task_del(movementTask);
}

void Snake::drawSnake() {
    snakePoints.clear();

    for (const auto& segment : snakeBody) {
        int row = segment.first;
        int col = segment.second;

        lv_point_t point;
        point.x = col * cellSize + cellSize / 2;
        point.y = row * cellSize + cellSize / 2;
        snakePoints.push_back(point);
    }

    lv_line_set_points(snakeLine, snakePoints.data(), snakePoints.size());
}

void Snake::clearSnake() {
    snakePoints.clear();
    lv_line_set_points(snakeLine, snakePoints.data(), 0);
}

void Snake::drawFood(int row, int col) {
    lv_point_t foodPoint[2];

    foodPoint[0].x = col * cellSize + cellSize / 2 - 5; // Adjust for size
    foodPoint[0].y = row * cellSize + cellSize / 2 - 5;
    foodPoint[1].x = col * cellSize + cellSize / 2 + 5;
    foodPoint[1].y = row * cellSize + cellSize / 2 + 5;

    lv_line_set_points(foodObj, foodPoint, 2);
}

void Snake::moveSnake() {
    auto headPos = snakeBody.front();
    int newRow = headPos.first;
    int newCol = headPos.second;

    switch (lastDirection) {
        case Direction::Up: newRow--; break;
        case Direction::Down: newRow++; break;
        case Direction::Left: newCol--; break;
        case Direction::Right: newCol++; break;
        default: return; // No movement
    }

    // Check bounds
    if (newRow < 0 || newRow >= nRows || newCol < 0 || newCol >= nCols || grid[newRow][newCol].type == SnakeTile::Type::SnakeBody) {
        if (!inGracePeriod) {
            inGracePeriod = true;
            gracePeriodTask = lv_task_create(Snake::gracePeriodTimeout, 750, LV_TASK_PRIO_LOW, this);
            return;
        } else {
            gameOver();
            return;
        }
    }

    // Check for food
    if (grid[newRow][newCol].type == SnakeTile::Type::Food) {
        snakeLength++;
        placeFood();
        currentInterval = std::max(200, currentInterval - 5);
        lv_task_set_period(movementTask, currentInterval);
    }

    // Clear tail if necessary
    if (snakeBody.size() >= snakeLength) {
        auto tailPos = snakeBody.back();
        grid[tailPos.first][tailPos.second].type = SnakeTile::Type::Empty;
        snakeBody.pop_back();
        clearSnake();  // Clear before redrawing
    }

    // Update snake position
    snakeBody.insert(snakeBody.begin(), {newRow, newCol});
    grid[newRow][newCol].type = SnakeTile::Type::SnakeHead;

    for (size_t i = 1; i < snakeBody.size(); i++) {
        auto segment = snakeBody[i];
        grid[segment.first][segment.second].type = SnakeTile::Type::SnakeBody;
    }

    updateGridDisplay();  // Ensure grid is updated after each move
}

void Snake::updateGridDisplay() {
    drawSnake();
}

void Snake::gameOver() {
    isGameOver = true;

    if (movementTask) {
        lv_task_del(movementTask);
        movementTask = nullptr;
    }
}

void Snake::gracePeriodTimeout(lv_task_t* task) {
    Snake* snake = static_cast<Snake*>(task->user_data);
    snake->inGracePeriod = false;
    snake->moveSnake();
    lv_task_del(snake->gracePeriodTask);
    snake->gracePeriodTask = nullptr;
}

void Snake::placeFood() {
    int row, col;
    do {
        row = rand() % nRows;
        col = rand() % nCols;
    } while (grid[row][col].type != SnakeTile::Type::Empty);

    grid[row][col].type = SnakeTile::Type::Food;
    drawFood(row, col);
}

bool Snake::OnTouchEvent(TouchEvents event) {
    if (isGameOver) return false;

    switch (event) {
        case TouchEvents::SwipeUp:
            lastDirection = Direction::Up;
            break;
        case TouchEvents::SwipeDown:
            lastDirection = Direction::Down;
            break;
        case TouchEvents::SwipeLeft:
            lastDirection = Direction::Left;
            break;
        case TouchEvents::SwipeRight:
            lastDirection = Direction::Right;
            break;
        default:
            return false;
    }

    updateGridDisplay();
    return true;
}

