#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <vector>
#include<map>
#include<queue>
#include <string>
#include <random>
#include <iostream>
using namespace std;

typedef int BOOL;
#define TRUE 1
#define FALSE 0
//平台发送的指令
#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"

//其他宏定义
#define BOARD_SIZE 12 //棋盘大小
#define PATTERN_SIZE 13 //棋型图案大小
#define EMPTY 0 //空白位置（可落子位置）为0
#define BLACK 1 //黑子为1
#define WHITE 2  //白子为2

//全局变量
char board[BOARD_SIZE][BOARD_SIZE] = { '0' };  //创建储存棋盘，char 
int myFlag;  //我方棋子 1为黑棋 2为白棋
int enemyFlag;  //敌方棋子
int DEPTH = 3;//最大搜索层数
mt19937_64 rng(std::random_device{}());// 用于生成64位整数的随机引擎
uint64_t zobristTable[BOARD_SIZE][BOARD_SIZE][3];//Zobrist哈希常数，使用随机的64位整数填充表，表示每个可能的状态
uint64_t zobristKey = 0;

// 用于存储已评估位置的置换表
struct TranspositionEntry {
    uint64_t key;
    int depth;
    int score;
};

map<uint64_t, TranspositionEntry> transpositionTable;//置换表

//自定义结构体
struct Command//棋子坐标
{
    int x;
    int y;
    Command(int i, int j) :x(i), y(j) {}
};

struct patterns {//五子棋棋型
    string pattern;
    int score;
};

struct position { //位置（含得分）
    Command cmd;
    int score;
    position(Command c = { 0,0 }, int s = 0) :cmd(c), score(s) {};
};

Command bestMove = Command(0, 0); // 定义存储最佳落子坐标的结构体变量

//放置敌方棋子
void place(struct Command cmd) {
    board[cmd.x][cmd.y] = enemyFlag;
}
//初始化棋子状态
void initAI(int me)
{
    enemyFlag = 3 - me;
}
//start
void start(int flag)
{
    memset(board, 0, sizeof(board));
    int middlePlace = BOARD_SIZE / 2;
    //中央位置开局已经写死
    board[middlePlace - 1][middlePlace - 1] = WHITE;
    board[middlePlace][middlePlace] = WHITE;
    board[middlePlace - 1][middlePlace] = BLACK;
    board[middlePlace][middlePlace - 1] = BLACK;

    initAI(flag);//初始化敌方棋子状态
}

Command get_position();
//落子
void turn()
{
    //我方棋子放置的位置的判断条件
    Command cmd = get_position();
    printf("%d %d", cmd.x, cmd.y);
    printf("\n");
    fflush(stdout);
    board[cmd.x][cmd.y] = myFlag;

    return;
}

void end(int x)
{
    exit(0);
}

void loop()
{
    char tag[10] = { 0 };
    struct Command command =
    {
        command.x = 0,
        command.y = 0,
    };
    int status;
    while (TRUE)
    {
        memset(tag, 0, sizeof(tag));
        scanf("%s", tag);
        if (strcmp(tag, START) == 0)
        {
            scanf("%d", &myFlag);
            start(myFlag);
            printf("OK\n");
            fflush(stdout);
        }
        else if (strcmp(tag, PLACE) == 0)
        {
            scanf("%d %d", &command.x, &command.y);
            place(command);
        }
        //我方出子
        else if (strcmp(tag, TURN) == 0)
        {
            turn();
        }
        else if (strcmp(tag, END) == 0)
        {
            scanf("%d", &status);
            end(status);
        }
    }
}


//用于填充Zobrist表的函数
void initializeZobristTable() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int player = 0; player < 3; player++) {
                zobristTable[i][j][player] = rng();
            }
        }
    }
}

// 用于更新给定移动的Zobrist密钥的函数
void updateZobristKey(int x, int y, int player) {
    zobristKey ^= zobristTable[x][y][player];
}

// 用于初始化当前棋盘状态的Zobrist密钥的函数
void initZobristKey() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != EMPTY) {
                updateZobristKey(i, j, board[i][j]);
            }
        }
    }
}



