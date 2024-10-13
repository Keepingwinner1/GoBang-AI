#include <iostream>
#include <vector>

const int BOARD_SIZE = 15;  // 五子棋标准棋盘大小
char board[BOARD_SIZE][BOARD_SIZE];  // 用于存储棋盘的二维数组

// 初始化棋盘，所有位置初始化为 '.'
void initBoard() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = '.';
        }
    }
}

// 打印棋盘
void printBoard() {
    std::cout << "  ";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << i % 10 << " ";  // 打印列号
    }
    std::cout << std::endl;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << i% 10 << " ";  // 打印行号
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if(board[i][j] == '1')
                std::cout << 'X' << " ";
            else
                std::cout << 'O' << " ";
        }
        std::cout << std::endl;
    }
}

// 在特定位置打印棋子
void printPiece(int x, int y, char player) {
    // 使用 ANSI 转义序列清除终端并重新打印棋盘
    std::cout << "\033[2J\033[1;1H";  // 清屏并将光标移动到左上角
    printBoard();  // 打印棋盘
    std::cout << "Player " << player << " placed at (" << x << ", " << y << ")\n";
}

// 判断某个位置是否可以下棋
bool isValidMove(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == '.';
}

// 下棋操作，player = '1'（黑） 或 '2'（白）
void makeMove(int x, int y, char player) {
    if (isValidMove(x, y)) {
        board[x][y] = player;
    }
    else {
        std::cout << "Invalid move! Please try again." << std::endl;
    }
}

// 检查是否有人获胜
bool checkWin(int x, int y, char player) {
    const int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };  // 水平、垂直、对角线方向

    for (const auto& dir : directions) {
        int count = 1;

        // 向一个方向检查
        for (int i = 1; i < 5; ++i) {
            int nx = x + dir[0] * i;
            int ny = y + dir[1] * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == player) {
                ++count;
            }
            else {
                break;
            }
        }

        // 向反方向检查
        for (int i = 1; i < 5; ++i) {
            int nx = x - dir[0] * i;
            int ny = y - dir[1] * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == player) {
                ++count;
            }
            else {
                break;
            }
        }

        // 判断是否连成5个
        if (count >= 5) {
            return true;
        }
    }

    return false;
}

int main() {
    initBoard();
    char currentPlayer = 'X';  // X 先手（黑子先手）
    int moveCount = 0;
    int x, y;

    printBoard();  // 打印初始棋盘

    while (true) {
        std::cout << "Player " << currentPlayer << ", enter your move (row and column): ";
        std::cin >> x >> y;

        if (isValidMove(x, y)) {
            char currentMove = '1';
            if (currentPlayer == 'O')
                currentMove = '2';

            makeMove(x, y, currentMove);
           
            printPiece(x, y, currentPlayer);  // 打印当前棋子位置

            // 检查当前玩家是否获胜
            if (checkWin(x, y, currentMove)) {
                std::cout << "Player " << currentPlayer << " wins!" << std::endl;
                break;
            }

            // 平局情况
            if (++moveCount == BOARD_SIZE * BOARD_SIZE) {
                std::cout << "It's a draw!" << std::endl;
                break;
            }

            // 切换玩家
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
        else {
            std::cout << "Invalid move. Try again." << std::endl;
        }
    }

    return 0;
}
