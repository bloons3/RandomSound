#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioPlaySdWav           playWav1;
AudioOutputUSB           audioOutput; // must set Tools > USB Type to Audio
AudioOutputAnalog        dac;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioConnection          patchCord3(playWav1, 0, dac, 0);

//code stolen from Andom and Matchfire w/ love
//also stolen from Audio examples

unsigned int audnum = 0; //Number of audo files found.

File root;


// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(9));

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(400);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  Serial.println("SD CARD INIT!");

//  root = SD.open("/");
//  printDirectory(root, 0);
  countWAV();


}

void playFile(const char *filename)
{
  playWav1.play(filename);
  // A brief delay for the library read WAV info
  delay(5);
  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
  }
}


int filePlay = 0;

void loop() {
  playSDWAV(random(1, audnum ));
  delay(5);
}


void countWAV() {
  //Counts the number of appropriate JPEG files found.
  Serial.println("Counting WAV images.");
  unsigned int count = 0;
  File dir = SD.open("/"); //Open the base directory.
  dir.rewindDirectory(); //Just in case someone hasn't been kind.
  File fi = dir.openNextFile(); //Open first file.
  while (fi) {
    //While we have the file open.
    if (! fi.isDirectory()) {
      //A file. Check if it is a wav
      if ((String(fi.name()).endsWith(".wav")) or (String(fi.name()).endsWith(".WAV"))) {
        //It does. Count it.
        //Serial.print(fi.name());
        //Serial.println(" found.");
        count ++;
      }
    }
    fi.close(); //Close the file
    delay(5); //Delay a little bit to be safe.
    fi = dir.openNextFile(); //Open the next file.
  }
  dir.rewindDirectory();
  dir.close(); //Close the directory
  audnum = count;
  Serial.print(count);
  Serial.println(" WAV files found.");
}

void playSDWAV(unsigned int fnum) {
  //Functions much like countWAV, except when it finds the audio it's looking for it stops and plays it.
  unsigned int count = 0;
  Serial.print("Playing Number ");
  Serial.println(fnum);
  File dir = SD.open("/");
  dir.rewindDirectory();
  File fi = dir.openNextFile();
  while (fi) {
    if (! fi.isDirectory()) {
      if ((String(fi.name()).endsWith(".wav")) or (String(fi.name()).endsWith(".WAV"))) {
        count ++;
        if (count == fnum) {
          //We've found the image we're looking for. Display it.
          Serial.print("Audio ");
          Serial.print(fi.name());
          Serial.print(" Found at position ");
          Serial.println(count);
          fi.close();
          delay(5);
          playFile(fi.name());  // filenames are always uppercase 8.3 format
          delay(5);
          break;
        }
      }
    }
    fi.close();
    delay(5);
    fi = dir.openNextFile();
  }
  dir.rewindDirectory();
  dir.close();
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
