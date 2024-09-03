#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include <vector>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Snake : public Screen {
      public:
        Snake();
        ~Snake() override;
        bool OnTouchEvent(TouchEvents event) override;
        void moveSnake(); // Changed from static to non-static
      private:
        void addSegment(int x, int y);
        void removeSegment();
        void teleportFood(); // New method to randomize food position
        bool checkCollision(int headX, int headY); // New method to check for collision
        int currentInterval;
        void endGame();
        bool isGameOver = false; // Track the game state

        enum class Direction { Up, Down, Left, Right };
        Direction lastDirection = Direction::Right;
        
        std::vector<lv_point_t> snakePoints; // Vector to hold the snake points
        static constexpr int segmentSize = 10; // Size of each segment
        
        lv_task_t* movementTask; // To handle periodic movement
      lv_obj_t* line;
      lv_obj_t* food;
      lv_style_t snakeStyle;
      lv_style_t foodStyle;
      };

    } // namespace Screens
    
    template <>
  struct AppTraits<Apps::Snake> {
    static constexpr Apps app = Apps::Snake;
    static constexpr const char* icon = "S";

    static Screens::Screen* Create(AppControllers& /*controllers*/) {
      return new Screens::Snake();
    };
  };
  
  } // namespace Applications
} // namespace Pinetime

