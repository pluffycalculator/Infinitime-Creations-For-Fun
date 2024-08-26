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
        void moveSnake(); // Changed from static to non-static
        void gameOver();
        int currentInterval;
        static void gracePeriodTimeout(lv_task_t* task);
      private:
       static constexpr int nColors = 4;
        lv_style_t cellStyles[nColors];
        
        static constexpr int nCols = 11;
        static constexpr int nRows = 11;
        static constexpr int nCells = nCols * nRows;

        lv_obj_t* gridDisplay;
        SnakeTile grid[nRows][nCols];

        enum class Direction { Up, Down, Left, Right };
        Direction lastDirection = Direction::Right;
        std::vector<std::pair<int, int>> snakeBody; // Store snake segments as (row, col)
        size_t snakeLength = 1; // Start length of the snake
        lv_task_t* movementTask; // To handle periodic movement
        bool isGameOver = false; // Flag to track game state
        bool inGracePeriod = false;
        lv_task_t* gracePeriodTask = nullptr;


        void updateGridDisplay();
        void placeFood();
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

