#pragma once

#include "systemtask/SystemTask.h"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include <vector>

#include "displayapp/screens/CheckboxList.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Snake : public Screen {
      public:
        Snake();
        ~Snake() override;
        void OnButtonEvent(lv_obj_t* obj, lv_event_t event);
        bool OnTouchEvent(TouchEvents event) override;
        void createStartScreen();
        void cycleDiff();
        void cycleSize();
        void startGame();
        void moveSnake(); // Changed from static to non-static
        
        static int segmentSize; // Size of each segment
        
        int labelVal = 0;
        int labelValS = 0;
        
        int gridSize;
        int cellSize;
        int snakeBigBoi;
        int speedSnake;
        int timeGoDown;
        
        lv_obj_t* sizeBtn;
        lv_obj_t* sizeLabel;
        lv_obj_t* diffBtn;
        lv_obj_t* diffLabel;
      
      private:
        void addSegment(int x, int y);
        void removeSegment();
        void teleportFood(); // New method to randomize food position
        bool checkCollision(int headX, int headY); // New method to check for collision
        int currentInterval;
        void endGame();
        bool isGameOver = false; // Track the game state
        bool isGameStarted = false; // Track the game state

        enum class Direction { Up, Down, Left, Right };
        Direction lastDirection = Direction::Right;
        
        std::vector<lv_point_t> snakePoints; // Vector to hold the snake points
        
        lv_task_t* movementTask; // To handle periodic movement
      lv_obj_t* startBtn;
      lv_obj_t* line;
      lv_obj_t* food;
      lv_style_t snakeStyle;
      lv_style_t foodStyle;
      lv_style_t buttonStyle;
      lv_style_t labelStyle;
      };

    } // namespace Screens
    
    template <>
  struct AppTraits<Apps::Snake> {
    static constexpr Apps app = Apps::Snake;
    static constexpr const char* icon = "S";

    static Screens::Screen* Create(AppControllers& controllers) {
      return new Screens::Snake();
    };
  };
  
  } // namespace Applications
} // namespace Pinetime

