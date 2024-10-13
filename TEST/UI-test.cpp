#include <iostream>
#include <vector>

const int BOARD_SIZE = 15;  // �������׼���̴�С
char board[BOARD_SIZE][BOARD_SIZE];  // ���ڴ洢���̵Ķ�ά����

// ��ʼ�����̣�����λ�ó�ʼ��Ϊ '.'
void initBoard() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = '.';
        }
    }
}

// ��ӡ����
void printBoard() {
    std::cout << "  ";
    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << i % 10 << " ";  // ��ӡ�к�
    }
    std::cout << std::endl;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        std::cout << i% 10 << " ";  // ��ӡ�к�
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if(board[i][j] == '1')
                std::cout << 'X' << " ";
            else
                std::cout << 'O' << " ";
        }
        std::cout << std::endl;
    }
}

// ���ض�λ�ô�ӡ����
void printPiece(int x, int y, char player) {
    // ʹ�� ANSI ת����������ն˲����´�ӡ����
    std::cout << "\033[2J\033[1;1H";  // ������������ƶ������Ͻ�
    printBoard();  // ��ӡ����
    std::cout << "Player " << player << " placed at (" << x << ", " << y << ")\n";
}

// �ж�ĳ��λ���Ƿ��������
bool isValidMove(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == '.';
}

// ���������player = '1'���ڣ� �� '2'���ף�
void makeMove(int x, int y, char player) {
    if (isValidMove(x, y)) {
        board[x][y] = player;
    }
    else {
        std::cout << "Invalid move! Please try again." << std::endl;
    }
}

// ����Ƿ����˻�ʤ
bool checkWin(int x, int y, char player) {
    const int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };  // ˮƽ����ֱ���Խ��߷���

    for (const auto& dir : directions) {
        int count = 1;

        // ��һ��������
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

        // �򷴷�����
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

        // �ж��Ƿ�����5��
        if (count >= 5) {
            return true;
        }
    }

    return false;
}

int main() {
    initBoard();
    char currentPlayer = 'X';  // X ���֣��������֣�
    int moveCount = 0;
    int x, y;

    printBoard();  // ��ӡ��ʼ����

    while (true) {
        std::cout << "Player " << currentPlayer << ", enter your move (row and column): ";
        std::cin >> x >> y;

        if (isValidMove(x, y)) {
            char currentMove = '1';
            if (currentPlayer == 'O')
                currentMove = '2';

            makeMove(x, y, currentMove);
           
            printPiece(x, y, currentPlayer);  // ��ӡ��ǰ����λ��

            // ��鵱ǰ����Ƿ��ʤ
            if (checkWin(x, y, currentMove)) {
                std::cout << "Player " << currentPlayer << " wins!" << std::endl;
                break;
            }

            // ƽ�����
            if (++moveCount == BOARD_SIZE * BOARD_SIZE) {
                std::cout << "It's a draw!" << std::endl;
                break;
            }

            // �л����
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
        else {
            std::cout << "Invalid move. Try again." << std::endl;
        }
    }

    return 0;
}
