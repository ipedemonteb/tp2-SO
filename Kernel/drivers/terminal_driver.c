// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/terminal_driver.h"
#include "../include/pipes.h"
#include "../include/process_manager.h"
#include "../include/video_driver.h"

#define MAX_SCREEN 6500
#define MAX_FG 128
#define CLEAR -33

static uint16_t lastX;

static int32_t currentX;

t_char screen[MAX_SCREEN];

static uint16_t width;
static uint16_t height;
static uint8_t char_height;
static uint8_t char_width;

static pipe_t *terminal_buffer;

static t_char blanck = {0, BLACK, BLACK};

static int32_t fg[MAX_FG] = {0};

void add_to_fg(uint16_t pid) { fg[pid] = 1; }

void rm_from_fg(uint16_t pid) { fg[pid] = 0; }

void t_kill_fg() {
  for (uint8_t i = 0; i < MAX_FG; i++) {
    if (fg[i]) {
      kill(i);
    }
  }
}

void re_draw_line(uint32_t from, uint32_t to) {
  for (uint16_t i = from; i < to; i++) {
    drawchar(screen[i].character, i % width, i / width, screen[i].fg_colour,
             screen[i].bg_colour);
  }
}

void re_draw_screen(void (*fn)(uint32_t)) {
  for (uint16_t i = 0; i < width * height; i++) {
    fn(i);
  }
}

static uint16_t displacement;

static void move_t_char(uint32_t i) {
  if (i + displacement < MAX_SCREEN)
    screen[i] = screen[i + displacement];
  else
    screen[i] = blanck;
}

int8_t t_move_screen_up(uint8_t amount) {
  if (!amount)
    return 0;
  if ((int)(currentX / width) - amount < 0)
    return -1;

  displacement = amount * width;
  re_draw_screen(move_t_char);
  move_screen_up(char_height * amount);
  currentX -= displacement;
  lastX -= displacement;
  return 0;
}

static void clear_t_char(uint32_t i) { screen[i] = blanck; }

void t_clear() {
  re_draw_screen(clear_t_char);
  clear();
  currentX = 0;
  lastX = 0;
}

void start_line(uint8_t new) {
  if (new) {
    if (currentX / width >= height - 1) {
      t_move_screen_up(1);
    }
    currentX += width;
  }
  currentX = (currentX / width) * width;
}

void t_move_cursor(int8_t amount) {
  if (currentX + amount < 0 || currentX + amount > lastX) {
    return;
  }

  drawchar(screen[currentX].character, currentX % width, currentX / width,
           screen[currentX].fg_colour, screen[currentX].bg_colour);
  currentX += amount;
  drawchar(screen[currentX].character, currentX % width, currentX / width,
           BLACK, WHITE);
}

void t_set_cursor() { t_move_cursor(0); }

void t_draw_line(char *line, uint8_t new) {
  start_line(new);
  uint32_t start_position = currentX;
  uint16_t i = 0;
  while (line[i]) {
    screen[currentX].character = line[i++];
    screen[currentX].bg_colour = BLACK;
    screen[currentX].fg_colour = WHITE;
    currentX++;
  }
  i = currentX;
  while (screen[currentX].character) {
    screen[currentX++] = blanck;
  }
  re_draw_line(start_position, currentX);
  currentX = i;
  lastX = currentX;
}

void print_t_char(t_char c) {
  drawchar(c.character, currentX % width, currentX / width, c.fg_colour,
           c.bg_colour);
}

void t_off_cursor() { print_t_char(screen[currentX]); }

void t_insert_char(char c) {
  if (lastX >= width * height - 1) {
    t_move_screen_up(1);
  }
  uint16_t aux = currentX;
  currentX = lastX;
  lastX++;
  while (currentX > aux) {
    screen[currentX] = screen[currentX - 1];
    print_t_char(screen[currentX]);
    currentX--;
  }
  screen[currentX].character = c;
  screen[currentX].fg_colour = WHITE;
  screen[currentX].bg_colour = BLACK;
  print_t_char(screen[currentX]);
  t_move_cursor(1);
}

void t_remove_char() {
  if (!screen[currentX - 1].character) {
    return;
  }

  uint16_t aux = --currentX;
  while (currentX <= lastX) {
    screen[currentX] = screen[currentX + 1];
    print_t_char(screen[currentX]);
    currentX++;
  }
  lastX--;
  currentX = aux;
}

int8_t t_increase_font_size() {
  if (char_height >= 4 * 16) {
    return -1;
  }

  char_height += 16;
  char_width += 8;
  width = getWidth() / char_width;
  height = getHeight() / char_height;
  fontSizeBigger();
  t_clear();
  return 0;
}

int8_t t_decrease_font_size() {
  if (char_height <= 16) {
    return -1;
  }

  char_height -= 16;
  char_width -= 8;
  width = getWidth() / char_width;
  height = getHeight() / char_height;
  fontSizeSmaller();
  t_clear();
  return 0;
}

void start_terminal() {
  terminal_buffer = get_terminal_buffer();
  char_height = 16;
  char_width = 8;
  width = getWidth() / char_width;
  height = getHeight() / char_height;
  t_clear();
}

void terminal() {
  char c;
  while (direct_read(terminal_buffer, &c)) {
    switch (c) {
    case RIGHT_ARROW:
      t_move_cursor(1);
      break;
    case LEFT_ARROW:
      t_move_cursor(-1);
      break;
    case UP_ARROW:
    case DOWN_ARROW:
      t_off_cursor();
      t_draw_line("", 0);
      t_set_cursor();
      break;
    case DELETE:
      t_remove_char();
      t_set_cursor();
      break;
    case CLEAR:
      t_clear();
      break;
    case '\n':
      t_off_cursor();
      t_draw_line("", 1);
      t_set_cursor();
      break;
    case 0:
      break;
    default:
      if (c <= 9) {
        t_move_screen_up(c);
      } else {
        t_insert_char(c);
      }
      break;
    }
  }
}
