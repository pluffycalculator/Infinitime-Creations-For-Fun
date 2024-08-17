#include "displayapp/screens/TicTacToe.h"
#include "components/motor/MotorController.h"
#include "components/datetime/DateTimeController.h"
#include "displayapp/LittleVgl.h"
#include "components/fs/FS.h"
#include "Symbols.h"

namespace {
    constexpr int CELL_SIZE = 80; // Each cell is 80x80 pixels
    constexpr int GRID_ROWS = 3;
    constexpr int GRID_COLS = 3;
}

namespace Pinetime::Applications::Screens {

   TicTacToe::TicTacToe(Controllers::MotorController& motorController,
                     Controllers::DateTime& dateTimeController,
                     Controllers::FS& filesystem)
    : motorController(motorController), // Initialize motorController first
      dateTimeController(dateTimeController), // Initialize dateTimeController second
      filesystem(filesystem),
      currentPlayer('X'),
      overlay(nullptr),
      gameOver(false) {
    InitializeBoard();

    lv_obj_t* parent = lv_scr_act(); // Use the active screen as the parent
    
    // Create the overlay background
    overlay = lv_obj_create(parent, nullptr);
    lv_obj_set_size(overlay, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_local_bg_color(overlay, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_opa(overlay, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70); // Semi-transparent
    lv_obj_set_hidden(overlay, true); // Hide the overlay initially

    for (int row = 0; row < GRID_ROWS; ++row) {
        for (int col = 0; col < GRID_COLS; ++col) {
            grid[row][col] = lv_cont_create(parent, nullptr);
            lv_obj_set_size(grid[row][col], CELL_SIZE, CELL_SIZE);
            lv_obj_set_pos(grid[row][col], col * CELL_SIZE, row * CELL_SIZE);
            lv_obj_set_style_local_border_width(grid[row][col], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
            lv_obj_set_style_local_border_color(grid[row][col], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
            lv_obj_set_style_local_bg_color(grid[row][col], LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

            lv_obj_t* label = lv_label_create(grid[row][col], nullptr);
            lv_label_set_text(label, " ");
            lv_obj_align(label, nullptr, LV_ALIGN_CENTER, 0, 0);
        }
    }
}


    TicTacToe::~TicTacToe() {
    for (int row = 0; row < GRID_ROWS; ++row) {
        for (int col = 0; col < GRID_COLS; ++col) {
            if (grid[row][col]) {
                lv_obj_del(grid[row][col]);
                grid[row][col] = nullptr;
            }
        }
    }

    if (overlay) {
        lv_obj_del(overlay);
    }
}


    void TicTacToe::InitializeBoard() {
        for (auto& row : board) {
            row.fill(' ');
        }
    }
    
    bool TicTacToe::OnTouchEvent(Pinetime::Applications::TouchEvents /*event*/) {
  return true;
}

    bool TicTacToe::OnTouchEvent(uint16_t x, uint16_t y) {
            if (gameOver) {
                gameOver = false;
                InitializeBoard();
                Refresh();
                lv_obj_set_hidden(overlay, true); // Hide the overlay
            return true;
            }

        int row = y / CELL_SIZE;
        int col = x / CELL_SIZE;

        if (row < 0 || row >= GRID_ROWS || col < 0 || col >= GRID_COLS) {
            return false;
        }

        if (board[row][col] == ' ') {
            board[row][col] = currentPlayer;
            if (CheckWin()) {
                gameOver = true;
            } else {
                SwitchPlayer();
            }
            Refresh();
            Vibrate();
        }

        return true;
    }

    void TicTacToe::SwitchPlayer() {
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }

    void TicTacToe::Refresh() {
        for (int row = 0; row < GRID_ROWS; ++row) {
            for (int col = 0; col < GRID_COLS; ++col) {
                lv_obj_t* label = lv_obj_get_child(grid[row][col], nullptr);
                if (label) {
                    lv_label_set_text_fmt(label, "%c", board[row][col]);
                    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
                    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
                }
            }
        }
    }

    bool TicTacToe::CheckWin() {
    char winner = ' ';

    // Check rows for a win
    for (int row = 0; row < GRID_ROWS; ++row) {
        if (board[row][0] != ' ' &&
            board[row][0] == board[row][1] &&
            board[row][1] == board[row][2]) {
            winner = board[row][0];
            DisplayWinner(winner);
            return true;
        }
    }

    // Check columns for a win
    for (int col = 0; col < GRID_COLS; ++col) {
        if (board[0][col] != ' ' &&
            board[0][col] == board[1][col] &&
            board[1][col] == board[2][col]) {
            winner = board[0][col];
            DisplayWinner(winner);
            return true;
        }
    }

    // Check the first diagonal (top-left to bottom-right) for a win
    if (board[0][0] != ' ' &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2]) {
        winner = board[0][0];
        DisplayWinner(winner);
        return true;
    }

    // Check the second diagonal (top-right to bottom-left) for a win
    if (board[0][2] != ' ' &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0]) {
        winner = board[0][2];
        DisplayWinner(winner);
        return true;
    }

    // No win detected
    return false;
}
    
  void TicTacToe::DisplayWinner(char winner) {
    lv_obj_set_hidden(overlay, false); // Show the overlay
    lv_obj_move_foreground(overlay);   // Ensure overlay is in front of other elements

    lv_obj_t* label = lv_label_create(overlay, nullptr); // Create the label on the overlay
    lv_label_set_text_fmt(label, "%c Won!", winner);
    lv_obj_align(label, nullptr, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);

    lv_obj_move_foreground(label);  // Move the label in front of the overlay

    gameOver = true; // Mark the game as over
}





    void TicTacToe::Vibrate() {
       motorController.RunForDuration(50);
    }

}

