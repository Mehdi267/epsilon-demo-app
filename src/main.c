#include <eadk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DIFF 15  
#define WIDTH 70 
#define HEIGHT 10  

const char eadk_app_name[]
#if PLATFORM_DEVICE
  __attribute__((section(".rodata.eadk_app_name")))
#endif
  = "App";

const uint32_t eadk_api_level
#if PLATFORM_DEVICE
  __attribute__((section(".rodata.eadk_api_level")))
#endif
  = 0;

eadk_color_t random_color() {
  return (eadk_color_t)eadk_random();
}

eadk_rect_t random_screen_rect() {
  uint16_t x = eadk_random() % (EADK_SCREEN_WIDTH - 1);
  uint16_t y = eadk_random() % (EADK_SCREEN_HEIGHT - 1);
  uint16_t width = eadk_random() % (EADK_SCREEN_WIDTH - x);
  uint16_t height = eadk_random() % (EADK_SCREEN_HEIGHT - y);
  return (eadk_rect_t){x, y, width, height};
}

void draw_random_colorful_rectangles() {
  eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_black);
  for (int i=0; i<100; i++) {
    eadk_display_push_rect_uniform(random_screen_rect(), random_color());
  }
}

void draw_random_buffer() {
  eadk_rect_t rect = {0, 0, 30, 30};
  size_t bufferSize = rect.width*rect.height*sizeof(eadk_color_t);
  eadk_color_t * pixels = (eadk_color_t *)malloc(bufferSize);
  if (pixels == NULL) {
    return;
  }
  memset(pixels, 0, bufferSize);
  for (int i=0; i<rect.width*rect.height; i++) {
    pixels[i] = random_color();
  }
  eadk_display_push_rect(rect, pixels);
  free(pixels);
}

void collision_bordure(eadk_rect_t* rec_src, int *dir_x, int *dir_y){
  if ((rec_src->y < 0) || (rec_src->y + rec_src->height > EADK_SCREEN_HEIGHT)){
    *dir_y*=-1;
  } 
  if ((rec_src->x < 0) || (rec_src->x + rec_src->width > EADK_SCREEN_WIDTH)){
    *dir_x*=-1;
  }
}

// void collision_bordure(eadk_rect_t* rec_src, eadk_rect_t* target_src){
//   if ((rec_src->y < 0) || (rec_src->y + rec_src->height > EADK_SCREEN_HEIGHT)){
//   } 
//   if ((rec_src->x < 0) || (rec_src->x + rec_src->width > EADK_SCREEN_WIDTH)){
//   }
// }


#define MOV 5
void move_pointer() {
  uint16_t size = 10;
  eadk_rect_t cursor = {(EADK_SCREEN_WIDTH-size)/2, (EADK_SCREEN_HEIGHT-size)/2, size, size};
  eadk_rect_t bar = {(EADK_SCREEN_WIDTH-WIDTH)/2, EADK_SCREEN_HEIGHT - DIFF , WIDTH, HEIGHT};
  eadk_rect_t screen = {0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT};
  int dir_x = 1;
  int dir_y = -1;
  while (true) {
    eadk_keyboard_state_t keyboard = eadk_keyboard_scan();
    // eadk_display_pull_rect(bar, eadk_color_white);
    if (eadk_keyboard_key_down(keyboard, eadk_key_back)) {
      return;
    }
    if (eadk_keyboard_key_down(keyboard, eadk_key_left) && bar.x > 0) {
      bar.x -= MOV;
    }
    if (eadk_keyboard_key_down(keyboard, eadk_key_right) && bar.x < EADK_SCREEN_WIDTH-size ) {
      if (!(bar.x + WIDTH + MOV > EADK_SCREEN_WIDTH))
        bar.x += MOV;
    }
    eadk_rect_t temp;
    memcpy(&temp, &cursor, sizeof(eadk_rect_t));
    temp.x += MOV*dir_x;
    temp.y += MOV*dir_y;
    collision_bordure(&temp, &dir_x, &dir_y);
    cursor.x += MOV*dir_x;
    cursor.y += MOV*dir_y;
    

    eadk_display_push_rect_uniform(screen, eadk_color_black);
    eadk_display_push_rect_uniform(cursor, eadk_color_green);
    eadk_display_push_rect_uniform(bar, eadk_color_white);
    eadk_display_wait_for_vblank();
    eadk_timing_msleep(20);
  }
}



int main(int argc, char * argv[]) {
  printf("External data : '%s'\n", eadk_external_data);
  // eadk_timing_msleep(3000);
  move_pointer();
}
