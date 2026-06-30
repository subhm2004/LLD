// models/Song.h — Ek gaane ka data: title, artist, aur audio data/path. System
// ki sabse basic unit jise playlist hold karti aur engine play karta hai.
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