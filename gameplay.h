#pragma once
#include "raylib.h"
#include <vector>
#include <cctype>

using namespace std;

const int CELL_SIZE = 80; 
const int OFFSET_Y = 100; 

enum class Orientation { HORIZONTAL, VERTICAL };
enum class ObstructionType { FOX_2, WALL_3, BEAR_4 };
enum class GameState { PLAYING, WON, LOSE_MOVE, LOSE_TIME }; 

class Level {
private:
    int id, gridSize, moveLimit, timelimit; 
public:
    Level(int l_id = 1, int gSize = 6, int mLimit = 15, int tLimit = 60) {
        id = l_id; gridSize = gSize; moveLimit = mLimit; timelimit = tLimit;
    }
    int getId() const { return id; }
    int getGridSize() const { return gridSize; }
    int getMoveLimit() const { return moveLimit; }
    int getTimeLimit() const { return timelimit; }
};

class GamePiece {
protected:
    int x, y, length;  
    Orientation axis; 
    char id; 
public:
    GamePiece(int startX, int startY, int len, Orientation dir, char identifier) {
        x = startX; y = startY; length = len; axis = dir; id = identifier;
    }
    virtual ~GamePiece() = default;
    virtual void move(int steps) {
        if (axis == Orientation::HORIZONTAL) x += steps; 
        else y += steps; 
    }
    int getX() const { return x; }
    int getY() const { return y; }
    int getLength() const { return length; }
    Orientation getAxis() const { return axis; }
    char getId() const { return id; }
};

class Pig : public GamePiece {
public:
    Pig(int startX, int startY, Orientation dir) 
        : GamePiece(startX, startY, 1, dir, 'P') {}
    bool isRescued(int exitx, int exity) const { 
        return (x == exitx && y == exity); 
    }
};

class Obstruction : public GamePiece {
private:
    ObstructionType type;
public:
    Obstruction(int startX, int startY, int len, Orientation dir, char id, ObstructionType t)
        : GamePiece(startX, startY, len, dir, id) { type = t; }
    ObstructionType getType() const { return type; }
};

class Board {
private:
    int gridSize, exitx, exity;    
    vector<GamePiece*> pieces;
public:
    Board(int size, int eX, int eY) { gridSize = size; exitx = eX; exity = eY; }
    ~Board() {
        for (auto p : pieces) delete p;
        pieces.clear();
    } 
    void addPiece(GamePiece* piece) { pieces.push_back(piece); }
    vector<GamePiece*> getPieces() const { return pieces; }
    int getExitX() const { return exitx; }
    int getExitY() const { return exity; } 
};

class GameManager {
private:
    GameState state;
    int currentLevelId, currentMove, moveLimit;
    float timeRemaining;
    Board* board;

