#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>

#define LEFT_WALL 1
#define TOP_WALL 1
#define BOTTOM_WALL LINES - 2
#define HORIZONTAL_CHAR  '='
#define VERTICAL_CHAR  '|'
#define BALL_CHAR 'o'
#define PADDLE_SIZE LINES / 5

int paddle_pos;
int paddle_pos_2;
int ball_x_dir = -1;
int ball_y_dir = 1;
int ball_x_pos;
int ball_y_pos;
int score = 0;
int game_over = 0;
int two_player_mode = 0;
int player_two_score = 0;

void init_game_board(){
  int i;
  int j;
  for(i=0; i<LINES; ++i){
    for(j=0; j< COLS; ++j){
      move(i, j);
      if((i == TOP_WALL || i == BOTTOM_WALL) && j > 0 && j < COLS - 2)
        addch(HORIZONTAL_CHAR);
      else if(j == LEFT_WALL && i > 0 && i < BOTTOM_WALL && two_player_mode == 0)
        addch(VERTICAL_CHAR);
    }
  }
  refresh();
}

void draw_paddle(){
  int i;
  for(i=paddle_pos; i < paddle_pos + PADDLE_SIZE; ++i){
    move(i, COLS-1);
    addch(VERTICAL_CHAR);
  }

  if(two_player_mode == 1){
    for(i=paddle_pos_2; i < paddle_pos_2 + PADDLE_SIZE; ++i){
      move(i, LEFT_WALL);
      addch(VERTICAL_CHAR);
    }
  }
  refresh();
}

void spawn_ball(){
  srand(time(NULL));
  ball_x_pos = rand() % (COLS - 5);
  ball_x_pos += 3;
  ball_y_pos = rand() % (LINES - 5);
  ball_y_pos += 3;

  if(rand() % 2 == 1)
    ball_y_dir = -1;
  else
    ball_y_dir = 1;
  ball_x_dir = -1;

  move(ball_y_pos, ball_x_pos);
  addch(BALL_CHAR);
  refresh();
}

void write_score(){
  if(two_player_mode == 0){
    move(0, COLS / 2 - 5);
    addstr("SCORE: ");
    char score_str[15];
    sprintf(score_str, "%d", score);
    addstr(score_str);
  }
  else{
    move(0, COLS / 2 - 15);
    addstr("LEFT SCORE: ");
    char score_str[15];
    sprintf(score_str, "%d  ", player_two_score);
    addstr(score_str);
    addstr("RIGHT SCORE: ");
    sprintf(score_str, "%d", score);
    addstr(score_str);
  }
  move(LINES, COLS);
}

void set_game_over_text(){
  int i;
  while(i<100){
    move(LINES / 2, COLS / 2 - 5);
    if(two_player_mode == 0)
      addstr("GAME OVER");
    else if(score > player_two_score)
      addstr("RIGHT WINS");
    else if(score == player_two_score)
      addstr("ITS A TIE");
    else
      addstr("LEFT WINS");
    if(two_player_mode == 0){
      move(LINES / 2 + 1, COLS / 2 - 5);
      addstr("SCORE: ");
      char score_str[15];
      sprintf(score_str, "%d", score);
      addstr(score_str);
    }
    else{
      move(LINES / 2 + 1, COLS / 2 - 15);
      addstr("LEFT SCORE: ");
      char score_str[15];
      sprintf(score_str, "%d  ", player_two_score);
      addstr(score_str);
      addstr("RIGHT SCORE: ");
      sprintf(score_str, "%d", score);
      addstr(score_str);
    }
    move(LINES / 2 + 2, COLS / 2 - 10);
    addstr("PLAY AGAIN? (Y/N): ");
    refresh();
    echo();
    switch(getch()){
    case 'y':
      game_over = 0;
      return;
    case 'n':
      game_over = 1;
      return;
    }
    usleep(60000);
    ++i;
  }
}

void move_ball(){
  if(game_over == 0){
    move(ball_y_pos, ball_x_pos);
    addch(' ');
    if(ball_x_pos == LEFT_WALL+1 && two_player_mode == 0)
      ball_x_dir = 1;
    else if(ball_x_pos == LEFT_WALL+1 && two_player_mode == 1 && ball_y_pos >= paddle_pos_2 && ball_y_pos <= paddle_pos_2 + PADDLE_SIZE){
      ball_x_dir = 1;
      ++player_two_score;
    }
    else if(ball_x_pos == LEFT_WALL+1){
      ball_x_dir = 0;
      ball_y_dir = 0;
      game_over = 1;
      return;
    }
    if(ball_y_pos == TOP_WALL+1 || ball_y_pos == BOTTOM_WALL-1)
      ball_y_dir = ball_y_dir * -1;
    if(ball_x_pos == COLS-1 && ball_y_pos >= paddle_pos && ball_y_pos <= paddle_pos + PADDLE_SIZE){
      ball_x_dir = -1;
      ++score;
    }
    else if(ball_x_pos == COLS-1){
      ball_x_dir = 0;
      ball_y_dir = 0;
      game_over = 1;
      return;
    }
    move(ball_y_pos += ball_y_dir, ball_x_pos += ball_x_dir);
    addch(BALL_CHAR);
  }
}