int main(int argc, char* argv[])
{
    // 初始化Zobrist表
    initializeZobristTable();
    // 初始化Zobrist密钥
    initZobristKey();
    loop();
    return 0;
}

//敌我双方棋型
vector<patterns> Pattern = {
    {"11111", 1000000},

    {"011110", 10000},

    {"211110", 1000},
    {"011112", 1000},
    {"10111", 1000},
    {"11011", 1000},
    {"11101", 1000},

    {"01110", 1000},
    {"011010", 1000},
    {"010110", 1000},

    {"211100", 100},
    {"001112", 100},
    {"211010", 100},
    {"010112", 100},
    {"210011", 100},
    {"110012", 100},
    {"11100", 100},
    {"00111", 100},
    {"11010", 100},
    {"01011", 100},
    {"10011", 100},
    {"11001", 100},

    {"001100", 10},
    {"001010", 10},
    {"010100", 10},
    {"211000", 1},
    {"001012", 1},
    {"210100", 1},
    {"000112", 1},
};
vector<patterns> _Pattern = {
    {"22222", 1000000},

    {"022220", 10000},

    {"122220", 1000},
    {"022221", 1000},

    {"20222", 1000},
    {"22022", 1000},
    {"22202", 1000},

    {"02220", 1000},
    {"022020", 1000},
    {"020220", 1000},

    {"122200", 100},
    {"002221", 100},
    {"122020", 100},
    {"020221", 100},
    {"120022", 100},
    {"220021", 100},

    {"22200", 100},
    {"00222", 100},
    {"22020", 100},
    {"02022", 100},
    {"20022", 100},
    {"22002", 100},

    {"002200", 10},
    {"002020", 10},
    {"020200", 10},
    {"122000", 1},
    {"002021", 1},
    {"120200", 1},
    {"000221", 1},
};

//AC模式匹配算法模块
// AC自动机节点
struct ACNode {
    map<char, ACNode*> children;
    ACNode* fail;
    int score;

    ACNode() : fail(nullptr), score(0) {}
};
// AC自动机
class ACAutomaton {
public:
    ACAutomaton(const vector<patterns>& patterns) {
        root = new ACNode();

        // 构建Trie树
        for (const auto& pattern : patterns) {
            ACNode* node = root;
            for (char c : pattern.pattern) {
                if (!node->children[c]) {
                    node->children[c] = new ACNode();
                }
                node = node->children[c];
            }
            node->score = pattern.score;
        }
        // 构建失败指针
        buildFailurePointer();
    }
    // 在输入字符串中匹配模式并计算得分
    int matchAndScore(const char* str) {
        int totalScore = 0;
        ACNode* currentState = root;

        for (int i = 0; str[i] != '\0'; ++i) {
            char c = str[i];

            while (currentState != root && !currentState->children[c]) {
                currentState = currentState->fail;
            }

            if (currentState->children[c]) {
                currentState = currentState->children[c];
            }

            totalScore += currentState->score;
        }

        return totalScore;
    }

private:
    // 构建失败指针
    void buildFailurePointer() {
        queue<ACNode*> q;
        for (auto& kv : root->children) {
            ACNode* child = kv.second;
            child->fail = root;
            q.push(child);
        }

        while (!q.empty()) {
            ACNode* current = q.front();
            q.pop();

            for (auto& kv : current->children) {
                char symbol = kv.first;
                ACNode* child = kv.second;
                q.push(child);

                ACNode* failNode = current->fail;
                while (failNode != root && !failNode->children[symbol]) {
                    failNode = failNode->fail;
                }

                if (failNode->children[symbol]) {
                    child->fail = failNode->children[symbol];
                }
                else {
                    child->fail = root;
                }
            }
        }
    }

    ACNode* root;
};

//局势评分模块
char line_x[BOARD_SIZE][PATTERN_SIZE] = { '\n' };
char line_y[BOARD_SIZE][PATTERN_SIZE] = { '\n' };
char line_r[15][PATTERN_SIZE] = { '\n' };
char line_l[15][PATTERN_SIZE] = { '\n' };

