#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1

typedef enum led_colors {
  red_led = RED_LED,
  green_led = GREEN_LED,
	blue_led = BLUE_LED,
} led_colors_t;


typedef enum onOff {
  off = 0,
  on = 1,

} onOff_t;

char led_mapping[3] [2] = {{0, red_led}, {1, green_led}, {2, blue_led}};

void initLED(void);

void offRGB(void);

void ledControl (led_colors_t colors, onOff_t onOff);