#ifndef DOROTHY_H
#define DOROTHY_H

typedef struct {
  byte r;
  byte g;
  byte b;
} RGB;

#define LCDRED   {255, 0, 0}
#define LCDGREEN {0, 255, 0}
#define LCDBLUE  {0, 0, 255}

/* Volumentric mean radius in meters */
#define EARTH_RADIUS 6371000

/* Degree to radian conversion */
#define TO_RAD (3.1415926536 / 180)

#define MINLOGDISTANCE 2.0
#define MAXLOGDISTANCE 7.0
#define COLORSCALE 51.2

#endif /* DOROTHY_H */