ACAutomaton acAutomaton(Pattern);
ACAutomaton _acAutomaton(_Pattern);

//局势评估函数 对于每下一步棋后的某一方得分进行评估
//将棋盘的每一行与每一列，每一个斜线进行扫描匹配，计算得分
int evaluateBoard(int role, int depth) {

    uint64_t boardHash = zobristKey;
    if (transpositionTable.find(boardHash) != transpositionTable.end() && transpositionTable[boardHash].depth >= depth) {
        return transpositionTable[boardHash].score;
    }

    int result = 0;

    // Rows and columns
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            line_x[i][j] = ((board[i][j] == role) ? '1' : (board[i][j] == 3 - role) ? '2' : '0');
            line_y[i][j] = ((board[j][i] == role) ? '1' : (board[j][i] == 3 - role) ? '2' : '0');
        }
    }

    // Diagonals
    for (int k = 0; k < 8; k++) {
        for (int i = 0; i < BOARD_SIZE - k; i++) {

            line_r[k][i] = ((board[i][i + k] == role) ? '1' : (board[i][i + k] == 3 - role) ? '2' : '0');
            line_l[k][i] = ((board[i][11 - i - k] == role) ? '1' : (board[i][11 - i - k] == 3 - role) ? '2' : '0');
        }
    }
    for (int k = 1; k < 8; k++) {
        for (int i = 0; i < BOARD_SIZE - k; i++) {
            line_r[k + 7][i] = ((board[i + k][i] == role) ? '1' : (board[i + k][i] == 3 - role) ? '2' : '0');
            line_l[k + 7][i] = ((board[i + k][11 - i] == role) ? '1' : (board[i + k][11 - i] == 3 - role) ? '2' : '0');
        }
    }

    //匹配计算得分
    for (int i = 0; i < BOARD_SIZE; i++) {
        result += acAutomaton.matchAndScore(line_x[i]);
        result -= _acAutomaton.matchAndScore(line_x[i]);
        result += acAutomaton.matchAndScore(line_y[i]);
        result -= _acAutomaton.matchAndScore(line_y[i]);
    }
    for (int j = 0; j < 15; j++) {
        result += acAutomaton.matchAndScore(line_l[j]);
        result -= _acAutomaton.matchAndScore(line_l[j]);
        result += acAutomaton.matchAndScore(line_r[j]);
        result -= _acAutomaton.matchAndScore(line_r[j]);
    }

    // 存储到置换表
    TranspositionEntry entry = { boardHash, depth, result };
    transpositionTable[boardHash] = entry;

    return result;
}

