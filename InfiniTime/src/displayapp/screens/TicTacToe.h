#pragma once

#include <lvgl/lvgl.h>
#include <cstdint>
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"
#include <array>

namespace Pinetime {
    namespace Applications {
        namespace Screens {

            class TicTacToe : public Screen {
            public:
                TicTacToe(Controllers::MotorController& motorController,
                          Controllers::DateTime& dateTimeController,
                          Controllers::FS& filesystem);
                ~TicTacToe() override;

                bool OnTouchEvent(TouchEvents event) override;
                bool OnTouchEvent(uint16_t x, uint16_t y) override;
                void Refresh() override;
                bool CheckWin();
                void Vibrate();
                void SwitchPlayer();
                void DisplayWinner(char winner); // Add this declaration

            private:
                void InitializeBoard();
                void ClearBoard();

                Controllers::MotorController& motorController;
                Controllers::DateTime& dateTimeController;
                Controllers::FS& filesystem;
                
                char currentPlayer;
                
                lv_obj_t* overlay; 

                bool gameOver; // Add this member variable to track game state
                std::array<std::array<char, 3>, 3> board;
                std::array<std::array<lv_obj_t*, 3>, 3> grid;
            };

        }  // namespace Screens

        template <>
        struct AppTraits<Apps::TicTacToe> {
            static constexpr Apps app = Apps::TicTacToe;
            static constexpr const char* icon = Screens::Symbols::grid;
            static Screens::Screen* Create(AppControllers& controllers) {
                return new Screens::TicTacToe(controllers.motorController,
                                              controllers.dateTimeController,
                                              controllers.filesystem);
            }
        };

    }  // namespace Applications
} 

