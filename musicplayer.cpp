#include <iostream>
using namespace std;
#include <unistd.h>  // for usleep

#include <chrono>
#include <cmath>
#include <algorithm>
#include <wiringPi.h>

//58 for time to say goodbye test
//80 for turret serenade
double songBPM = 80;//80 by default

//vibrato frequencies can be changed
#define VIB_SLOW_FREQ 4
#define VIB_MED_FREQ 6
#define VIB_FAST_FREQ 8


//0.0293 old
const double vibratoRange = 0.015;  //no clue if this is a good vibrato range need to test




const int dirPin1 = 27;
const int stepPin1 = 22;

const int dirPin2 = 23;
const int stepPin2 = 24;

const int dirPin3 = 2;
const int stepPin3 = 3;


//volatile variables for timing interrupts
volatile bool globalStepState1 = false;
volatile unsigned long globalStepDelay1 = 1000;  // in microseconds



//POTENTIAL grace note flags utilizing the absurdly high length numbers
//no one is going to use a 255th note lmao
#define GRACE_NOTE_8TH 255
#define GRACE_NOTE_16TH 254
#define GRACE_NOTE_32ND 253


#define REST 0


// === Octave 2 ===
#define NOTE_C2 65  // 65.41 Hz
#define NOTE_CS2 69
#define NOTE_DF2 69
#define NOTE_D2 74
#define NOTE_DS2 78
#define NOTE_EF2 78
#define NOTE_E2 83
#define NOTE_F2 88
#define NOTE_FS2 93
#define NOTE_GF2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_AF2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_BF2 117
#define NOTE_B2 124

// === Octave 3 ===
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_DF3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_EF3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_GF3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_AF3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_BF3 233
#define NOTE_B3 247

// === Octave 4 ===
#define NOTE_C4 261
#define NOTE_CS4 277
#define NOTE_DF4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_EF4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_GF4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_AF4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_BF4 466
#define NOTE_B4 494

// === Octave 5 ===
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_DF5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_EF5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_GF5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_AF5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_BF5 932
#define NOTE_B5 988



#define BPMtoMilisec 60000

const double BPMs = BPMtoMilisec / songBPM;
double WholeNoteMS = 4 * BPMtoMilisec / songBPM;



#define STACCATO 0x01      // 00000001
#define VIBRATO_SLOW 0x02  // 00000010
#define VIBRATO_MED 0x04   // 00000100
#define VIBRATO_FAST 0x08  // 00001000
#define DOTTED 0x10        // 00010000
#define GLISSANDO 0x20     // 00100000
#define TRILL 0x40         // 01000000
#define UNUSED 0x80        // 10000000
//POTENTIAL Vibrato encoding numbers using the three bits different combinations to represent different numbers and therefore more control over vibrato speed/intensity
struct NoteStorage {
    uint16_t n;     //note frequency
    uint8_t len;    //store length as an 8 bit integer 0 to 255 (whole note = 1, quarter = 4, eighth = 8, sixteenth = 16 etc)
    uint8_t flags;  //bit 0 = staccato, bit1 = vibrato (00000011) = both bits on staccato AND vibrato, can add more
    //functions to assign to the bits like glissando and slurs.  use bitwise and (&) to bitmask it.  use bitwise or (|) to chain multiple flags like STACCATO | DOTTED | GLISS
    // flags:
    // 00000000 none
    // 00000001 staccato
    // 00000010 vibrato slow
    // 00000100 vibrato med
    // 00001000 vibrato fast
    // 00010000 dotted note
    // 00100000 glissando
    // 01000000 trill
    // 10000000

    // Input && 0x01 = staccato on/off etc
};