void move_paddle(){
  switch(getch()){
  case KEY_UP:
    if(paddle_pos > 1){
      move(paddle_pos + PADDLE_SIZE-1, COLS-1);
      addch(' ');
      --paddle_pos;
      move(paddle_pos + PADDLE_SIZE-1, COLS-1);
      addch(' ');
      --paddle_pos;
    }
    break;
  case KEY_DOWN:
    if(paddle_pos + PADDLE_SIZE < LINES-1){
      move(paddle_pos, COLS-1);
      addch(' ');
      ++paddle_pos;
      move(paddle_pos, COLS-1);
      addch(' ');
      ++paddle_pos;
    }
    break;
    if(two_player_mode == 1){
      case 'w':
        if(paddle_pos_2 > 1){
          move(paddle_pos_2 + PADDLE_SIZE-1, LEFT_WALL);
          addch(' ');
          --paddle_pos_2;
          move(paddle_pos_2 + PADDLE_SIZE-1, LEFT_WALL);
          addch(' ');
          --paddle_pos_2;
        }
        break;
      case 's':
        if(paddle_pos_2 + PADDLE_SIZE < LINES-1){
          move(paddle_pos_2, LEFT_WALL);
          addch(' ');
          ++paddle_pos_2;
          move(paddle_pos_2, LEFT_WALL);
          addch(' ');
          ++paddle_pos_2;
        }
        break;
    }
  }
}

void move_paddle_2(){
  switch(getch()){
  case 'w':
    if(paddle_pos_2 > 1){
      move(paddle_pos_2 + PADDLE_SIZE-1, LEFT_WALL);
      addch(' ');
      --paddle_pos_2;
      move(paddle_pos_2 + PADDLE_SIZE-1, LEFT_WALL);
      addch(' ');
      --paddle_pos_2;
    }
    break;
  case 's':
    if(paddle_pos_2 + PADDLE_SIZE < LINES-1){
      move(paddle_pos_2, LEFT_WALL);
      addch(' ');
      ++paddle_pos_2;
      move(paddle_pos_2, LEFT_WALL);
      addch(' ');
      ++paddle_pos_2;
    }
    break;
  }
}

void count_in(){
  move(LINES / 2, COLS / 2);
  addch('3');
  refresh();
  sleep(1);
  move(LINES / 2, COLS / 2);
  addch('2');
  refresh();
  sleep(1);
  move(LINES / 2, COLS / 2);
  addch('1');
  refresh();
  sleep(1);
  move(LINES / 2, COLS / 2);
  addch(' ');
  refresh();
}

void select_num_players(){
  while(1){
    if(two_player_mode == 0){
      move(LINES / 2, COLS / 2 - 4);
      standout();
      addstr("1 player");
      standend();
      move(LINES / 2 + 1, COLS / 2 - 4);
      addstr("2 player");
    }
    else{
      move(LINES / 2, COLS / 2 - 4);
      addstr("1 player");
      move(LINES / 2 + 1, COLS / 2 - 4);
      standout();
      addstr("2 player");
      standend();
    }
    move(LINES - 1, COLS - 1);
    refresh();
    switch(getch()){
    case KEY_UP:
      two_player_mode = 0;
      break;
    case KEY_DOWN:
      two_player_mode = 1;
      break;
    case 10:
      move(LINES / 2, COLS / 2 - 4);
      addstr("         ");
      move(LINES / 2 + 1, COLS / 2 - 4);
      addstr("         ");
      return;
    case KEY_ENTER:
      move(LINES / 2, COLS / 2 - 4);
      addstr("         ");
      move(LINES / 2 + 1, COLS / 2 - 4);
      addstr("         ");
      return;
      return;
    }
    usleep(60000);
  }
  refresh();
}

void init_game(){
  initscr();
  clear();
  nodelay(stdscr, true);
  noecho();
  keypad(stdscr, true);
  select_num_players();
  init_game_board();
  paddle_pos = (LINES / 2) - (PADDLE_SIZE / 2);
  if(two_player_mode == 1)
    paddle_pos_2 = (LINES / 2) - (PADDLE_SIZE / 2);
  score = 0;
  player_two_score = 0;
  game_over = 0;
  count_in();
  spawn_ball();
}

int main(){
  
  init_game();
 
  while(game_over == 0){
    usleep(60000);
    move_ball();
    draw_paddle();
    refresh();
    move_paddle();
    write_score();
    refresh();
    if(game_over == 1){
      set_game_over_text();
      if(game_over == 0)
	      init_game();
    }
  }

  endwin();
  return 0;
}
