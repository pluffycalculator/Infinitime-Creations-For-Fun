#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include <vector>

namespace Pinetime {
  namespace Applications {
    struct SnakeTile {
      enum class Type { Empty, SnakeBody, SnakeHead, Food };
      Type type = Type::Empty;
    };
    
    namespace Screens {
      class Snake : public Screen {
      public:
        Snake();
        ~Snake() override;

        bool OnTouchEvent(TouchEvents event) override;
        void moveSnake();
        void gameOver();
        int currentInterval;
        static void gracePeriodTimeout(lv_task_t* task);
      private:
        lv_style_t snakeStyle;
        lv_style_t foodStyle;

        static constexpr int nCols = 240;
        static constexpr int nRows = 240;
        static constexpr int nCells = nCols * nRows;
        static constexpr int cellSize = 1;  // Adjust as needed for the size of each cell

        SnakeTile grid[nRows][nCols];

        enum class Direction { Up, Down, Left, Right };
        Direction lastDirection = Direction::Right;
        std::vector<std::pair<int, int>> snakeBody;  // Store snake segments as (row, col)
        size_t snakeLength = 1;  // Start length of the snake
        lv_task_t* movementTask;  // To handle periodic movement
        bool isGameOver = false;  // Flag to track game state
        bool inGracePeriod = false;
        lv_task_t* gracePeriodTask = nullptr;

        std::vector<lv_point_t> snakePoints;  // Store points to draw the snake
        lv_obj_t* snakeLine;  // lv_line object to draw the snake
        lv_obj_t* foodObj;  // lv_line object to draw the food

        void updateGridDisplay();
        void placeFood();
        void drawSnake();
        void clearSnake();
        void drawFood(int row, int col);
        bool checkCollision(int newRow, int newCol);
      };
    }

    template <>
    struct AppTraits<Apps::Snake> {
      static constexpr Apps app = Apps::Snake;
      static constexpr const char* icon = "S";

      static Screens::Screen* Create(AppControllers& /*controllers*/) {
        return new Screens::Snake();
      };
    };
  }
}

    

