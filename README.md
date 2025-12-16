# Sane Music Player
A simple music player written in C with Raylib, Taglib and MiniAudio.

## Usage
Clone the repository and build the project with 'make clean all'. Make sure Raylib and Taglib are installed. If it still doesn't compile, try changing the dependency paths in the Makefile.
This is an early version of the application so songs in the playlist are hardcoded, change them in the app_init function in app.c. Arrow keys can be used to lower or increase volume and to play the next or previous song in the playlist.
