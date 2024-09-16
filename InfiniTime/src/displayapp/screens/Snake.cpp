#include "displayapp/screens/Snake.h"
#include "displayapp/DisplayApp.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <lvgl/lvgl.h>



using namespace Pinetime::Applications::Screens;

int Snake::segmentSize = 10;

void snakeMoveTask(lv_task_t* task) {
    Snake* snake = static_cast<Snake*>(task->user_data);
    snake->moveSnake(); // Call the member function
}

static void btnEventHandler(lv_obj_t* obj, lv_event_t event) {
  Snake* snake = static_cast<Snake*>(obj->user_data);
  snake->OnButtonEvent(obj, event);
}

Snake::Snake() : currentInterval(1000), isGameStarted(false) {
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create the start screen
    createStartScreen();
}

Snake::~Snake() {
    lv_task_del(movementTask);
    lv_obj_clean(lv_scr_act());
    lv_style_reset(&snakeStyle);
    lv_style_reset(&foodStyle);
}

void Snake::createStartScreen() {
    lv_obj_clean(lv_scr_act()); // Clear any existing objects
    
    lv_style_init(&buttonStyle);
    
    lv_style_set_bg_opa(&buttonStyle, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&buttonStyle, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_style_set_bg_grad_color(&buttonStyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_grad_dir(&buttonStyle, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    
    //lv_style_set_bg_color(&buttonStyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_border_width(&buttonStyle, LV_STATE_DEFAULT, 4);
    lv_style_set_border_color(&buttonStyle, LV_STATE_DEFAULT, LV_COLOR_LIME);
    
    lv_style_init(&labelStyle);
    
    lv_style_set_text_color(&labelStyle, LV_STATE_DEFAULT, LV_COLOR_LIME);  

    // Create a button for starting the game
    startBtn = lv_btn_create(lv_scr_act(), nullptr);
    startBtn->user_data = this;
    lv_obj_set_event_cb(startBtn, btnEventHandler);
    lv_obj_set_size(startBtn, 200, 50);
    lv_obj_add_style(startBtn, LV_BTN_PART_MAIN, &buttonStyle);
    lv_obj_align(startBtn, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, -5); // Center the button
    
    sizeBtn = lv_btn_create(lv_scr_act(), nullptr);
    sizeBtn->user_data = this;
    lv_obj_set_event_cb(sizeBtn, btnEventHandler);
    lv_obj_set_size(sizeBtn, 200, 75);
    lv_obj_add_style(sizeBtn, LV_BTN_PART_MAIN, &buttonStyle);
    lv_obj_align(sizeBtn, nullptr, LV_ALIGN_IN_TOP_MID, 0, 0); // Center the button
    
    diffBtn = lv_btn_create(lv_scr_act(), nullptr);
    diffBtn->user_data = this;
    lv_obj_set_event_cb(diffBtn, btnEventHandler);
    lv_obj_set_size(diffBtn, 200, 75);
    lv_obj_add_style(diffBtn, LV_BTN_PART_MAIN, &buttonStyle);
    lv_obj_align(diffBtn, nullptr, LV_ALIGN_CENTER, 0, 5); // Center the button

    
    
    

    lv_obj_t* btnLabel = lv_label_create(startBtn, nullptr);
    lv_label_set_text(btnLabel, "Start Game");
        lv_obj_add_style(btnLabel, LV_BTN_PART_MAIN, &labelStyle);
    diffLabel = lv_label_create(diffBtn, nullptr);
    lv_label_set_text(diffLabel, "Easy");
        lv_obj_add_style(diffLabel, LV_BTN_PART_MAIN, &labelStyle);
    sizeLabel = lv_label_create(sizeBtn, nullptr);
    lv_label_set_text(sizeLabel, "10x10");
    lv_obj_add_style(sizeLabel, LV_BTN_PART_MAIN, &labelStyle);
    
    speedSnake = 1000;
    timeGoDown = 15;
    gridSize = 10;
    cellSize = (240 / gridSize);
    
}

void Snake::OnButtonEvent(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    if (obj == startBtn) {
    cellSize = (240 / gridSize);
    segmentSize = cellSize;
    snakeBigBoi = (4 * cellSize) / 5;
      startGame();
    return;
    }
    if (obj == diffBtn) {
    labelVal = (labelVal + 1) % 4;
      cycleDiff();
      currentInterval = speedSnake;
      return;
    }
    if (obj == sizeBtn) {
    labelValS = (labelValS + 1) % 5;
      cycleSize();
      return;
    }
  }
}

void Snake::cycleSize() {
    switch (labelValS) {
      case 0:
          lv_label_set_text(sizeLabel, "10x10");
          gridSize = 10;
          break;
      case 1:
          lv_label_set_text(sizeLabel, "16x16");
          gridSize = 16;
          break;
      case 2:
          lv_label_set_text(sizeLabel, "24x24");
          gridSize = 24;
          break;
      case 3:
          lv_label_set_text(sizeLabel, "48x48");
          gridSize = 48;
          break;
      case 4:
          lv_label_set_text(sizeLabel, "6x6");
          gridSize = 6;
          break;
          }
  }

  void Snake::cycleDiff() {
    switch (labelVal) {
      case 0:
          lv_label_set_text(diffLabel, "Easy");
          speedSnake = 1000;
          timeGoDown = 15;
          break;
      case 1:
          lv_label_set_text(diffLabel, "Medium");
          speedSnake = 800;
          timeGoDown = 30;
          break;
      case 2:
          lv_label_set_text(diffLabel, "Hard");
          speedSnake = 650;
          timeGoDown = 40;
          break;
      case 3:
          lv_label_set_text(diffLabel, "Maybe Not...");
          speedSnake = 400;
          timeGoDown = 50;
          break;
          }
  }
          
void Snake::startGame() {
    lv_obj_clean(lv_scr_act()); // Clear the start screen
    isGameStarted = true;  // Mark the game as started


    // Initialize the game objects
    for (int i = 0; i < 5; ++i) {
        snakePoints.push_back({static_cast<lv_coord_t>(120 + (cellSize / 2)), static_cast<lv_coord_t>(120 + (cellSize / 2))});
 // Start at the center (125, 125)
    }

    lv_style_init(&snakeStyle);
    lv_style_set_line_width(&snakeStyle, LV_STATE_DEFAULT, snakeBigBoi); // Set line width (cellSize * (4 / 5))
    lv_style_set_line_color(&snakeStyle, LV_STATE_DEFAULT, LV_COLOR_LIME); // Set line color
    lv_style_set_line_rounded(&snakeStyle, LV_STATE_DEFAULT, true);

    line = lv_line_create(lv_scr_act(), nullptr); // Create the snake
    lv_line_set_points(line, snakePoints.data(), snakePoints.size());
    lv_obj_add_style(line, LV_LINE_PART_MAIN, &snakeStyle);
    lv_obj_set_pos(line, 0, 0); // Set snake position

    lv_style_init(&foodStyle);
    lv_style_set_bg_color(&foodStyle, LV_STATE_DEFAULT, LV_COLOR_RED); // Set food color
    lv_style_set_radius(&foodStyle, LV_STATE_DEFAULT, (cellSize / 2)); // Set food as a circle

    food = lv_obj_create(lv_scr_act(), nullptr);
    lv_obj_add_style(food, LV_OBJ_PART_MAIN, &foodStyle);
    lv_obj_set_size(food, cellSize, cellSize); // Set food size

    movementTask = lv_task_create(snakeMoveTask, currentInterval, LV_TASK_PRIO_MID, this);
    teleportFood();
}


bool Snake::OnTouchEvent(TouchEvents event) {
    if (!isGameStarted || isGameOver) {
        return false; // Ignore touch events when the game is over or not started
    }

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
    return true;
}

// ... (rest of your existing game logic remains the same)


void Snake::moveSnake() {
  // Get the position of the head (first segment)
  lv_coord_t headX = snakePoints.front().x;
    lv_coord_t headY = snakePoints.front().y;
  // Calculate new head position based on the last direction
 
    switch (lastDirection) {
        case Direction::Up:
            headY -= cellSize; // Move up
            break;
        case Direction::Down:
            headY += cellSize; // Move down
            break;
        case Direction::Left:
            headX -= cellSize; // Move left
            break;
        case Direction::Right:
            headX += cellSize; // Move right
            break;
    }

    // Check for wall collisions
    if (headX < 0 || headX >= 240 || headY < 0 || headY >= 240) {
        endGame(); // End the game if the snake hits a wall
        return; // Exit the function
    }

    // Check for body collisions
    for (size_t i = 1; i < snakePoints.size(); ++i) {
        if (snakePoints[i].x == headX && snakePoints[i].y == headY) {
            endGame(); // End the game if the snake collides with its body
            return; // Exit the function
        }
    }
 // Check for collision with food
  if (checkCollision(headX, headY)) {
    // Add a new segment at the new head position
    addSegment(headX, headY);
    currentInterval = std::max((speedSnake * (1 / 5)), currentInterval - timeGoDown);
    lv_task_set_period(movementTask, currentInterval);
    teleportFood(); // Teleport the food to a new position
  } else {
    // Normal movement without eating food
    addSegment(headX, headY);
    removeSegment(); // Remove the last segment
  }
}

void Snake::addSegment(int x, int y) {
  // Insert new head position
  snakePoints.insert(snakePoints.begin(), {static_cast<lv_coord_t>(x), static_cast<lv_coord_t>(y)});

  // Update the line points
  lv_line_set_points(line, snakePoints.data(), snakePoints.size()); // Update the line with new points
}

void Snake::removeSegment() {
  if (snakePoints.size() > 1) { // Keep at least one segment
    snakePoints.pop_back(); // Remove the last segment
    lv_line_set_points(line, snakePoints.data(), snakePoints.size()); // Update the line with the new points
  }
}

void Snake::teleportFood() {
    int foodX, foodY;
    bool collision;

    do {
        // Randomize food position within the grid
        foodX = (rand() % gridSize) * cellSize; 
        foodY = (rand() % gridSize) * cellSize; 

        // Check for collision with the snake, considering the 5-pixel offset
        collision = false;
        for (const auto& segment : snakePoints) {
            if (segment.x == foodX && segment.y == foodY) {
                collision = true; // Food overlaps with a snake segment
                break; // No need to check further
            }
            // Check against the position where the snake segment would visually overlap the food
            if (segment.x - (cellSize / 2) == foodX && segment.y - (cellSize / 2) == foodY) {
                collision = true; // Food overlaps with a snake segment accounting for the offset
                break; // No need to check further
            }
        }
    } while (collision); // Repeat until we find a valid position

    // Set the randomized position of the food
    lv_obj_set_pos(food, foodX, foodY);  
}


bool Snake::checkCollision(int headX, int headY) {
  lv_coord_t foodX = lv_obj_get_x(food);
  lv_coord_t foodY = lv_obj_get_y(food);

  // Check if the snake's head is at the same position as the food
  return (headX - (cellSize / 2) == foodX && headY - (cellSize / 2) == foodY);
}
void Snake::endGame() {
  lv_style_set_line_color(&snakeStyle, LV_STATE_DEFAULT, LV_COLOR_GREEN);  // Set line color
  lv_obj_add_style(line, LV_LINE_PART_MAIN, &snakeStyle);
    isGameOver = true; // Add a member variable to track game state
}

