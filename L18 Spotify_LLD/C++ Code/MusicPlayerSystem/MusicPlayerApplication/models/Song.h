// ============================================================================
//  models/Song.h — Ek gaane ka data (title, artist, file path)
// ----------------------------------------------------------------------------
//  System ki sabse basic unit — playlist inhi ka collection hai, engine
//  inhe play karta hai, strategy inhi ke order pe kaam karti hai. Simple
//  data holder with getters. filePath asli me audio file ka location hota
//  (demo me sirf string).
// ============================================================================
#ifndef SONG_HPP
#define SONG_HPP
#include <string>
#include <iostream>

using namespace std;

class Song {
private:
    string title;
    string artist;
    string filePath;
public:
    Song(string t, string a, string f) {
        title = t;
        artist = a;
        filePath = f;
    }
    string getTitle() { 
        return title; 
    }
    string getArtist() { 
        return artist;      
    }
    string getFilePath() { 
        return filePath;  
    }
};
#endif // SONG_HPP