//Turret 1_______________________________________________________________________________________________________________________________________________________________________________________________________________
const NoteStorage turret1Melody[]  = {
    //for turret srenade  5 flats:  B♭, E♭, A♭, D♭, and G♭.
    //turret 1___________________________________________________________________________________________
    //-------------------
    //measure 1
    { NOTE_BF4, 8, VIBRATO_FAST},  // Eighth note

    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 2
    { NOTE_BF4, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 3


    { NOTE_A4, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 4


    { NOTE_A4, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 5
    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 6
    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 7
    { NOTE_A3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 8

    { NOTE_A3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 9
    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 10
    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 11
    { NOTE_A3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 12

    { NOTE_A3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 13
    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 14
    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 15
    { NOTE_A3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 16

    { NOTE_A3, 8, VIBRATO_FAST},  // Eighth note
    // staccatoMode = STACCATO;
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 17

    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note

    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 18
    { NOTE_BF3, 8, VIBRATO_FAST},  // Eighth note

    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 19
    { NOTE_A3, 8 },             // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 20
    { NOTE_A3, 8 },             // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 21
    { NOTE_A3, 8 },             // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 22
    { NOTE_A3, 8 },             // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 23
    { NOTE_BF3, 8 },  // Eighth note

    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_DF3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 16, STACCATO },  // Sixteenth note
    { NOTE_C3, 16, STACCATO },   // Sixteenth note

    { NOTE_DF3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 24
    { NOTE_A3, 8 },             // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note
    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note
    //-------------------
    //measure 25 (change in rythm)

    { NOTE_EF4, 8, STACCATO },  // Eighth note
    { NOTE_F4, 8, STACCATO },   // Eighth note //dual with BF2

    { NOTE_GF4, 8, STACCATO },  // Eighth note
    { NOTE_EF4, 8, STACCATO },  // Eighth note

    { NOTE_F4, 8, STACCATO },   // Eighth note
    { NOTE_GF4, 8, STACCATO },  // Eighth note

    { NOTE_EF4, 16, STACCATO },  // Sixteenth note
    { NOTE_F4, 16, STACCATO },   // Sixteenth note

    { NOTE_GF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 26 //gets lower

    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_EF4, 8, STACCATO },  // Eighth note //dual with BF2

    { NOTE_F4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_EF4, 8, STACCATO },  // Eighth note
    { NOTE_F4, 8, STACCATO },   // Eighth note

    { NOTE_DF4, 16, STACCATO },  // Sixteenth note
    { NOTE_EF4, 16, STACCATO },  // Sixteenth note

    { NOTE_F4, 8, STACCATO },  // Eighth note


    //-------------------
    //measure 27 //gets lower
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note //dual with BF2

    { NOTE_EF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_EF4, 8, STACCATO },  // Eighth note

    { NOTE_C4, 16, STACCATO },   // Sixteenth note
    { NOTE_DF4, 16, STACCATO },  // Sixteenth note

    { NOTE_EF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 28 //gets lower  base clef

    { NOTE_A3, 8, STACCATO },   // Eighth note
    { NOTE_BF3, 8, STACCATO },  // Eighth note //dual with BF2

    { NOTE_C3, 8, STACCATO },  // Eighth note
    { NOTE_A3, 8, STACCATO },  // Eighth note

    { NOTE_BF3, 8, STACCATO },  // Eighth note
    { NOTE_C3, 8, STACCATO },   // Eighth note

    { NOTE_A3, 16, STACCATO },   // Sixteenth note
    { NOTE_BF3, 16, STACCATO },  // Sixteenth note

    { NOTE_C3, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 29
    { NOTE_BF4, 8 },  // Eighth note

    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 30
    { NOTE_BF4, 8 },  // Eighth note

    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 31
    { NOTE_A4, 8 },  // Eighth note

    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 32
    { NOTE_A4, 8 },  // Eighth note

    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 33
    { NOTE_BF4, 8 },  // Eighth note

    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 34
    { NOTE_BF4, 8 },  // Eighth note

    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 35
    { NOTE_A4, 8 },  // Eighth note

    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 36
    { NOTE_A4, 8 },  // Eighth note

    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 37
    { NOTE_BF4, 8 },  // Eighth note

    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 38
    { NOTE_BF4, 8 },  // Eighth note

    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_DF4, 8, STACCATO },  // Eighth note

    { NOTE_BF4, 16, STACCATO },  // Sixteenth note
    { NOTE_C4, 16, STACCATO },   // Sixteenth note

    { NOTE_DF4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 39
    { NOTE_A4, 8 },  // Eighth note

    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 40
    { NOTE_A4, 8 },  // Eighth note

    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note
    { NOTE_A4, 8, STACCATO },   // Eighth note
    { NOTE_BF4, 8, STACCATO },  // Eighth note
    { NOTE_C4, 8, STACCATO },   // Eighth note

    { NOTE_A4, 16, STACCATO },   // Sixteenth note
    { NOTE_BF4, 16, STACCATO },  // Sixteenth note

    { NOTE_C4, 8, STACCATO },  // Eighth note

    //-------------------
    //measure 41
    { NOTE_BF3, 8 },
    { NOTE_F3, 8, STACCATO },
    { NOTE_BF3, 8 },
    { NOTE_DF4, 8, STACCATO },
    { NOTE_F3, 8 },
    { NOTE_BF3, 8, STACCATO },
    { NOTE_DF4, 8 },
    { NOTE_F3, 8, STACCATO },

    //-------------------
    //measure 42
    { NOTE_BF3, 8 },
    { NOTE_DF4, 8, STACCATO },
    { NOTE_BF3, 8 },
    { NOTE_F3, 8, STACCATO },
    { NOTE_BF3, 8 },
    { NOTE_DF4, 8 },
    { NOTE_EF4, 8 },
    { NOTE_BF3, 8, STACCATO },
    //-------------------
    //measure 43
    { NOTE_AF3, 8 },
    { NOTE_C4, 8, STACCATO },
    { NOTE_AF3, 8 },
    { NOTE_EF4, 8, STACCATO },
    { NOTE_AF3, 8 },
    { NOTE_C4, 8 },
    { NOTE_DF4, 8 },
    { NOTE_BF3, 8, STACCATO },
    //-------------------
    //measure 44
    { NOTE_AF3, 8 },
    { NOTE_C4, 8, STACCATO },
    { NOTE_AF3, 8 },
    { NOTE_EF4, 8, STACCATO },
    { NOTE_AF3, 8 },
    { NOTE_C4, 8 },
    { NOTE_DF4, 8 },
    { NOTE_BF3, 8, STACCATO },
    //-------------------
    //measure 45
    { NOTE_BF3, 8 },
    { NOTE_DF4, 8 },
    { NOTE_BF3, 8, STACCATO },
    { NOTE_F4, 8, STACCATO },
    { NOTE_BF3, 8 },
    { NOTE_DF4, 8 },
    { NOTE_EF4, 8 },
    { NOTE_BF3, 8, STACCATO },
    //-------------------
    //measure 46
    { NOTE_BF3, 8 },
    { NOTE_DF4, 8 },
    { NOTE_BF3, 8, STACCATO },
    { NOTE_F4, 8, STACCATO },
    { NOTE_BF3, 8 },
    { NOTE_C4, 8 },
    { NOTE_DF4, 8 },
    { NOTE_BF3, 8, STACCATO },
    //-------------------
    //measure 47
    { NOTE_F3, 8 },
    { NOTE_G3, 8 },
    { NOTE_AF3, 8 },
    { NOTE_BF3, 8 },
    { NOTE_C4, 8 },
    { NOTE_BF3, 8 },
    { NOTE_AF3, 8 },
    { NOTE_G3, 8 },

    //measure 48
    { NOTE_F3, 1,VIBRATO_SLOW}  //whole note
};


//Turret 2_______________________________________________________________________________________________________________________________________________________________________________________________________________
const NoteStorage turret2Melody[] = {
    //for turret srenade  5 flats:  B♭, E♭, A♭, D♭, and G♭.
    //turret 2___________________________________________________________________________________________
    //-------------------
    //Measure 1
    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note
    //-------------------
    //measure 2
    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_F4, 8 },  // Eighth note

    //-------------------
    //measure 3
    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note
    //-------------------
    //measure 4
    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_EF4, 8 },  // Eighth note

    // -------------------
    //Measure 5
    { REST, 8 },  //Eighth rest

    { NOTE_BF4, 8 },            //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_F4, 8, STACCATO },   //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_EF4, 8 },            //Eighth note
    //-------------------
    //Measure 6
    { REST, 8 },  //Eighth rest

    { NOTE_BF4, 8 },            //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_F4, 8, STACCATO },   //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_EF4, 8 },            //Eighth note
    //-------------------
    //Measure 7
    { REST, 8 },  //Eighth rest

    { NOTE_A4, 8 },             //Eighth note
    { NOTE_C4, 8 },             //Eighth note
    { NOTE_A4, 8, STACCATO },   //Eighth note
    { NOTE_EF4, 8, STACCATO },  //Eighth note
    { NOTE_A4, 8, STACCATO },   //Eighth note
    { NOTE_C4, 8 },             //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    //-------------------
    //Measure 8
    { REST, 8 },  //Eighth rest

    { NOTE_A4, 8 },   //Eighth note
    { NOTE_C4, 8 },   //Eighth note
    { NOTE_A4, 8 },   //Eighth note
    { NOTE_EF4, 8 },  //Eighth note
    { NOTE_A4, 8 },   //Eighth note
    { NOTE_C4, 8 },   //Eighth note
    { NOTE_DF4, 8 },  //Eighth note
    //-------------------
    //Measure 9
    { REST, 8 },  //Eighth rest


    { NOTE_BF4, 8 },            //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_F4, 8, STACCATO },   //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_EF4, 8 },            //Eighth note
    //-------------------
    //Measure 10
    { REST, 8 },  //Eighth rest

    { NOTE_BF4, 8 },            //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_F4, 8, STACCATO },   //Eighth note
    { NOTE_BF4, 8, STACCATO },  //Eighth note
    { NOTE_C4, 8 },             //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    //-------------------
    //Measure 11
    { REST, 8 },  //Eighth rest

    { NOTE_F4, 8 },             //Eighth note
    { NOTE_EF4, 8 },            //Eighth note
    { NOTE_DF4, 8, STACCATO },  //Eighth note
    { NOTE_EF4, 8, STACCATO },  //Eighth note
    { NOTE_DF4, 8, STACCATO },  //Eighth note
    { NOTE_C4, 8 },             //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    //-------------------
    //Measure 12
    { REST, 8 },  //Eighth rest

    { NOTE_EF4, 8 },            //Eighth note
    { NOTE_DF4, 8 },            //Eighth note
    { NOTE_C4, 8, STACCATO },   //Eighth note
    { NOTE_DF4, 8, STACCATO },  //Eighth note
    { NOTE_C4, 8, STACCATO },   //Eighth note
    { NOTE_BF4, 8 },            //Eighth note
    { NOTE_A4, 8 },             //Eighth note
    //-------------------
    //Measure 13
    { NOTE_BF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_A4, 8 },           //Eighth note
    { NOTE_GS4, 4, DOTTED },  //Dotted quarter note
    { NOTE_G4, 8 },           //Eighth note
    //-------------------
    //Measure 14
    { NOTE_AF4, 4 },  //Quarter note
    { NOTE_BF4, 2 },  //Half note

    { REST, 8 },  //Eighth rest

    { NOTE_C3, 16 },   // Sixteenth note
    { NOTE_BF3, 16 },  // Sixteenth note
    //-------------------
    //Measure 15
    { NOTE_DF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_C4, 8 },           //Eighth note
    { NOTE_BF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_DF4, (8) },        //Eighth note

    //-------------------
    //Measure 16
    { NOTE_C4, 2 },  //Half note
    { REST, 4 },     //Quarter rest
    { REST, 8 },     //Eighth rest

    { NOTE_EF4, 8 },  //Eighth note
    //-------------------
    //Measure 17
    { NOTE_DF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_C4, 16 },          // Sixteenth note
    { NOTE_BF4, 16 },         // Sixteenth note
    { NOTE_C4, 16 },          // Sixteenth note
    { NOTE_DF4, 4 },          //Quarter note

    { REST, 16 },     // Sixteenth rest
    { NOTE_EF4, 8 },  // Eighth note
    //-------------------
    //Measure 18
    { NOTE_F4, 4 },           //Quarter note
    { NOTE_EF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_DF4, 4 },          //Quarter note
    { NOTE_C4, 8 },           //Eighth note


    //-------------------
    // //Measure 19
    { NOTE_DF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_C4, 8 },           //Eighth note
    { NOTE_BF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_A4, 16 },          // Sixteenth note
    { NOTE_AF4, 16 },         // Sixteenth note
    //-------------------
    //measure 20

    { NOTE_G4, 2 },           //Half note
    { NOTE_AF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_BF4, 8 },          //Eighth note

    //-------------------
    //measure 21
    { NOTE_C4, 4, DOTTED },  //Dotted quarter note
    { NOTE_DF4, 8 },         //Eighth note
    { NOTE_C4, 4, DOTTED },  //Dotted quarter note
    { NOTE_DF4, 8 },         //Eighth note
    //-------------------
    //measure 22
    { NOTE_F4, 4 },    //Quarter note
    { NOTE_EF4, 2 },   //Half note
    { REST, 8 },       //Eighth rest
    { NOTE_EF4, 16 },  //Sixteenth note
    { NOTE_F4, 16 },   //Sixteenth note

    //-------------------
    // measure 23
    { NOTE_GF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_F4, 8 },           //Eighth note
    { NOTE_EF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_DF4, 16 },         //Sixteenth note
    { NOTE_EF4, 16 },         //Sixteenth note

    //-------------------
    //measure 24

    { NOTE_C4, 2, DOTTED },  //Dotted Half note
    { REST, 8 },             //Eighth rest
    { NOTE_EF4, 16 },        //Sixteenth note
    { NOTE_F4, 16 },         //Sixteenth note

    //-------------------
    //measure 25

    { NOTE_GF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_AF4, 8 },          //Eighth note
    { NOTE_GF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_BF4, 8 },          //Eighth note

    //-------------------
    //measure 26
    { NOTE_AF4, GRACE_NOTE_8TH },  //(grace note indicated by length 0 and length is subtracted off of the following note)
    { NOTE_GF4, 2, DOTTED },       //Dotted Half note (will have the grace note length (a sixteenth note) subtracted off to maintain measure timeing)
    { REST, 8 },                   //Eighth rest
    { NOTE_GF4, 16 },              //Sixteenth note
    { NOTE_AF4, 16 },              //Sixteenth note

    //-------------------
    //measure 27
    { NOTE_GF4, 4 },          //Quarter note
    { NOTE_F4, 4 },           //Quarter note
    { NOTE_EF4, 4, DOTTED },  //Dotted quarter note
    { NOTE_F4, 16 },          //Sixteenth note
    { NOTE_EF4, 16 },         //Sixteenth note
    //-------------------
    //measure 28

    { NOTE_DF4, 2 },  //Half note
    { NOTE_C4, 4 },   //Quarter note
    { NOTE_A3, 4 },   //Quarter note

    //-------------------
    //measure 29 to 30  //start repeat like beginning

    //-------------------
    //measure 29
    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note
    //-------------------
    //measure 30
    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note

    { REST, 8 },      // Eighth rest
    { NOTE_DF4, 8 },  // Eighth note


    //-------------------
    //measure 31
    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note
    //-------------------
    //measure 32
    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note

    { REST, 8 },     // Eighth rest
    { NOTE_C4, 8 },  // Eighth note


    //-------------------
    //measure 33
    { REST, 8 },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_F4, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_EF5, 8, STACCATO },
    //-------------------
    //measure 34
    { REST, 8 },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_F4, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_EF5, 8, STACCATO },
    //-------------------
    //measure 35
    { REST, 8 },
    { NOTE_A4, 8, STACCATO },
    { NOTE_C5, 8, STACCATO },
    { NOTE_A4, 8, STACCATO },
    { NOTE_EF5, 8, STACCATO },
    { NOTE_A4, 8, STACCATO },
    { NOTE_C5, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    //-------------------
    //measure 36
    { REST, 8 },
    { NOTE_A4, 8, STACCATO },
    { NOTE_C5, 8, STACCATO },
    { NOTE_A4, 8, STACCATO },
    { NOTE_EF5, 8, STACCATO },
    { NOTE_A4, 8, STACCATO },
    { NOTE_C5, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    //-------------------
    //measure 37
    { REST, 8 },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_F4, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_EF5, 8, STACCATO },
    //-------------------
    //measure 38
    { REST, 8 },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_F4, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_C4, 8, STACCATO },
    { NOTE_DF4, 8, STACCATO },
    //-------------------
    //measure 39
    { REST, 8 },
    { NOTE_F4, 8, STACCATO },
    { NOTE_EF5, 8, STACCATO },
    { NOTE_DF4, 8, STACCATO },
    { NOTE_EF4, 8, STACCATO },
    { NOTE_DF4, 8, STACCATO },
    { NOTE_C4, 8, STACCATO },
    { NOTE_DF4, 8, STACCATO },
    //-------------------
    //measure 40
    { REST, 8 },
    { NOTE_EF4, 8, STACCATO },
    { NOTE_DF5, 8, STACCATO },
    { NOTE_C4, 8, STACCATO },
    { NOTE_DF4, 8, STACCATO },
    { NOTE_C4, 8, STACCATO },
    { NOTE_BF4, 8, STACCATO },
    { NOTE_A4, 8, STACCATO },


    //-------------------
    //measure 41
    { NOTE_BF4, 4, DOTTED },  // dotted quarter
    { NOTE_A4, 8 },
    { NOTE_GS3, 4, DOTTED },
    { NOTE_G3, 8 },
    //-------------------
    //measure 42
    { NOTE_AF4, 4 },
    { NOTE_BF4, 2 },   // half note
    { REST, 8 },       //Eighth rest
    { NOTE_C4, 16 },   //Sixteenth note
    { NOTE_BF4, 16 },  //Sixteenth note

    //-------------------
    //measure 43
    { NOTE_DF4, 4, DOTTED },  // dotted quarter
    { NOTE_C4, 8 },
    { NOTE_BF3, 4, DOTTED },
    { NOTE_A3, 8 },
    //-------------------
    //measure 44
    { NOTE_C4, 2 },   // half note
    { REST, 4 },      // quarter rest
    { REST, 8 },      // eighth rest
    { NOTE_EF4, 8 },  // eighth note



    //-------------------
    //measure 45
    { NOTE_DF4, 4, DOTTED },  // dotted quarter
    { NOTE_C4, 16 },          //Sixteenth note
    { NOTE_BF3, 16 },         //Sixteenth note
    { NOTE_C4, 16 },          // Sixteenth note
    { NOTE_DF4, 4 },          // quarter note
    { REST, 16 },             // Sixteenth rest
    { NOTE_EF4, 8 },          // Eighth note

    //-------------------
    //measure 46
    { NOTE_F4, 4 },           // quarter note
    { NOTE_EF4, 4, DOTTED },  // dotted quarter
    { NOTE_DF4, 4 },          // quarter note
    { NOTE_C4, 8 },           // Eighth note

    //-------------------
    //measure 47
    { NOTE_DF4, 4, DOTTED },  // dotted quarter
    { NOTE_C4, 8 },           // Eighth note
    { NOTE_BF4, 4, DOTTED },  // dotted quarter
    { NOTE_A3, 16 },          //Sixteenth note
    { NOTE_AF3, 16 },         // Sixteenth note

    //-------------------
    //measure 48
    { NOTE_C3, 1, VIBRATO_SLOW}  //whole note

};

const NoteStorage turret1Test[] = {
    // { NOTE_DF4, 1, VIBRATO_MED},  //whole note
    // { NOTE_C4, 1, VIBRATO_FAST},  //whole note
    // { NOTE_BF4, 1, VIBRATO_SLOW},  //whole note
    // { NOTE_A4, 1, GLISSANDO},  //whole note
    // { NOTE_B4, 1}  //whole note
};

const NoteStorage turret2Test[] = {
    { NOTE_D4, 4, VIBRATO_MED},  //quarter note
    { NOTE_G4, 2, VIBRATO_MED},  //half note
    { NOTE_FS4, 8, VIBRATO_MED},  //eighth note
    { NOTE_A4, 8, VIBRATO_MED},  //eighth note
    { NOTE_G4, 2, DOTTED | VIBRATO_MED},  //whole note
};


class Note {
public:
    uint16_t n;
    double len;
    bool staccato;
    bool vibrato;
    bool glissando;
    bool trill;

    Note()
    : n(0), len(0), staccato(false), vibrato(false), glissando(false), trill(true) {}

    Note(int no, double length, bool s = false, bool v = false, bool c = false, bool t = false)
    : n(no), len(length), staccato(s), vibrato(v), glissando(c), trill(t) {}
};



class Music {
public:
    int dirPin, stepPin;
    bool dir = false;

    bool staccatoMode = false;
    bool vibratoMode = false;
    bool glissandoMode = false;
    bool trillMode = false;
    //acceptable vibrato range falls under a quarter tone away from the note
    // a quarter tone has a ratio of 24 to sqrt 2 which is approx 1.0293 according to google
    //vibratoRange will be denoted as a percentage deviation above or below the base frequency

    float vibratoFrequency = 5;    //hz
    float staccatoRatio = 0.7;

    //current numbers
    unsigned long noteStartTime = 0;
    unsigned long nextStepTime = 0;
    unsigned long noteEndTime = 0;
    unsigned long noteDuration = 0;
    uint16_t frequency = 0;
    bool isPlaying = false;
    bool stepState = false;
    unsigned long lastVibratoUpdate;


    // Note* noteList;
    const NoteStorage* noteList;
    // int maxNotes;
    uint16_t noteCount;

    int currentIndex = 0;

    bool ended = false;

    bool graceNoteActive = false;
    double graceNoteDuration = 0;  // 0 by default so grace note can accumulate in the event of multiple at once

    uint16_t glissTarget = 0.0;
    double glissStep = 0.0;  //how much the frequency needs to increase each millisecond to reach the glissando target
    bool prevWasGliss = false;

    const float TRILL_FREQ_SEMITONE_RATIO = pow(2.0, 1.0 / 12.0);  // ~1.0595 (1 semitone)
    const float TRILL_ALT_INTERVAL = TRILL_FREQ_SEMITONE_RATIO;    // One semitone up
    const unsigned long TRILL_RATE = 20;                           // Trill alternates every 20 ms (50 Hz rate)

    bool trillState = false;  // toggles each trill pulse
    unsigned long nextTrillTime = 0;
    int trillBaseFreq = 0;


    Music(int d, int s, const NoteStorage* pgmList, int count)
    : dirPin(d), stepPin(s), noteList(pgmList), noteCount(count) {}



    void init() {
        pinMode(dirPin, OUTPUT);
        pinMode(stepPin, OUTPUT);
        digitalWrite(dirPin, LOW);
        digitalWrite(stepPin, LOW);
    }

    // void addNote(int n, double l, bool s=false){//s and v are false by default
    //   if (noteCount < maxNotes) {
    //     noteList[noteCount++] = Note(n, l, s);
    //   }else{
    //     Serial.println("[ERROR] exceeded max notes");
    //   }
    // }

    bool timeUp(double start, double durationUs) {  //returns true if the time is up
        return (micros() - start >= durationUs);
    }

    Note decodeNote(int i) {

        if (i >= noteCount || i < 0) {
            std::cout << "[ERROR] decodeNote index out of bounds, reading as quarter rest" << std::endl;
            //truncate i as a sanity check
            i = noteCount - 1;  //array indicies start from 0 bc of zero indexing
            //return a quarter rest as a placeholder for invalid values
            return Note(REST, 1.0 / 4.0, false, false);  // quarter rest
        }
        // memcpy_P(&ns, &noteList[i], sizeof(NoteStorage));  // read from PROGMEM
        NoteStorage ns = noteList[i];

        int freq = ns.n;

        // Grace note check
        if (ns.len >= 253) {  //grace note denoted by each GRACE_NOTE macro
            graceNoteActive = true;
            switch (ns.len) {
                case GRACE_NOTE_8TH:
                    graceNoteDuration += 1.0 / 8.0;
                    break;

                case GRACE_NOTE_16TH:
                    graceNoteDuration += 1.0 / 16.0;
                    break;

                case GRACE_NOTE_32ND:
                    graceNoteDuration += 1.0 / 32.0;
                    break;
            }
            //early return, grace note shouldent be affected by STACCATO and VIBRATO or any other flag anyway
            return Note(freq, graceNoteDuration);
        }



        double length = 1.0 / ns.len;  // bc its encoded as denominator
        if (ns.flags & DOTTED) {       //if note contains the dotted flag then it is 1.5 times longer
            length *= 1.5;
        }
        if (graceNoteActive) {  //grace note handler
            //if previous note was a grace note as indicated by flag
            //subtract grace note duration from current note to maintain measure timing
            length -= graceNoteDuration;
            if (length < 0) {
                //prevent negative numbers
                length = 0;
                std::cout << "[ERROR] Grace note(s) longer than following note that isn't a grace note :[" << std::endl;
            }
            graceNoteActive = false;  // uncheck gracenote flag

            graceNoteDuration = 0.0;  //reset grace note duration so it can accumulate again
            //in the event of multiple consecutive grace notes
        }

        bool staccato = ns.flags & STACCATO;
        bool vibrato = (ns.flags & VIBRATO_SLOW) || (ns.flags & VIBRATO_MED) || (ns.flags & VIBRATO_FAST);
        bool glissando = ns.flags & GLISSANDO;
        bool trill = ns.flags & TRILL;

        if (ns.flags & GLISSANDO) {
            //if glissando is called read next note to get the glissando target
            NoteStorage gns;
            if (i < (noteCount - 1)) {
                gns = noteList[i + 1];
                // read from PROGMEM the following note
            } else {
                gns = noteList[i];
                // read from PROGMEM the same note if somehow glis is called at the last note of the sequence
                std::cout << "[ERROR] glissando called with no following note, glissing to same final note" << std::endl;
            }
            //target note is the next note in the sequence
            glissTarget = gns.n;
        }

        //select vibrato rate
        if (ns.flags & VIBRATO_SLOW) {
            vibratoFrequency = VIB_SLOW_FREQ;
        } else if (ns.flags & VIBRATO_MED) {
            vibratoFrequency = VIB_MED_FREQ;
        } else if (ns.flags & VIBRATO_FAST) {
            vibratoFrequency = VIB_FAST_FREQ;
        }



        return Note(freq, length, staccato, vibrato, glissando, trill);
    }



    void restart() {
        currentIndex = 0;
        ended = false;
    }

    void update() {
        unsigned long nowMs = millis();
        unsigned long nowUs = micros();

        if (!isPlaying) {
            if (currentIndex < noteCount) {
                Note note = decodeNote(currentIndex++);
                staccatoMode = note.staccato;
                vibratoMode = note.vibrato;
                glissandoMode = note.glissando;
                trillMode = note.trill;
                // Save glissando status for the NEXT note
                bool suppressDirectionChange = prevWasGliss;
                prevWasGliss = glissandoMode;

                startNote(note.n, note.len, suppressDirectionChange);
            } else {
                ended = true;
            }
            return;
        }



        //playNote

        if (nowMs >= noteEndTime) {  //exit if past end time
            isPlaying = false;
            digitalWrite(stepPin, LOW);  // stop playing note
            return;
        }


        unsigned long stepDelay = 1000000L / frequency;


        if (glissandoMode) {
            float elapsed = nowMs - noteStartTime;
            float t = elapsed / noteDurationMs;  // progress [0.0 - 1.0]

            // Clamp t between 0 and 1
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            // Convert to pitch space (log2 of frequency)
            float pitchStart = log2f(frequency);
            float pitchEnd = log2f(glissTarget);


            //use linear interpolation for now, maybe need to change later
            // float glissFreq = (glissStep * elapsed) + frequency;  //y = mx + b form

            // Linear interpolation in pitch space
            float currentPitch = pitchStart + t * (pitchEnd - pitchStart);

            // Convert back to frequency space
            float glissFreq = powf(2.0f, currentPitch);

            //constrain with the min being freq or glissTarget depending on which is lower and similar for max
            float freqF = static_cast<float>(frequency);
            float targetF = static_cast<float>(glissTarget);
            glissFreq = std::min(std::max(glissFreq, std::min(freqF, targetF)),
                                 std::max(freqF, targetF));

            //failsafe for divide by 0
            if (glissFreq <= 0) glissFreq = 1;
            stepDelay = 1000000L / glissFreq;
        }

        else if (trillMode && !glissandoMode && frequency > 0) {
            if (nowMs >= nextTrillTime) {
                trillState = !trillState;  // flip trill state
                float trillFreq = trillBaseFreq * (trillState ? TRILL_ALT_INTERVAL : 1.0);
                if (trillFreq <= 0) trillFreq = 1;
                stepDelay = 1000000L / trillFreq;
                nextTrillTime = nowMs + TRILL_RATE;
            }
        }


        else if (vibratoMode && frequency > 0 && !glissandoMode) {  //glissando cant have vibrato(the following note can though)
            // float phaseShift = random(0, 6283) / 1000.0; // Random start

            float elapsed = (nowMs - noteStartTime) / 1000.0;
            //vibratoFrequency is the period
            //hertz is how many times in one second it cyclesa
            float B = 2 * 3.14159265358979323846264338 * (vibratoFrequency);
            //could add randomized phase shifts if vibrato sounds too samey
            float phaseShift = 0;
            //vibrato range is amplitude
            float vibratoOffset = vibratoRange * sin(B * (elapsed + phaseShift));
            float modulatedFreq = frequency * (1.0 + vibratoOffset);

            //failsafe for divide by 0
            // if (modulatedFreq <= 0) modulatedFreq = 1;
            if (modulatedFreq < 1.0) modulatedFreq = 1.0;
            // if (modulatedFreq > 10000.0) modulatedFreq = 10000.0;

            stepDelay = 1000000L / modulatedFreq;
            lastVibratoUpdate = nowUs;
            // Serial.println("VIBRATO MODE");
        }









        if (nowUs >= nextStepTime) {
            stepState = !stepState;  //flip step state
            if (nowMs >= (noteStartTime + noteDuration)) {
                digitalWrite(stepPin, LOW);  // stop playing note bc staccato ends early
                stepState = false;
                glissandoMode = false;
            } else {
                digitalWrite(stepPin, stepState);
            }

            nextStepTime = nowUs + stepDelay / 2;  //set next time
        }
    }

    void startNote(int freq, double type, bool suppressDirFlip = false) {
        unsigned long nowMs = millis();
        unsigned long nowUs = micros();
        frequency = freq;
        unsigned long fullDuration = type * WholeNoteMS;
        //glissando cannot be used with staccato
        noteDuration = fullDuration;
        if (staccatoMode && !(glissandoMode)) {
            noteDuration *= staccatoRatio;
        }
        // Serial.println(staccatoMode);
        noteStartTime = nowMs;
        noteEndTime = nowMs + fullDuration;

        if (frequency == 0) {
            //REST
            isPlaying = true;
            return;  //exit to not run rest of stepper code
        }

        if (glissandoMode) {
            //calculate the frequency step each millisecond needs to change to reach glissando target

            if (fullDuration > 0) {
                glissStep = (glissTarget - frequency) / (double)fullDuration;
            } else {
                glissStep = 0;
            }
        }

        if (trillMode) {
            trillBaseFreq = frequency;
            trillState = false;
            nextTrillTime = nowMs + TRILL_RATE;
        }

        unsigned long stepDelay = 1000000L / frequency;
        nextStepTime = nowUs + stepDelay / 2;
        stepState = false;
        isPlaying = true;

        if (!suppressDirFlip) {
            dir = !dir;  // flip direction only if not glissing from previous note
        }
          digitalWrite(dirPin, dir ? HIGH : LOW);
        //digitalWrite(dirPin, HIGH);
    }
};

//BPM80 for turret serenade
// const NoteStorage* stepper1CurrSong = turret1Melody;
// unsigned int stepper1CurrLength = sizeof(turret1Melody) / sizeof(NoteStorage);
// const NoteStorage* stepper2CurrSong = turret2Melody;
// unsigned int stepper2CurrLength = sizeof(turret2Melody) / sizeof(NoteStorage);


//BPM58 for turret serenade
const NoteStorage* stepper1CurrSong = turret1Test;
unsigned int stepper1CurrLength = sizeof(turret1Test) / sizeof(NoteStorage);

const NoteStorage* stepper2CurrSong = turret2Test;
unsigned int stepper2CurrLength = sizeof(turret2Test) / sizeof(NoteStorage);



Music stepper1(dirPin1, stepPin1, stepper1CurrSong, stepper1CurrLength);  //melody size is calculated
//using the total size of the melody object
//divided by the size of each of the notestorage structs
Music stepper2(dirPin2, stepPin2, stepper2CurrSong, stepper2CurrLength);



void setup() {
    songBPM = 58;
    WholeNoteMS = 4 * BPMtoMilisec / songBPM;
    stepper1.init();
    stepper2.init();
    // stepper3.init();
    usleep(2000 * 1000);  // 2 seconds
    std::cout << "Init" << std::endl;
}

void loop() {
    //for turret srenade  5 flats:  B♭, E♭, A♭, D♭, and G♭.
    stepper1.update();
    stepper2.update();

    if (stepper1.ended && stepper2.ended) {
        std::cout << "Song ended – pausing before restart..." << std::endl;
        usleep(10000 * 1000);

        stepper1.restart();
        stepper2.restart();
        std::cout << "Restarting" << std::endl;
    }
}

//cpp main

int main(){
    wiringPiSetupGpio();
    setup();

    while(true){
        loop();
    }

    return 0;

}

