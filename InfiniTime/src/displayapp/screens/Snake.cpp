#include "Snake.h"
#include <lvgl/lvgl.h>
#include <cstdio>
#include <cstdlib>

using namespace Pinetime::Applications::Screens;

// Free function to wrap the member function
void snakeMoveTask(lv_task_t* task) {
    Snake* snake = static_cast<Snake*>(task->user_data);
    snake->moveSnake(); // Call the member function
}

Snake::Snake() : currentInterval(1000) {

struct colorPair {
    lv_color_t bg;
    lv_color_t fg;
  };

  static constexpr colorPair colors[nColors] = {
    {LV_COLOR_MAKE(0, 0, 0), LV_COLOR_BLACK},
    {LV_COLOR_MAKE(0, 255, 0), LV_COLOR_LIME},
    {LV_COLOR_MAKE(0, 0, 255)},
    {LV_COLOR_MAKE(255, 0, 0), LV_COLOR_RED},
  };

    gridDisplay = lv_table_create(lv_scr_act(), nullptr);
    
    for (size_t i = 0; i < nColors; i++) {
    lv_style_init(&cellStyles[i]);

    lv_style_set_border_color(&cellStyles[i], LV_STATE_DEFAULT, lv_color_hex(0x000000));
    lv_style_set_border_width(&cellStyles[i], LV_STATE_DEFAULT, 1);
    lv_style_set_bg_opa(&cellStyles[i], LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&cellStyles[i], LV_STATE_DEFAULT, colors[i].bg);
    lv_style_set_text_color(&cellStyles[i], LV_STATE_DEFAULT, colors[i].fg);
    
    // Set padding to adjust cell height
        lv_style_set_pad_top(&cellStyles[i], LV_STATE_DEFAULT, -1);  // Adjust as needed
        lv_style_set_pad_bottom(&cellStyles[i], LV_STATE_DEFAULT, -6); // Adjust as needed


    lv_obj_add_style(gridDisplay, LV_TABLE_PART_CELL1 + i, &cellStyles[i]);
  }
    
    movementTask = lv_task_create(snakeMoveTask, currentInterval, LV_TASK_PRIO_MID, this);

    lv_table_set_col_cnt(gridDisplay, nCols);
    lv_table_set_row_cnt(gridDisplay, nRows);

    for (int col = 0; col < nCols; col++) {
    lv_table_set_col_width(gridDisplay, col, 240 / nCols);
    for (int row = 0; row < nRows; row++) {
        grid[row][col].type = SnakeTile::Type::Empty;
        lv_table_set_cell_type(gridDisplay, row, col, 1);
        lv_table_set_cell_align(gridDisplay, row, col, LV_LABEL_ALIGN_CENTER);
   }
}
lv_obj_set_pos(gridDisplay, 0, 0);

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

void Snake::updateGridDisplay() {
    for (int row = 0; row < nRows; row++) {
        for (int col = 0; col < nCols; col++) {
            switch (grid[row][col].type) {
                case SnakeTile::Type::Empty:
                    lv_table_set_cell_value(gridDisplay, row, col, "");
                    lv_table_set_cell_type(gridDisplay, row, col, 1);
                    break;
                case SnakeTile::Type::SnakeBody:
                    lv_table_set_cell_value(gridDisplay, row, col, "");
                    lv_table_set_cell_type(gridDisplay, row, col, 2);
                    break;
                case SnakeTile::Type::SnakeHead:
                    lv_table_set_cell_value(gridDisplay, row, col, "");
                    lv_table_set_cell_type(gridDisplay, row, col, 3);
                    break;
                case SnakeTile::Type::Food:
                    lv_table_set_cell_value(gridDisplay, row, col, "");
                    lv_table_set_cell_type(gridDisplay, row, col, 4);
                    break;
            }
        }
    }
}



void Snake::moveSnake() {
    // Determine the new head position based on lastDirection
    auto headPos = snakeBody.front();
    int newRow = headPos.first;
    int newCol = headPos.second;

    switch (lastDirection) {
        case Direction::Up:
            newRow--;
            break;
        case Direction::Down:
            newRow++;
            break;
        case Direction::Left:
            newCol--;
            break;
        case Direction::Right:
            newCol++;
            break;
        default:
            return; // No movement
    }

    // Check bounds
     // Check bounds and self-collision
    if (newRow < 0 || newRow >= nRows || newCol < 0 || newCol >= nCols || grid[newRow][newCol].type == SnakeTile::Type::SnakeBody) {
        if (!inGracePeriod) {
            inGracePeriod = true;
            // Start a task to delay the game over
            gracePeriodTask = lv_task_create(Snake::gracePeriodTimeout, 750, LV_TASK_PRIO_LOW, this);
            return;
        } else {
            gameOver();
            return;
        }
    }

    // Check for food and adjust the snake length if food is eaten
    if (grid[newRow][newCol].type == SnakeTile::Type::Food) {
        snakeLength++;
        placeFood(); // Place new food
            // Decrease the interval, but don't go below a certain threshold (e.g., 200 ms)
    currentInterval = std::max(200, currentInterval - 5);
    lv_task_set_period(movementTask, currentInterval);
    }

    // Update the grid: Clear the tail segment
    if (snakeBody.size() >= snakeLength) {
        auto tailPos = snakeBody.back();
        grid[tailPos.first][tailPos.second].type = SnakeTile::Type::Empty;
        snakeBody.pop_back(); // Remove tail
    }

    // Update the head position
    snakeBody.insert(snakeBody.begin(), {newRow, newCol}); // Insert new head
    grid[newRow][newCol].type = SnakeTile::Type::SnakeHead;

    // Update body segments
    for (size_t i = 1; i < snakeBody.size(); i++) {
        auto segment = snakeBody[i];
        grid[segment.first][segment.second].type = SnakeTile::Type::SnakeBody;
    }

    // Render the updated grid
    updateGridDisplay();
}

void Snake::gameOver() {
    isGameOver = true; // Set the game over flag

    // Delete or stop the movement task
    if (movementTask) {
        lv_task_del(movementTask);
        movementTask = nullptr;
    }
  }
  
  void Snake::gracePeriodTimeout(lv_task_t* task) {
    Snake* snake = static_cast<Snake*>(task->user_data);
    snake->inGracePeriod = false;
    snake->moveSnake(); // Attempt to move again
    lv_task_del(snake->gracePeriodTask); // Clean up the task
    snake->gracePeriodTask = nullptr;
}


void Snake::placeFood() {
    int row, col;
    do {
        row = rand() % nRows;
        col = rand() % nCols;
    } while (grid[row][col].type != SnakeTile::Type::Empty); // Find an empty cell

    grid[row][col].type = SnakeTile::Type::Food; // Place food
}


bool Snake::OnTouchEvent(TouchEvents event) {
    if (isGameOver) return false; // Ignore input if the game is over
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

