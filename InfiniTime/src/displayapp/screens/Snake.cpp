#include "displayapp/screens/Snake.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <lvgl/lvgl.h>

using namespace Pinetime::Applications::Screens;

void snakeMoveTask(lv_task_t* task) {
    Snake* snake = static_cast<Snake*>(task->user_data);
    snake->moveSnake(); // Call the member function
}

      Snake::Snake() : currentInterval(1000) {
        srand(static_cast<unsigned int>(time(nullptr)));
        
        for (int i = 0; i < 5; ++i) {
    snakePoints.push_back({125, 125}); // Start at the center (120, 120)
  }
      
      lv_style_init(&snakeStyle);
  // Set snakeStyle properties
        lv_style_set_line_width(&snakeStyle, LV_STATE_DEFAULT, 8);  // Set line width
        lv_style_set_line_color(&snakeStyle, LV_STATE_DEFAULT, LV_COLOR_LIME);  // Set line color
        lv_style_set_line_rounded(&snakeStyle, LV_STATE_DEFAULT, true);
        
       //static lv_point_t line_points[] = {{0, 0}, {10, 0}}; //size of initial snake     
  
       line = lv_line_create(lv_scr_act(), nullptr); //create snake
       lv_line_set_points(line, snakePoints.data(), snakePoints.size());
       lv_obj_add_style(line, LV_LINE_PART_MAIN, &snakeStyle);
      lv_obj_set_pos(line, 0, 0); //set snake position
      
      // Initialize the food style
  lv_style_init(&foodStyle);
  lv_style_set_bg_color(&foodStyle, LV_STATE_DEFAULT, LV_COLOR_RED);  // Set food color
  lv_style_set_radius(&foodStyle, LV_STATE_DEFAULT, 5);  // Set the radius for the circle

      
  // Create a new object for the food (as a circle)
  food = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_add_style(food, LV_OBJ_PART_MAIN, &foodStyle);
  lv_obj_set_size(food, 10, 10);  // Set the size of the food (diameter of 10 pixels)
  
  movementTask = lv_task_create(snakeMoveTask, currentInterval, LV_TASK_PRIO_MID, this);
  
  teleportFood();
      }


     Snake::~Snake() {
  lv_task_del(movementTask);
  lv_obj_clean(lv_scr_act());
  lv_style_reset(&snakeStyle);
  lv_style_reset(&foodStyle);
  
}

bool Snake::OnTouchEvent(TouchEvents event) {
if (isGameOver) {
        return false; // Ignore touch events when the game is over
    }
  // Check if the touch event is a swipe or tap for direction change
  if (event == TouchEvents::SwipeUp) {
    lastDirection = Direction::Up;
  } else if (event == TouchEvents::SwipeDown) {
    lastDirection = Direction::Down;
  } else if (event == TouchEvents::SwipeLeft) {
    lastDirection = Direction::Left;
  } else if (event == TouchEvents::SwipeRight) {
    lastDirection = Direction::Right;
  }

  // Move the snake in the last direction
  return true; // Indicate that the event was handled
}

void Snake::moveSnake() {
  // Get the position of the head (first segment)
  lv_coord_t headX = snakePoints.front().x;
    lv_coord_t headY = snakePoints.front().y;
  // Calculate new head position based on the last direction
 
    switch (lastDirection) {
        case Direction::Up:
            headY -= 10; // Move up
            break;
        case Direction::Down:
            headY += 10; // Move down
            break;
        case Direction::Left:
            headX -= 10; // Move left
            break;
        case Direction::Right:
            headX += 10; // Move right
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
    currentInterval = std::max(200, currentInterval - 5);
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
        foodX = (rand() % 24) * 10; 
        foodY = (rand() % 24) * 10; 

        // Check for collision with the snake, considering the 5-pixel offset
        collision = false;
        for (const auto& segment : snakePoints) {
            if (segment.x == foodX && segment.y == foodY) {
                collision = true; // Food overlaps with a snake segment
                break; // No need to check further
            }
            // Check against the position where the snake segment would visually overlap the food
            if (segment.x - 5 == foodX && segment.y - 5 == foodY) {
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
  return (headX - 5 == foodX && headY - 5 == foodY);
}
void Snake::endGame() {
  lv_style_set_line_color(&snakeStyle, LV_STATE_DEFAULT, LV_COLOR_GREEN);  // Set line color
  lv_obj_add_style(line, LV_LINE_PART_MAIN, &snakeStyle);
    isGameOver = true; // Add a member variable to track game state
}

