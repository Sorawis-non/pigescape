#include "raylib.h"
#include <math.h>
#include "gameplay.h" 

float GRID_OFFSET_X = 0;
float GRID_OFFSET_Y = 0;
float VISUAL_CELL_SIZE = 0;
float VISUAL_CELL_SIZE_Y = 0;

enum GameScreen { SCREEN_LOGIN, SCREEN_GAMEPLAY };

int main(){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); 
    InitWindow(1500, 800, "เกมหมูหนี");
    SetTargetFPS(120);

    // ==========================================
    // 🌟 โหลดรูปภาพทั้งหมดไว้ที่หน้า FRONT ที่เดียว!
    // ==========================================
    // 1. กลุ่มรูปภาพหน้าเมนู
    Texture2D BG_picture = LoadTexture("picture/BG.png");
    Texture2D PIG_ESCAPE_picture = LoadTexture("picture/PIG escape.png");
    Texture2D NAME_picture = LoadTexture("picture/NAME.png");
    Texture2D START_GAME_picture = LoadTexture("picture/Start game.png");

    // 2. กลุ่มรูปภาพหน้าเล่นเกม (อิงจากภาพตัวอย่างใหม่ที่เพื่อนให้มา)
    Texture2D GAMEPLAY_BG_picture   = LoadTexture("picture/BG2.png");
    Texture2D GRID_picture          = LoadTexture("picture/grid.png");
    Texture2D BLOCK_PIG_picture     = LoadTexture("picture/block_pig.png");   
    Texture2D BLOCK_WOLF_H_picture  = LoadTexture("picture/block_wolf_h.png");  
    Texture2D BLOCK_WOLF_V_picture  = LoadTexture("picture/block_wolf_v.png");
    Texture2D BLOCK_STONE_H_picture = LoadTexture("picture/block_stone_h.png");  
    Texture2D BLOCK_STONE_V_picture = LoadTexture("picture/block_stone_v.png");
    Texture2D BLOCK_BEAR_picture    = LoadTexture("picture/block_bear.png");  

    bool isNameInputActive = false; 
    int framesCounter = 0;
    int backspaceFrames = 0; // ตัวแปรสำหรับระบบลบค้าง
    GameScreen currentScreen = SCREEN_LOGIN;

    char nameInput[50] = "\0"; 
    int letterCount = 0;

    while(!WindowShouldClose()) {

        if (currentScreen == SCREEN_LOGIN) {
            // ==========================================
            // RENDER: หน้า LOGIN
            // ==========================================
            Rectangle source = { 0.0, 0.0, (float)BG_picture.width, (float)BG_picture.height };
            Rectangle dest = { 0.0, 0.0, (float)GetScreenWidth(), (float)GetScreenHeight() };

            int floatOffsetY = (int)(sin(GetTime() * 3.0) * 10.0);
            int PIG_ESCAPE_picture_X = (GetScreenWidth() - PIG_ESCAPE_picture.width) / 2;
            int PIG_ESCAPE_picture_Y = 150 + floatOffsetY;
            int NAME_picture_X = (GetScreenWidth() - NAME_picture.width) / 2;
            int NAME_picture_Y = 850; 
            int START_GAME_picture_x = (GetScreenWidth() - START_GAME_picture.width) / 2;
            int START_GAME_picture_Y = 1200; 

            Vector2 mousePoint = GetMousePosition();
            Rectangle startButtonRec = { (float)START_GAME_picture_x, (float)START_GAME_picture_Y, (float)START_GAME_picture.width, (float)START_GAME_picture.height };
            Color startButtonColor = WHITE;
            
            if (CheckCollisionPointRec(mousePoint, startButtonRec)) {
                startButtonColor = GRAY;   
                START_GAME_picture_Y -= 10;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    InitGame(); 
                    currentScreen = SCREEN_GAMEPLAY; 
                }
            }

            Rectangle nameBoxRec = { (float)NAME_picture_X, (float)NAME_picture_Y, (float)NAME_picture.width, (float)NAME_picture.height };
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePoint, nameBoxRec)) isNameInputActive = true; 
                else isNameInputActive = false;
            }

            // ระบบรับค่าพิมพ์ชื่อ
            if (isNameInputActive) {
                framesCounter++;
                int key = GetCharPressed();
                while (key > 0) {
                    if ((key >= 32) && (key <= 125) && (letterCount < 49)) {
                        nameInput[letterCount] = (char)key;
                        nameInput[letterCount + 1] = '\0'; 
                        
                        int currentTextWidth = MeasureText(nameInput, 60);
                        int maxBoxWidth = 600; // Limit ความกว้างกล่อง
                        
                        if (currentTextWidth > maxBoxWidth) {
                            nameInput[letterCount] = '\0'; 
                        } else {
                            letterCount++;
                        }
                    }
                    key = GetCharPressed(); 
                }

                // ระบบกดลบแบบค้าง
                if (IsKeyDown(KEY_BACKSPACE)) {
                    backspaceFrames++;
                    if (backspaceFrames == 1 || (backspaceFrames > 30 && backspaceFrames % 4 == 0)) {
                        letterCount--;
                        if (letterCount < 0) letterCount = 0;
                        nameInput[letterCount] = '\0';
                    }
                } else {
                    backspaceFrames = 0;
                }
            } else {
                framesCounter = 0;
            }

            BeginDrawing();
            ClearBackground(WHITE); 
            DrawTexturePro(BG_picture, source, dest, {0, 0}, 0.0, WHITE);
            DrawTexture(PIG_ESCAPE_picture, PIG_ESCAPE_picture_X, PIG_ESCAPE_picture_Y, WHITE);
            DrawTexture(NAME_picture, NAME_picture_X, NAME_picture_Y, WHITE);
            DrawTexture(START_GAME_picture, START_GAME_picture_x, START_GAME_picture_Y, startButtonColor);

            DrawText(nameInput, NAME_picture_X + 50, NAME_picture_Y + 130, 60, BLACK); 

            if (isNameInputActive) {
                if ((framesCounter / 60) % 2 == 0) {
                    int textWidth = MeasureText(nameInput, 60); 
                    DrawText("|", NAME_picture_X + 50 + textWidth, NAME_picture_Y + 130, 60, BLACK); 
                }
            }
            EndDrawing();

        } else if (currentScreen == SCREEN_GAMEPLAY) {
            // ==========================================
            // RENDER: หน้า GAMEPLAY
            // ==========================================
            float gridScale = 0.22f;
            float scaledGridWidth  = GRID_picture.width  * gridScale;
            float scaledGridHeight = GRID_picture.height * gridScale;
            float GRID_picture_X = (GetScreenWidth()  - scaledGridWidth)  / 2.0f;
            float GRID_picture_Y = (GetScreenHeight() - scaledGridHeight) / 2.0f;

            float marginLeft   = scaledGridWidth  * 0.295f;
            float marginTop    = scaledGridHeight * 0.085f;
            float marginRight  = scaledGridWidth  * 0.295f;  // เพิ่มจาก 0.110 → 0.295
            float marginBottom = scaledGridHeight * 0.085f;

            float playableW = scaledGridWidth  - marginLeft - marginRight;
            float playableH = scaledGridHeight - marginTop  - marginBottom;

            GRID_OFFSET_X      = GRID_picture_X + marginLeft;
            GRID_OFFSET_Y      = GRID_picture_Y + marginTop;
            VISUAL_CELL_SIZE   = playableW / 6.0f;
            VISUAL_CELL_SIZE_Y = playableH / 6.0f;

            UpdateGameplay(); // สั่งรัน Logic หลังบ้าน

            BeginDrawing();
            ClearBackground(RAYWHITE); 


            // วาดพื้นหลังกระดานเกม (แผ่นป่า)
            Rectangle gameSource = { 0.0, 0.0, (float)GAMEPLAY_BG_picture.width, (float)GAMEPLAY_BG_picture.height };
            Rectangle gameDest = { 0.0, 0.0, (float)GetScreenWidth(), (float)GetScreenHeight() };
            DrawTexturePro(GAMEPLAY_BG_picture, gameSource, gameDest, {0, 0}, 0.0, WHITE);
             

            // วาดตารางด้วย DrawTextureEx เพื่อบังคับสเกล
            DrawTextureEx(GRID_picture, { (float)GRID_picture_X, (float)GRID_picture_Y }, 0.0f, gridScale, WHITE);
            DrawRectangleLines(GRID_OFFSET_X, GRID_OFFSET_Y, VISUAL_CELL_SIZE * 6, VISUAL_CELL_SIZE_Y * 6, BLUE);
            DrawText(TextFormat("OX:%.0f OY:%.0f CS:%.0f", GRID_OFFSET_X, GRID_OFFSET_Y, VISUAL_CELL_SIZE), 20, 140, 25, YELLOW);
            DrawText(TextFormat("GW:%.0f GH:%.0f GX:%.0f GY:%.0f", scaledGridWidth, scaledGridHeight, GRID_picture_X, GRID_picture_Y), 20, 170, 25, YELLOW);

            // โชว์ UI ตัวอักษร
            DrawText(TextFormat("Player: %s", nameInput), 20, 100, 30, ORANGE);
            DrawText(TextFormat("Level: %d", game.getCurrentLevelId()), 20, 20, 30, DARKGRAY);
            DrawText(TextFormat("Moves: %d / %d", game.getCurrentMove(), game.getMoveLimit()), 20, 60, 30, DARKBLUE);

            const char* timeText = TextFormat("Time: %d s", game.getTimeRemaining());
            int timeTextWidth = MeasureText(timeText, 40); // วัดความกว้างของข้อความ
            DrawText(timeText, (GetScreenWidth() - timeTextWidth) / 2, 20, 40, RED);

            // วาดช่องทางออกสีเขียว
            DrawRectangle(
                GRID_OFFSET_X + game.getExitX() * VISUAL_CELL_SIZE,
                GRID_OFFSET_Y + game.getExitY() * VISUAL_CELL_SIZE_Y,  // ✅
                VISUAL_CELL_SIZE, VISUAL_CELL_SIZE_Y, GREEN             // ✅
            );

            // ========================================================
            // 🌟 ระบบวาดรูปตัวละครทับลงบนกล่อง
            // ========================================================
            for (GamePiece* p : game.getPieces()) {
                int px = GRID_OFFSET_X + (p->getX() * VISUAL_CELL_SIZE);
                int py = GRID_OFFSET_Y + (p->getY() * VISUAL_CELL_SIZE_Y);  // ✅
                int w  = (p->getAxis() == Orientation::HORIZONTAL) ? p->getLength() * VISUAL_CELL_SIZE   : VISUAL_CELL_SIZE;
                int h  = (p->getAxis() == Orientation::VERTICAL)   ? p->getLength() * VISUAL_CELL_SIZE_Y : VISUAL_CELL_SIZE_Y;  // ✅


                Color boxColor = (p->getId() == 'P') ? PINK : (p->getLength() == 2) ? ORANGE : (p->getLength() == 3) ? GRAY : SKYBLUE;
                DrawRectangle(px + 2, py + 2, w - 4, h - 4, boxColor);

                Rectangle destRec = { (float)px + 2, (float)py + 2, (float)w - 4, (float)h - 4 };
                Vector2 origin = { 0.0f, 0.0f };

                if (p->getId() == 'P') {
                    Rectangle sourceRec = { 0.0f, 0.0f, (float)BLOCK_PIG_picture.width, (float)BLOCK_PIG_picture.height };
                    DrawTexturePro(BLOCK_PIG_picture, sourceRec, destRec, origin, 0.0f, WHITE);
                } else if (p->getLength() == 2 && p->getAxis() == Orientation::HORIZONTAL) {
                    Rectangle sourceRec = { 0.0f, 0.0f, (float)BLOCK_WOLF_H_picture.width, (float)BLOCK_WOLF_H_picture.height };
                    DrawTexturePro(BLOCK_WOLF_H_picture, sourceRec, destRec, origin, 0.0f, WHITE);
                } else if (p->getLength() == 2 && p->getAxis() == Orientation::VERTICAL) {
                    Rectangle sourceRec = { 0.0f, 0.0f, (float)BLOCK_WOLF_V_picture.width, (float)BLOCK_WOLF_V_picture.height };
                    DrawTexturePro(BLOCK_WOLF_V_picture, sourceRec, destRec, origin, 0.0f, WHITE);
                } else if (p->getLength() == 3 && p->getAxis() == Orientation::HORIZONTAL) {
                    Rectangle sourceRec = { 0.0f, 0.0f, (float)BLOCK_STONE_H_picture.width, (float)BLOCK_STONE_H_picture.height };
                    DrawTexturePro(BLOCK_STONE_H_picture, sourceRec, destRec, origin, 0.0f, WHITE);
                } else if (p->getLength() == 3 && p->getAxis() == Orientation::VERTICAL) {
                    Rectangle sourceRec = { 0.0f, 0.0f, (float)BLOCK_STONE_V_picture.width, (float)BLOCK_STONE_V_picture.height };
                    DrawTexturePro(BLOCK_STONE_V_picture, sourceRec, destRec, origin, 0.0f, WHITE);
                } else if (p->getLength() == 4) {
                    Rectangle sourceRec = { 0.0f, 0.0f, (float)BLOCK_BEAR_picture.width, (float)BLOCK_BEAR_picture.height };
                    DrawTexturePro(BLOCK_BEAR_picture, sourceRec, destRec, origin, 0.0f, WHITE);
                }

                // เส้นขอบสีแดง เอาไว้เล็ง Grid ล่องหน
                DrawRectangleLines(px, py, w, h, RED); 
            }

            if (game.getGameState() == GameState::WON) {
                int msgFontSize = 30;
                const char* msg = "CLEARED! Press SPACE to continue";
                int msgW = MeasureText(msg, msgFontSize);
                int boxW = msgW + 60;
                int boxH = 80;
                int boxX = (GetScreenWidth()  - boxW) / 2;
                int boxY = (GetScreenHeight() - boxH) / 2;
                DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.8f));
                DrawText(msg, boxX + 30, boxY + 25, msgFontSize, GREEN);
                if (IsKeyPressed(KEY_SPACE)) {
                currentScreen = SCREEN_LOGIN;
                nameInput[0] = '\0';
                letterCount = 0;
            }
                if (IsKeyPressed(KEY_R)) game.startGame(game.getCurrentLevelId());
            } else if (game.getGameState() == GameState::LOSE_MOVE || game.getGameState() == GameState::LOSE_TIME) {
                int msgFontSize = 30;
                const char* msg = "GAME OVER! Press R to Restart";
                int msgW = MeasureText(msg, msgFontSize);
                int boxW = msgW + 60;
                int boxH = 80;
                int boxX = (GetScreenWidth()  - boxW) / 2;
                int boxY = (GetScreenHeight() - boxH) / 2;
                DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.8f));
                DrawText(msg, boxX + 30, boxY + 25, msgFontSize, RED);
                if (IsKeyPressed(KEY_R)) game.startGame(game.getCurrentLevelId());
            }

            EndDrawing();
        }
    }

    // ==========================================
    // ลบรูปภาพออกจากแรมก่อนปิดโปรแกรม
    // ==========================================
    UnloadTexture(BG_picture);
    UnloadTexture(PIG_ESCAPE_picture);
    UnloadTexture(NAME_picture);
    UnloadTexture(START_GAME_picture);
    UnloadTexture(GAMEPLAY_BG_picture);
    UnloadTexture(BLOCK_PIG_picture);
    UnloadTexture(BLOCK_WOLF_H_picture);
    UnloadTexture(BLOCK_WOLF_V_picture);
    UnloadTexture(BLOCK_STONE_H_picture);
    UnloadTexture(BLOCK_STONE_V_picture);
    UnloadTexture(BLOCK_BEAR_picture);
    
    CloseWindow();
    return 0;
}