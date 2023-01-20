#include <Arduino.h>

void drawBibleVerse();
void countVerses();
void readBibleVerse(int n);

// mapping suggestion for ESP32, e.g. LOLIN32, see .../variants/.../pins_arduino.h for your board
// NOTE: there are variants with different pins for SPI ! CHECK SPI PINS OF YOUR BOARD
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V

// include library, include base class, make path known
#include <GxEPD.h>
#include <GxGDEW029T5/GxGDEW029T5.h> // 2.9" b/w UC8151 (IL0373)

#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include "SPIFFS.h"

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/17, /*RST=*/16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/16, /*BUSY=*/4);

uint16_t numberOfVerses = 0;
char bibleVerse[1024];
char bibleVerseLocation[32];

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  display.init(115200);

  countVerses();
  readBibleVerse(random(0, numberOfVerses));

  drawBibleVerse();
  display.update();
  display.update();
  display.update();

  display.powerDown();

  Serial.println("setup done");
}

void loop(){};

void countVerses()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/verses.txt");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Counting verses:");
  bool readingVerseLocation = false;
  while (file.available())
  {
    file.find('\n');
    if (readingVerseLocation)
    {
      numberOfVerses++;
      readingVerseLocation = false;
    }
    else
    {
      readingVerseLocation = true;
    }
  }

  Serial.println("Number of verses: " + String(numberOfVerses));
  file.close();
}

void readBibleVerse(int n)
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/verses.txt");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("Reading verse: " + String(n));

  uint16_t currentVerse = 0;
  bool readingVerseLocation = false;
  for (int i = 0; i < n && file.available(); i++)
  {
    file.find('\n');
    file.find('\n');
  }

  strcpy(bibleVerse, (file.readStringUntil('\n') + "\n").c_str());
  strcpy(bibleVerseLocation, (file.readStringUntil('\n') + "\n").c_str());

  file.close();

  Serial.println("Verse: " + String(bibleVerse));
  Serial.println("Verse location: " + String(bibleVerseLocation));
}

void drawBibleVerse()
{
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(0, 14);

  display.println(bibleVerse);
  display.println(bibleVerseLocation);
}