//启发式评估函数：对于单个点进行评估 用于筛选进行搜索的点  
int evaluatePoint(Command cmd) {

    //对某一位置的横竖斜共四个方向进行遍历，匹配算分
    char pattern_x[10] = { '\n' };
    char pattern_y[10] = { '\n' };
    char pattern_l[10] = { '\n' };
    char pattern_r[10] = { '\n' };

    //修改启发式式评估函数开始的坐标
    //水平方向
    for (int i = max(0, cmd.x - 4), j = 0; i <= min(BOARD_SIZE - 1, cmd.x + 4); i++, j++) {
        if (board[cmd.x][i] == myFlag || i == cmd.y)
            pattern_x[j] = '1';
        else if (board[cmd.x][i] == enemyFlag)
            pattern_x[j] = '2';
        else
            pattern_x[j] = '0';
    }

    //竖直方向
    for (int i = max(0, cmd.y - 4), j = 0; i <= min(BOARD_SIZE - 1, cmd.y + 4); i++, j++) {
        if (board[i][cmd.y] == myFlag || i == cmd.x)
            pattern_y[j] = '1';
        else if (board[i][cmd.y] == enemyFlag)
            pattern_y[j] = '2';
        else
            pattern_y[j] = '0';
    }

    //左斜方向 左下角到右上角
    for (int i = min(BOARD_SIZE - 1, cmd.x + 4), j = max(0, cmd.y - 4), k = 0;
        i >= max(0, cmd.x - 4) && j <= min(BOARD_SIZE - 1, cmd.y + 4); i--, j++, k++) {
        if (board[i][j] == myFlag || (i == cmd.x && j == cmd.y))
            pattern_l[k] = '1';
        else if (board[i][j] == enemyFlag)
            pattern_l[k] = '2';
        else
            pattern_l[k] = '0';
    }
    //右斜方向  左上角到右下角
    int i = max(cmd.x - 4, 0), j = max(cmd.y - 4, 0), k = 0;
    for (; i <= min(BOARD_SIZE - 1, cmd.x + 4) && j <= min(BOARD_SIZE - 1, cmd.y + 4); i++, j++, k++) {
        if (board[i][j] == myFlag || (i == cmd.x && j == cmd.y))
            pattern_r[k] = '1';
        else if (board[i][j] == enemyFlag)
            pattern_r[k] = '2';
        else
            pattern_r[k] = '0';
    }

    //匹配计算得分
    int total_score = 0;
    total_score = acAutomaton.matchAndScore(pattern_x) + acAutomaton.matchAndScore(pattern_y) + acAutomaton.matchAndScore(pattern_l) + acAutomaton.matchAndScore(pattern_r);
    total_score = total_score - _acAutomaton.matchAndScore(pattern_x) - _acAutomaton.matchAndScore(pattern_y) - _acAutomaton.matchAndScore(pattern_l) - _acAutomaton.matchAndScore(pattern_r);

    return total_score;
}


//比较函数
bool compareCommands1(const position& pos1, const position& pos2) {
    return pos1.score > pos2.score;
}
bool compareCommands2(const position& pos1, const position& pos2) {
    return pos1.score < pos2.score;
}

//判断周围是否存在棋子
bool nearby(int x, int y) {
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0)
                continue;
            if (x + i >= 0 && x + i < BOARD_SIZE &&
                y + j >= 0 && y + j < BOARD_SIZE &&
                board[x + i][y + j] != EMPTY)
                return 1;
        }
    return 0;
}

vector<position> getPossiblePosition(int currentPlayer) {
    vector<position> PossiblePosition;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (board[i][j] == EMPTY && nearby(i, j))
                PossiblePosition.push_back(position(Command(i, j), 0));

    //启发排序
    if (currentPlayer == myFlag)
        sort(PossiblePosition.begin(), PossiblePosition.end(), compareCommands1);//降序排列
    else
        sort(PossiblePosition.begin(), PossiblePosition.end(), compareCommands2);//升序排列

    //取点
    PossiblePosition.resize(min(static_cast<size_t>(25), PossiblePosition.size()));
    return PossiblePosition;
}

