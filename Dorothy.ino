#include <Wire.h>
#include <rgb_lcd.h>
#include <LGPS.h>

gpsSentenceInfoStruct info;
char buff[256];
char lcdbuff1[256];
char lcdbuff2[256];

rgb_lcd lcd;

int colorR = 0;
int colorG = 255;
int colorB = 0;

static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

/* Convert GPS-type location value into decimal location */
static double getDecimalPos(const char *str)
{
  bool isLongitude;
  double pos = 0.0;
  char i;

  if(str[4] == '.') { /* Latitude */
    char intpart[2];
    char floatpart[7];
    strncpy(intpart, str, 2);
    pos = atof(intpart);
    strncpy(floatpart, &str[2], 7);
    pos = pos + atof(floatpart) / 60.0;
    if (str[10] == 'S') {
      pos = -1.0 * pos;
    } else if (str[10] == 'N') {
      /* pass */
    } else {  /* unexpected */
      pos = 0.0;
    }
  } else if (str[5] == '.') {  /* Longitude */
    char intpart[3];
    char floatpart[7];
    strncpy(intpart, str, 3);
    pos = atof(intpart);
    strncpy(floatpart, &str[3], 7);
    pos = pos + atof(floatpart) / 60.0;
    if (str[11] == 'W') {
      pos = -1.0 * pos;
    } else if (str[11] == 'E') {
      /* pass */
    } else {  /* unexpected */
      pos = 0.0;
    }
  }
  return pos;
}


void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  double latitude;
  double longitude;
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    lcd.clear();
    lcd.setCursor(0, 0);
    if (num < 1) {
       lcd.print("No satellites...");
    } else {
       sprintf(lcdbuff1, "Lat:%.4f",latitude);
       sprintf(lcdbuff2, "Lon:%.4f",longitude);
       lcd.print(lcdbuff1);
       lcd.setCursor(0,1);
       lcd.print(lcdbuff2);
    };
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
}

void setup() {
  // put your setup code here, to run once:

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
    
  lcd.setRGB(colorR, colorG, colorB);
    
  // Print a message to the LCD.
  lcd.setCursor(0,0);
  lcd.print("Long way");
  lcd.setCursor(0,1);
  lcd.print("  from home...");

  Serial.begin(115200);
  LGPS.powerOn();
  Serial.println("LGPS Power on, and waiting ..."); 
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);
  delay(2000);
}
