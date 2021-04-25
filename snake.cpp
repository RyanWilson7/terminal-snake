// Author : Ryan Wilson
// License : https://www.gnu.org/licenses/gpl-3.0.en.html
#include <cstring>
#include <iostream>
#include <list>
#include <ncurses.h>
#include <thread>
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define FOOD_PAIR 4
#define SNAKE_PAIR 5
#define BORDER_PAIR 6
#define HEAD_PAIR 7
struct XY {
  int x;
  int y;
};

void drawBoard(WINDOW *win, char *board, int width, int height, int score,
               bool dead) {
  char title[] = "Snake by Ryan Wilson\tScore : ";
  char cScore = '0' + score;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      wmove(win, y, x);
      char c = board[y * width + x];
      if (c == '%')
        waddch(win, c | COLOR_PAIR(FOOD_PAIR));
      else if (c == '+' || c == '0')
        waddch(win, c | COLOR_PAIR(SNAKE_PAIR));
      else if (c == '^' || c == '>' || c == 'v' || c == '<' || c == 'X')
        waddch(win, c | COLOR_PAIR(HEAD_PAIR));
      else
        waddch(win, c | COLOR_PAIR(BORDER_PAIR));
    }
  }
  wmove(win, 1, width / 2 - sizeof(title));
  waddstr(win, title);
  waddch(win, cScore | COLOR_PAIR(BORDER_PAIR));
  if (dead)
    waddstr(win, " You have died! Space to restart.");
  wrefresh(win);
}

int main() {
  int width = 120, height = 30;
  char *board = new char[width * height];
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  curs_set(0);
  start_color();
  init_pair(FOOD_PAIR, COLOR_RED, COLOR_BLACK);
  init_pair(SNAKE_PAIR, COLOR_GREEN, COLOR_BLACK);
  init_pair(BORDER_PAIR, COLOR_YELLOW, COLOR_BLACK);
  init_pair(HEAD_PAIR, COLOR_MAGENTA, COLOR_BLACK);
  WINDOW *win = newwin(height, width, 0, 0);

  while (true) {
    std::list<XY> snake = {{60, 15}, {61, 15}, {62, 15}, {63, 15}, {64, 15},
                           {65, 15}, {66, 15}, {67, 15}, {68, 15}, {69, 15}};
    int snakeDirection = WEST;
    char head = '<';
    XY food = {30, 15};
    int score = 0;
    bool dead = false;
    while (!dead) {
      auto t1 = std::chrono::system_clock::now();
      while ((std::chrono::system_clock::now() - t1) <
             std::chrono::milliseconds(60)) {
        int input = getch();
        switch (input) {
        case 'w':
          snakeDirection = NORTH;
          break;

        case 'd':
          snakeDirection = EAST;
          break;

        case 's':
          snakeDirection = SOUTH;
          break;

        case 'a':
          snakeDirection = WEST;
          break;
        }
      }

      switch (snakeDirection) {
      case NORTH:
        snake.push_front({snake.front().x, snake.front().y - 1});
        head = '^';
        break;
      case EAST:
        snake.push_front({snake.front().x + 1, snake.front().y});
        head = '>';
        break;
      case SOUTH:
        snake.push_front({snake.front().x, snake.front().y + 1});
        head = 'v';
        break;
      case WEST:
        snake.push_front({snake.front().x - 1, snake.front().y});
        head = '<';
        break;
      }

      if (snake.front().x == food.x && snake.front().y == food.y) {
        score++;
        while (board[food.y * width + food.x] != ' ') {
          food.x = rand() % width;
          food.y = (rand() % (height - 3)) + 3;
        }
        for (int i = 0; i < 5; i++)
          snake.push_back({snake.back().x, snake.back().y});
      }

      if (snake.front().x < 1 || snake.front().x >= width - 1 ||
          snake.front().y < 3 || snake.front().y >= height - 1)
        dead = true;

      for (std::list<XY>::iterator i = snake.begin(); i != snake.end(); i++)
        if (i != snake.begin() && i->x == snake.front().x &&
            i->y == snake.front().y)
          dead = true;

      snake.pop_back();
      memset(board, ' ', (width * height) * sizeof(char));

      for (int i = 0; i < width; i++) {
        board[i] = '=';
        board[2 * width + i] = '=';
        board[(height - 1) * width + i] = '=';
      }
      for (int i = 0; i < height; i++) {
        board[i * width] = '|';
        board[i * width + (width - 1)] = '|';
      }

      for (auto s : snake)
        board[s.y * width + s.x] = dead ? '+' : '0';

      board[snake.front().y * width + snake.front().x] = dead ? 'X' : head;
      board[food.y * width + food.x] = '%';
      drawBoard(win, board, width, height, score, dead);
    }
    while (int i = getch() != ' ') {
    }
  }

  delete[] board;
  return 0;
}