//判断是否有一方连成五子
bool checkWin() {
    // 检查水平方向
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j <= BOARD_SIZE - 5; ++j) {
            if (board[i][j] != EMPTY) {
                bool win = true;
                for (int k = 1; k < 5; ++k) {
                    if (board[i][j + k] != board[i][j]) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }
    }

    // 检查垂直方向
    for (int i = 0; i <= BOARD_SIZE - 5; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] != EMPTY) {
                bool win = true;
                for (int k = 1; k < 5; ++k) {
                    if (board[i + k][j] != board[i][j]) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }
    }
    // 检查左斜方向
    for (int i = 0; i <= BOARD_SIZE - 5; ++i) {
        for (int j = 0; j <= BOARD_SIZE - 5; ++j) {
            if (board[i][j] != EMPTY) {
                bool win = true;
                for (int k = 1; k < 5; ++k) {
                    if (board[i + k][j + k] != board[i][j]) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }
    }
    // 检查右斜方向
    for (int i = 0; i <= BOARD_SIZE - 5; ++i) {
        for (int j = 4; j < BOARD_SIZE; ++j) {
            if (board[i][j] != EMPTY) {
                bool win = true;
                for (int k = 1; k < 5; ++k) {
                    if (board[i + k][j - k] != board[i][j]) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }
    }
    return false; // 没有获胜
}

//主算法
//含有alpha-beta剪枝算法的minmax搜索
int minmax(int depth, int alpha, int beta, int currentPlayer) {

    if (depth == DEPTH || checkWin()) {//递归结束
        return evaluateBoard(myFlag, depth);
    }

    int bestScore = (currentPlayer == myFlag) ? -10000000 : 10000000;
    bool shouldBreak = false;

    vector<position> Possition = getPossiblePosition(currentPlayer);

    for (auto i = Possition.begin(); i != Possition.end() && !shouldBreak; i++) {
        board[i->cmd.x][i->cmd.y] = currentPlayer;
        updateZobristKey(i->cmd.x, i->cmd.y, currentPlayer);
        int score = minmax(depth + 1, alpha, beta, 3 - currentPlayer);
        board[i->cmd.x][i->cmd.y] = EMPTY;
        updateZobristKey(i->cmd.x, i->cmd.y, EMPTY);
        if ((currentPlayer == myFlag && score > bestScore) || (currentPlayer == enemyFlag && score < bestScore)) {
            bestScore = score;
            if (depth == 0) {
                bestMove.x = i->cmd.x;
                bestMove.y = i->cmd.y;
            }
        }
        if (currentPlayer == myFlag) {
            alpha = (alpha > bestScore) ? alpha : bestScore;
        }
        else {
            beta = (beta < bestScore) ? beta : bestScore;
        }
        shouldBreak = (beta <= alpha);
    }

    return bestScore;
}

//防守评估函数
int _evaluatePoint(Command cmd) {
    //我方落子此处情况
    char pattern_x[10] = { '\n' };
    char pattern_y[10] = { '\n' };
    char pattern_l[10] = { '\n' };
    char pattern_r[10] = { '\n' };
    //落子此处对方情况
    char _pattern_x[10] = { '\n' };
    char _pattern_y[10] = { '\n' };
    char _pattern_l[10] = { '\n' };
    char _pattern_r[10] = { '\n' };

    //对某一位置的横竖斜共四个方向进行遍历，匹配算分
    //水平方向
    for (int i = max(0, cmd.x - 4), j = 0; i <= min(BOARD_SIZE - 1, cmd.x + 4); i++, j++) {
        if (board[cmd.x][i] == myFlag) {
            pattern_x[j] = '1';
            _pattern_x[j] = '1';
        }
        else if (i == cmd.y) {
            pattern_x[j] = '1';
            _pattern_x[j] = '2';
        }
        else if (board[cmd.x][i] == enemyFlag) {
            pattern_x[j] = '2';
            _pattern_x[j] = '2';
        }
        else {
            pattern_x[j] = '0';
            _pattern_x[j] = '0';
        }

    }
    //竖直方向
    for (int i = max(0, cmd.y - 4), j = 0; i <= min(BOARD_SIZE - 1, cmd.y + 4); i++, j++) {
        if (board[i][cmd.y] == myFlag)
        {
            pattern_y[j] = '1';
            _pattern_y[j] = '1';
        }
        else if (i == cmd.x) {
            pattern_y[j] = '1';
            _pattern_y[j] = '2';
        }
        else if (board[i][cmd.x] == enemyFlag)
        {
            pattern_y[j] = '2';
            _pattern_y[j] = '2';
        }
        else
        {
            pattern_y[j] = '0';
            _pattern_y[j] = '0';
        }
    }


    //左斜方向  左下到右上
    for (int i = min(BOARD_SIZE - 1, cmd.x + 4), j = max(0, cmd.y - 4), k = 0;
        i >= max(0, cmd.x - 4) && j <= min(BOARD_SIZE - 1, cmd.y + 4); i--, j++, k++) {
        if (board[i][j] == myFlag)
        {
            pattern_l[k] = '1';
            _pattern_l[k] = '1';
        }
        else if (i == cmd.x && j == cmd.y) {
            pattern_l[k] = '1';
            _pattern_l[k] = '2';
        }
        else if (board[i][j] == enemyFlag)
        {
            pattern_l[k] = '2';
            _pattern_l[k] = '2';
        }
        else
        {
            pattern_l[k] = '0';
            _pattern_l[k] = '0';
        }
    }
    //右斜方向
    for (int i = max(cmd.x - 4, 0), j = max(cmd.y - 4, 0), k = 0;
        i <= min(BOARD_SIZE - 1, cmd.x + 4) && j <= min(BOARD_SIZE - 1, cmd.y + 4); i++, j++, k++) {
        if (board[i][j] == myFlag)
        {
            pattern_r[k] = '1';
            _pattern_r[k] = '1';
        }
        else if (i == cmd.x && j == cmd.y) {
            pattern_r[k] = '1';
            _pattern_r[k] = '2';
        }
        else if (board[i][j] == enemyFlag)
        {
            pattern_r[k] = '2';
            _pattern_r[k] = '2';
        }
        else
        {
            pattern_r[k] = '0';
            _pattern_r[k] = '0';
        }
    }

    //对我方进行匹配
    int score1 = acAutomaton.matchAndScore(pattern_x);
    int score2 = acAutomaton.matchAndScore(pattern_y);
    int score3 = acAutomaton.matchAndScore(pattern_l);
    int score4 = acAutomaton.matchAndScore(pattern_r);
    //cout << "我方落子"<<cmd.x<<' '<<cmd.y <<"处得分："<<score1 << ' ' << score2 << ' ' << score3 << ' ' << score4 << endl;
    // 对敌方进行匹配
    int score5 = _acAutomaton.matchAndScore(_pattern_x);
    int score6 = _acAutomaton.matchAndScore(_pattern_y);
    int score7 = _acAutomaton.matchAndScore(_pattern_l);
    int score8 = _acAutomaton.matchAndScore(_pattern_r);

    //cout << "对方落子此处得分：" << score1 << ' ' << score2 << ' ' << score3 << ' ' << score4 << endl;
    //返回顺序：我方成五>敌方成五>我方活四>敌方活四
    if (score1 >= 1000000 || score2 >= 1000000 || score3 >= 1000000 || score4 >= 1000000)
        return 1;//我方成五
    else if (score5 >= 1000000 || score6 >= 1000000 || score7 >= 1000000 || score8 >= 1000000) {
        return 3;//敌方成五
    }
    else if (score1 >= 10000 || score2 >= 10000 || score3 >= 10000 || score4 >= 10000)
        return 2;//我方活四
    else if (score5 >= 10000 || score6 >= 10000 || score7 >= 10000 || score8 >= 10000)
        return 4;//敌方活四
    //cout << "无上述情况发生" << endl;
    return 0;//不存在上述情况
}

//防守判断函数
Command defendIfNecessary() {
    vector<position> PossiblePosition;
    // 找到周围的点
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == EMPTY && nearby(i, j)) {
                PossiblePosition.push_back(position(Command(i, j), 0));
            }
        }
    }
    //存在上述情况发生则返回该位置
    for (const auto& pos : PossiblePosition) {
        int returnvalue = _evaluatePoint(pos.cmd);
        //cout << "返回值为：" << returnvalue << endl;
        switch (returnvalue) {
            case 1:
            case 2:
            case 3:
            case 4:
                return pos.cmd;
            default:
                break;
        }
    }
    return Command(-1, -1);//不存在
}

//获取下一步位置坐标
Command get_position() {
    //minmax函数初始参数
    int depth = 0; // 初始搜索层数为0  
    int alpha = -10000000; // alpha 代表已知的max层（己方层）最大值
    int beta = 10000000; // beta代表已知的min层（对手层）最小值

    // 检查是否需要防守敌方（含己方有利态势立刻进攻）
    Command defenseMove = defendIfNecessary();
    if (defenseMove.x != -1 && defenseMove.y != -1) {
        return defenseMove;
    }
    // 调用minmax函数并传入参数
    minmax(depth, alpha, beta, myFlag);

    return bestMove;
}