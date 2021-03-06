#include <syntacts>
#include <chrono>
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace tact;

// This example is meant to show the full process of creating cues, sequencing 
// them in time, and finally playing it. We're not saying that Syntacts is meant
// to create music, but this is an illustration that can be played over speakers

/// Returns musical note n semitones away from f0 (440 Hz = A4 by default)
template <typename Osc>
Signal note(int n, float f0 = 440) {
    static float a  = std::pow(2.0f, 1.0f/12.0f);
    float f = f0 * std::pow(a,n);
    return Osc(f) * ASR(0.05f, 0.10f, 0.1f);
}

/// Returns musical note corresponding to name and octave
template <typename Osc>
Signal key(const std::string& name, int octave) {
    static std::vector<std::string> names = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    int idx = (int)std::distance(names.begin(), std::find(names.begin(), names.end(), name));
    int i = idx - 9 + (octave - 4) * 12;
    return note<Osc>(i, 440);
}

// Cretes the notes required for "Funkytown" and returns the notes sequenced in time 
Signal funktown() {
    auto Dsharp = key<Saw>("D#",6);
    auto Csharp = key<Saw>("C#",6);
    auto Asharp = key<Saw>("A#",5);
    auto Gsharp = key<Saw>("G#",6);
    auto G      = key<Saw>("G", 6);  
    return Dsharp << Dsharp << Csharp << Dsharp  << 0.2f << Asharp  << 0.2f << Asharp << Dsharp << Gsharp << G << Dsharp;
}

// Cretes the notes required for "Dixie" and returns the notes sequenced in time 
Signal dixie() {
    return key<Square>("B", 5) <<
           key<Square>("G#",5) <<
           key<Square>("E", 5) <<
           key<Square>("E", 5) <<
           key<Square>("E", 5) <<
           key<Square>("F#",5) <<
           key<Square>("G#",5) <<
           key<Square>("A", 5) <<
           key<Square>("B", 5) <<
           key<Square>("B", 5) <<
           key<Square>("B", 5) <<
           key<Square>("G#",5);
}

int main(int argc, char const *argv[])
{
    Session session;
    session.open();    
    auto f = funktown(); 
    Library::saveSignal(f, "funky");
    session.playAll(f);
    sleep(f.length() + 0.25f);
    auto d = dixie();
    session.playAll(d);
    sleep(d.length() + 0.25f);
    return 0;
}
