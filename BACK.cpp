#include "gameplay.h"

GameManager game; 

// ถูกเรียกใช้แค่ครั้งเดียวตอนกดปุ่ม Start
void InitGame() {
    game.startGame(1); 
}

// ระบบคิดคำนวณลอจิกการขยับบล็อก (ไม่มีคำสั่งวาดภาพปนเลย)
void UpdateGameplay() {
    game.updateTime(GetFrameTime());

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && game.getGameState() == GameState::PLAYING) {
        Vector2 mouse = GetMousePosition();
        int gridX = (int)((mouse.x - GRID_OFFSET_X) / VISUAL_CELL_SIZE);
        int gridY = (int)((mouse.y - GRID_OFFSET_Y) / VISUAL_CELL_SIZE_Y);  // ✅ เปลี่ยนตรงนี้ 

        char clickedId = ' ';
        for (GamePiece* p : game.getPieces()) {
            if (p->getAxis() == Orientation::HORIZONTAL) {
                if (gridY == p->getY() && gridX >= p->getX() && gridX < p->getX() + p->getLength()) {
                    clickedId = p->getId(); break;
                }
            } else { 
                if (gridX == p->getX() && gridY >= p->getY() && gridY < p->getY() + p->getLength()) {
                    clickedId = p->getId(); break;
                }
            }
        }
        if (clickedId == 'P') game.processMove(clickedId, 1);
        else if (clickedId != ' ') game.processMoveToEnd(clickedId); 
    }
}