    bool canMove(GamePiece* p, int dir) {
        int newX = p->getX() + (p->getAxis() == Orientation::HORIZONTAL ? dir : 0);
        int newY = p->getY() + (p->getAxis() == Orientation::VERTICAL   ? dir : 0);
        if (p->getAxis() == Orientation::HORIZONTAL) {
            if (newX < 0 || newX + p->getLength() > 6) return false;
        } else {
            if (newY < 0 || newY + p->getLength() > 6) return false;
        }
        int ax1 = newX, ay1 = newY;
        int ax2 = newX + (p->getAxis() == Orientation::HORIZONTAL ? p->getLength() : 1) - 1;
        int ay2 = newY + (p->getAxis() == Orientation::VERTICAL   ? p->getLength() : 1) - 1;
        for (GamePiece* other : board->getPieces()) {
            if (other->getId() == p->getId()) continue;
            int bx1 = other->getX(), by1 = other->getY();
            int bx2 = bx1 + (other->getAxis() == Orientation::HORIZONTAL ? other->getLength() : 1) - 1;
            int by2 = by1 + (other->getAxis() == Orientation::VERTICAL   ? other->getLength() : 1) - 1;
            if (ax1 <= bx2 && ax2 >= bx1 && ay1 <= by2 && ay2 >= by1) return false;
        }
        return true;
    }

public:
    GameManager() { board = nullptr; state = GameState::PLAYING; }
    void startGame(int level) {
        currentLevelId = level; currentMove = 0; moveLimit =28; timeRemaining = 60.0f; state = GameState::PLAYING;
        if (board != nullptr) delete board;
    board = new Board(6, 5, 2); 

    // ==========================================
    // 🐷 1. น้องหมู (สีชมพูแนวนอนยาว 2)
    // ==========================================
    board->addPiece(new Pig(0, 2, Orientation::HORIZONTAL));

    // ==========================================
    // 🐺 2. แก๊งหมาป่า สีส้ม (ความยาว 2)
    // ==========================================
    board->addPiece(new Obstruction(1, 0, 2, Orientation::VERTICAL,   'A', ObstructionType::FOX_2)); // แนวตั้ง บนซ้าย
    board->addPiece(new Obstruction(4, 0, 2, Orientation::HORIZONTAL, 'B', ObstructionType::FOX_2)); // แนวนอน บนขวา
    board->addPiece(new Obstruction(0, 3, 2, Orientation::VERTICAL,   'C', ObstructionType::FOX_2)); // แนวตั้ง ซ้ายกลาง
    board->addPiece(new Obstruction(1, 3, 2, Orientation::HORIZONTAL, 'D', ObstructionType::FOX_2)); // แนวนอน กลางซ้าย
    board->addPiece(new Obstruction(3, 3, 2, Orientation::HORIZONTAL, 'E', ObstructionType::FOX_2)); // แนวนอน กลางขวา
    board->addPiece(new Obstruction(1, 4, 2, Orientation::HORIZONTAL, 'F', ObstructionType::FOX_2)); // แนวนอน ล่างซ้าย
    board->addPiece(new Obstruction(3, 4, 2, Orientation::VERTICAL,   'G', ObstructionType::FOX_2)); // แนวตั้ง ล่างขวา 1
    board->addPiece(new Obstruction(4, 4, 2, Orientation::VERTICAL,   'H', ObstructionType::FOX_2)); // แนวตั้ง ล่างขวา 2

    // ==========================================
    // 🧱 3. แก๊งกำแพงหิน สีเทา (ความยาว 3)
    // ==========================================
    board->addPiece(new Obstruction(2, 0, 3, Orientation::VERTICAL,   'I', ObstructionType::WALL_3)); // แนวตั้ง กลางบน
    board->addPiece(new Obstruction(5, 3, 3, Orientation::VERTICAL,   'J', ObstructionType::WALL_3)); // แนวตั้ง ขวาสุด
    board->addPiece(new Obstruction(0, 5, 3, Orientation::HORIZONTAL, 'K', ObstructionType::WALL_3)); // แนวนอน ล่างสุด
    }
    void updateTime(float dt) {
        if (state == GameState::PLAYING) {
            timeRemaining -= dt;
            if (timeRemaining <= 0) { timeRemaining = 0; state = GameState::LOSE_TIME; }
        }
    }
    GameState getGameState() { return state; }
    vector<GamePiece*> getPieces() { return board ? board->getPieces() : vector<GamePiece*>(); }
    int getCurrentLevelId() { return currentLevelId; }
    int getCurrentMove() { return currentMove; }
    int getMoveLimit() { return moveLimit; }
    int getTimeRemaining() { return (int)timeRemaining; }
    int getExitX() { return board ? board->getExitX() : 0; }
    int getExitY() { return board ? board->getExitY() : 0; }

    void processMove(char id, int steps) {
        if (state != GameState::PLAYING) return;
        for (GamePiece* p : board->getPieces()) {
            if (p->getId() != id) continue;
            if (!canMove(p, steps)) return;
            p->move(steps);
            currentMove++;
            break;
        }
        if (currentMove >= moveLimit) state = GameState::LOSE_MOVE;
        for (GamePiece* p : board->getPieces()) {
            if (p->getId() == 'P') {
                Pig* pig = static_cast<Pig*>(p);
                if (pig->isRescued(board->getExitX(), board->getExitY())) state = GameState::WON;
            }
        }
    }

    void processMoveToEnd(char id) {
        if (state != GameState::PLAYING) return;
        for (GamePiece* p : board->getPieces()) {
            if (p->getId() != id) continue;
            // หาทิศ: ลองไปขวา/ลงก่อน ถ้าไม่ได้ให้ไปซ้าย/ขึ้น
            int dir = canMove(p, 1) ? 1 : -1;
            // ขยับจนสุด
            while (canMove(p, dir)) p->move(dir);
            break;
        }
        currentMove++;
        if (currentMove >= moveLimit) state = GameState::LOSE_MOVE;
        for (GamePiece* p : board->getPieces()) {
            if (p->getId() == 'P') {
                Pig* pig = static_cast<Pig*>(p);
                if (pig->isRescued(board->getExitX(), board->getExitY())) state = GameState::WON;
            }
        }
    }
};

extern GameManager game;
extern float GRID_OFFSET_X;
extern float GRID_OFFSET_Y;
extern float VISUAL_CELL_SIZE;
extern float VISUAL_CELL_SIZE_Y;

void InitGame();
void UpdateGameplay();