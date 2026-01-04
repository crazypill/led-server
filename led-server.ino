/*
 *  LED strip light Webserver controller -  Set processor type to Huzzah! ESP8266 - 80Mhz
 */

#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>        // Include the mDNS library

Adafruit_SSD1306  display = Adafruit_SSD1306(128, 32, &Wire);

ESP8266WebServer webServer(80);
ESP8266HTTPUpdateServer httpUpdateServer;

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif


#define RED_LED 14
#define GRN_LED 12
#define BLU_LED 13
#define WHT_LED 15

#define LATENCY 2500 

// define to see what's happening with the OLED feather wing
//#define USE_OLED

static const uint8_t PROGMEM s_folabs_logo[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x20, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x30, 0x40,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x08, 0x38, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x7c, 0x10, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xfc, 0x98,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x11, 0xf2, 0xfe, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x36, 0xf8, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0xb9, 0x88,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x11, 0xfe, 0x3f, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x7e, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x3c, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x08, 0x1c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x18, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x18, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0x99, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x08, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


typedef struct tagNVSettings
{
  int r;
  int g;
  int b;
  int w;
  int rainbow;
  int brightness;
  int reserved1;
  int reserved2;
} NVSettings;



typedef struct tagColorSpec
{
  int r1;
  int g1;
  int b1;
  
  int r2;
  int g2;
  int b2;

  int steps;
  int delayTime;
} ColorSpec;

static const ColorSpec kColorSpecTable[] = 
{
  // red -> yellow
  { 255,   0,   0, 255, 255,   0, 1024, LATENCY },
  
  // yellow -> green
  { 255, 255,   0,   0, 255,   0, 1024, LATENCY },

  // green -> cyan
  {   0, 255,   0,   0, 255, 255, 1024, LATENCY },

  // cyan -> blue
  {   0, 255, 255,   0,   0, 255, 1024, LATENCY },

  // blue to magenta
  {   0,   0, 255,  255,   0, 255, 1024, LATENCY },

  // magenta -> red
  { 255,   0, 255,  255,   0,   0, 1024, LATENCY },
};

#define countof(a) (sizeof( a ) / sizeof( a[0] ))

static const char* ssid     = "Vitesse";
static const char* password = "SummerSummer7";

static NVSettings sSettings = {};
static int        sColorSpecIndex = 0;
static int        sFade_step      = 0;


void single_blink()
{
    digitalWrite( LED_BUILTIN, HIGH );
    delay( 500 );
    digitalWrite( LED_BUILTIN, LOW );
    delay( 500 );
}


void restoreNVSettings()
{
    uint8_t* settings = (uint8_t*)&sSettings;
  
    for( int i = 0; i < sizeof( NVSettings ); i++ )
      settings[i] = EEPROM.read( i );

    // do the restore part - rainbow setting is already restored
    set_rgb( sSettings.r, sSettings.g, sSettings.b );
    set_white( sSettings.w );
}


void writeNVSettings()
{
  uint8_t* settings = (uint8_t*)&sSettings;
  for( int i = 0; i < sizeof( NVSettings ); i++ )
    EEPROM.write( i, settings[i] );

   if( EEPROM.commit() )
      Serial.println( "EEPROM successfully committed" );
   else
      Serial.println("ERROR! EEPROM commit failed");
}

void setup() 
{
  pinMode( BUTTON_A, INPUT_PULLUP );
  pinMode( BUTTON_B, INPUT_PULLUP );
  pinMode( BUTTON_C, INPUT_PULLUP );
  pinMode( LED_BUILTIN, OUTPUT );

  Serial.begin( 115200 );
  delay( 100 );
  EEPROM.begin( sizeof( NVSettings ) );
  delay( 100 );

  Wire.begin(); 

#ifdef USE_OLED
  Serial.println( "Starting up OLED" );
  display.begin( SSD1306_SWITCHCAPVCC, 0x3C ); // Address 0x3C for 128x32

  display.clearDisplay();
  display.drawBitmap( 0, 0, s_folabs_logo, 128, 32, 1 );
  display.setTextSize( 1 );
  display.setTextColor( WHITE );
  display.setCursor( 42, (32 - 6) / 2  );
  display.println( "Far Out Labs" );
  display.setCursor( 0, 0 );
  display.display(); // actually display all of the above

  delay( 2000 );
  
  // We start by connecting to a WiFi network
  display.clearDisplay();
  display.setCursor( 2, (32 - 6) / 2  );
  display.print("Connecting to ");
  display.println( ssid );
  display.setCursor( 0, 0 );
  display.display();
#endif
  
  WiFi.begin( ssid, password );

#ifdef USE_OLED
  display.setCursor( 2, display.height() - 12 );
  while( WiFi.status() != WL_CONNECTED ) 
  {
    delay( 500 );
    display.print(".");
    display.display();
  }

  display.clearDisplay();
  display.setCursor( 2, 2 );
  display.println( "WiFi connected" );  
  display.println( "IP address: " );
  display.println( WiFi.localIP() );
  display.display();
#else
  while( WiFi.status() != WL_CONNECTED ) 
    single_blink();
#endif

  httpUpdateServer.setup( &webServer );

  webServer.on( "/color", HTTP_POST, []() {
    String r = webServer.arg("r");
    String g = webServer.arg("g");
    String b = webServer.arg("b");
    
    single_blink();
    sSettings.rainbow = false;
    set_rgb( r.toInt(), g.toInt(), b.toInt() );
    writeNVSettings();
    webServer.send( 404, "text/plain", "color: " + String( r ) + ", " + String( g ) + ", " + String( b ) + "\n" );

#ifdef USE_OLED
    display.clearDisplay();
    display.setCursor( 2, 2 );
    display.print( "color: " );  
    display.println(  String( r ) + ", " + String( g ) + ", " + String( b ) );
    display.display();
#endif    
  } );


  webServer.on( "/white", HTTP_POST, []() {
    String w = webServer.arg("w");
    single_blink();
    set_white( w.toInt() );
    writeNVSettings();
    webServer.send( 404, "text/plain", "white: " + String( w ) + "\n" );

#ifdef USE_OLED
    display.clearDisplay();
    display.setCursor( 2, 2 );
    display.print( "white: " );  
    display.println(  String( w ) );
    display.display();
#endif    
  } );

  
  webServer.on( "/rainbow", HTTP_GET, []() {
#ifdef USE_OLED
    display.clearDisplay();
    display.setCursor( 2, 2 );
    display.println( "rainbow requested" );  
    display.display();
#endif  
    single_blink();  
    sSettings.rainbow = true;
    writeNVSettings();
    webServer.send( 404, "text/plain", "rainbow\n" );
  } );


webServer.on( "/brightness", HTTP_POST, []() {
    String z = webServer.arg("b");
    single_blink();
    set_brightness( z.toInt() );
    writeNVSettings();
    webServer.send( 404, "text/plain", "brightness: " + String( z ) + "\n" );

#ifdef USE_OLED
    display.clearDisplay();
    display.setCursor( 2, 2 );
    display.print( "brightness: " );  
    display.println(  String( z ) );
    display.display();
#endif    
  } );



  webServer.begin();
  Serial.println( "Web server started" );
  single_blink();

  if( !MDNS.begin( "lights" ) ) 
    Serial.println( "Error setting up mDNS responder!" );
  else
  {
    Serial.println( "mDNS responder started for lights.local" );
    MDNS.addService( "http", "tcp", 80 );
    single_blink();
  }

  // restore settings
  restoreNVSettings();
}



void set_rgb( int r, int g, int b )
{
    if( r > 255 )
        r = 255;
    if( r < 0 )
        r = 0;

    if( g > 255 )
        g = 255;
    if( g < 0 )
        g = 0;

    if( b > 255 )
        b = 255;
    if( b < 0 )
        b = 0;
    
    analogWrite( RED_LED, r );
    analogWrite( GRN_LED, g );
    analogWrite( BLU_LED, b );    
    sSettings.r = r;
    sSettings.g = g;
    sSettings.b = b;
}


void set_white( int w )
{
  if( w > 255 )
      w = 255;
  if( w < 0 )
      w = 0;
    
  analogWrite( WHT_LED, w );
  sSettings.w = w;
}


void set_brightness( int z )
{
  if( z > 255 )
      z = 255;
  if( z < 0 )
      z = 0;
    
  sSettings.brightness = z;
}


void fade_up_rgb( int r, int g, int b, int steps, useconds_t latency )
{
    float dr = r / steps;
    float dg = g / steps;
    float db = b / steps;
    
    for( int i = 0; i < steps; i++ )
    {
        set_rgb( dr * i, dg * i, db * i );
        delayMicroseconds( latency );
    }    
}


// mu = 0..1 (0 = rgb1 and 1 = rgb2, anywhere between is a blend)
void blend_rgb( int r1, int g1, int b1, int r2, int g2, int b2, float mu )
{
    int brightnessInverse = 255 - sSettings.brightness;
  
    if( mu < 0.0f )
        mu = 0.0f;
    if( mu > 1.0f )
        mu = 1.0f;

    float oneMinusAlpha = 1.0f - mu;
    int r = r2 * mu + (r1 * oneMinusAlpha);
    int g = g2 * mu + (g1 * oneMinusAlpha);
    int b = b2 * mu + (b1 * oneMinusAlpha);

    int rb = r - brightnessInverse;
    if( rb < 0 )
      rb = 0;
      
    int gb = g - brightnessInverse;
    if( gb < 0 )
      gb = 0;

    int bb = b - brightnessInverse;
    if( bb < 0 )
      bb = 0;
    
    set_rgb( rb, gb, bb );    
}


void fade_to_rgb( int r, int g, int b, int from_r, int from_g, int from_b, int steps, useconds_t latency )
{
    for( int i = 0; i < steps; i++ )
    {
        blend_rgb( r, g, b, from_r, from_g, from_b, i * (1.0f / steps) );
        delayMicroseconds( latency );
    }    
}


bool fade_to_rgb_with_state( int r, int g, int b, int from_r, int from_g, int from_b, int steps, useconds_t latency )
{
  if( sFade_step >= steps )
  {
    sFade_step = 0;
    return true;
  }
    
  blend_rgb( r, g, b, from_r, from_g, from_b, sFade_step * (1.0f / steps) );
  delayMicroseconds( latency );  
  ++sFade_step;
  return false; // not done
}



void sweep_led( int rmax, int gmax, int bmax )
{
    float r = rmax / 255;
    float g = gmax / 255;
    float b = bmax / 255;
    
    fade_up_rgb( rmax, gmax, bmax, 255, LATENCY );

    for( int i = 0; i < 255; i++ )
    {
        set_rgb( r * (255 - i), g * (255 - i), b * (255 - i) );
        delayMicroseconds( LATENCY );
    }
}



void loop()
{
  MDNS.update();
  webServer.handleClient();

  if( sSettings.rainbow )
  {
    if( sColorSpecIndex >= countof( kColorSpecTable ) )
      sColorSpecIndex = 0;
      
    const ColorSpec* currentColor = &kColorSpecTable[sColorSpecIndex];
    if( fade_to_rgb_with_state( currentColor->r1, currentColor->g1, currentColor->b1, currentColor->r2, currentColor->g2, currentColor->b2, currentColor->steps, currentColor->delayTime ) )
    {
        // when we are done, go to the next color in the table
        ++sColorSpecIndex;
    }
    
    delayMicroseconds( LATENCY * 2 );
  }    
}


// EOF
