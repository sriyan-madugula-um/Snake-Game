#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>

using namespace std;

bool gameover;
const int width = 5; // Gameboard dimensions
const int height = 2;
int x, y, fruitX, fruitY, score; // Coordinates of head and fruit
int score_increment = 10; // How much score increases every time fruit is hit
int tailX[100], tailY[100]; // Stores coordinates of tail locations
int nTail; // Length of tail
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

// Function to set the terminal in non-canonical mode
void setNonCanonicalMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Function to reset the terminal to canonical mode
void resetTerminalMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Function to get a single character from the terminal without waiting for Enter
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

void Setup() {
    gameover = false;
    dir = STOP; // Start with the snake moving to the right
    x = width / 2;
    y = height / 2;
    do {
        fruitX = rand() % width;
        fruitY = rand() % height;
    }
    while(x == fruitX || y == fruitY); // Initial fruit not placed at snake head
    score = 0;
}

void Draw() {
    system("clear"); // Clear the console (optional)

    for (int i = 0; i < width + 2; i++) 
        cout << "#"; // Top wall
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#"; // Left wall
            if (i == y && j == x)
                cout << "O"; // Snake head
            else if (i == fruitY && j == fruitX)
                cout << "F"; // Fruit
            else {
                bool printTail = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o"; // Snake tail
                        printTail = true;
                    }
                }
                if (!printTail)
                    cout << " ";
            }
            if (j == width - 1)
                cout << "#"; // Right wall
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#"; // Bottom wall
    cout << endl;

    cout << "Score:" << score << endl;
}

void Input() {
    char key = getch();
    switch (key) {
    case 'a': // Left key
        if (dir != RIGHT)
            dir = LEFT;
        break;
    case 'd': // Right key
        if (dir != LEFT)
            dir = RIGHT;
        break;
    case 'w': // Up key
        if (dir != DOWN)
            dir = UP;
        break;
    case 's': // Down key
        if (dir != UP)
            dir = DOWN;
        break;
    case 'x': // Quit key
        gameover = true;
        break;
    }
}

// Helper function for generating random fruit locations outside of the tail
bool overlapsTail(int fX, int fY) {
    for(int i = 0; i < nTail; ++i) {
        if (tailX[i] == fX && tailY[i] == fY) {
            // The fruit position overlaps with a segment of the snake's tail
            return true;
        }
    }
    return false;
}

bool is_gameBoard_full() {
    return nTail == width*height - 1;
}

void Algorithm() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;

    // Update the position of the head based on the current direction
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    // Check for collisions with walls or the snake's own tail
    if (x < 0 || x >= width || y < 0 || y >= height)
        gameover = true;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameover = true;

    // Check if the snake has eaten the fruit
    if (x == fruitX && y == fruitY) {
        score += score_increment;
        do {
            if(is_gameBoard_full()) {
                Draw();
                gameover = true;
                break;
            }
            fruitX = rand() % width;
            fruitY = rand() % height;
        }
        while(overlapsTail(fruitX, fruitY) || (x == fruitX && y == fruitY));
        nTail++;
    }

    // Update the position of the tail
    tailX[0] = x;
    tailY[0] = y;

    // Update the positions of the rest of the tail
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
}


int main() {
    srand(static_cast<unsigned>(time(0))); // Seed for randomization
    setNonCanonicalMode(); // Set terminal to non-canonical mode

    Setup();

    while (!gameover) {
        Draw();
        Input();
        Algorithm();
    }

    cout << "Game Over!" << endl;

    resetTerminalMode(); // Reset terminal to canonical mode
    return 0;
}
