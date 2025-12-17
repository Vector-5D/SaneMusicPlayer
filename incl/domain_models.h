#pragma once

#include <stddef.h>
#include <stdbool.h>

// ========================================================================
// TRACKS
// ========================================================================

typedef struct track {
    char* path; // identifier
    char* title;
    char* artist;
    char* album;
    char* genre;
    int duration; // in seconds
    int year;
    int track_number;
} track_t;

typedef struct track_list {
    track_t* items;
    size_t count;
    size_t capacity;
} track_list_t;

// track functions
// ---------------
// creates a track with only a path
track_t* track_create(const char* path);
// frees a track
void track_free(track_t* track);
// returns a copy of the given track
track_t* track_copy(const track_t* track);

// track list functions
// --------------------
// creates an empty list of tracks
track_list_t* track_list_create();
// frees a list of tracks
void track_list_free(track_list_t* list);
// appends a track to a list of tracks
bool track_list_append(track_list_t* list, track_t* track);
// removes the track at the given index and shifts the rest down
bool track_list_remove(track_list_t* list, size_t index);
// removes the track with the given path if it exists
bool track_list_remove_by_path(track_list_t* list, const char* path);
// clears the list of tracks (this does not free it)
bool track_list_clear(track_list_t* list);
// gets the track at the provided index in a list of tracks
track_t* track_list_get(track_list_t* list, size_t index);
// gets the track with the provided path if it exists
track_t* track_list_find_by_path(track_list_t* list, const char* path);
// checks if a list of tracks contains a track
bool track_list_contains(track_list_t* list, const track_t* track);
// gives back the index of a track if the track exists in the list
size_t track_list_index_of(track_list_t* list, const track_t* track);

// ========================================================================
// ALBUMS
// ========================================================================

// albums store pointers to tracks in the central track_list
// they don't own the track data only reference it
typedef struct album {
    char* title; // NOT unique
    track_t** tracks; // array of pointers to tracks in central list
    size_t track_count;
    size_t track_capacity;
} album_t;

typedef struct album_list {
    album_t* items;
    size_t count;
    size_t capacity;
} album_list_t;

// album functions
// ---------------
// creates an album with only a title
album_t* album_create(const char* title);
// frees an album (does NOT free the tracks, only the pointer array)
void album_free(album_t* album);
// adds a pointer to a track in the central list
bool album_add_track(album_t* album, track_t* track);
// removes a track pointer from an album
bool album_remove_track(album_t* album, track_t* track);
// checks if an album contains a pointer to this track
bool album_has_track(album_t* album, const track_t* track);
// gets the number of tracks in an album
size_t album_get_track_count(const album_t* album);
// gets the total duration of all tracks in an album
int album_get_total_duration(const album_t* album);
// gets a track pointer at the given index
track_t* album_get_track(const album_t* album, size_t index);

// album list functions
// --------------------
// creates an empty list of albums
album_list_t* album_list_create();
// frees a list of albums
void album_list_free(album_list_t* list);
// appends an album to a list of albums
bool album_list_append(album_list_t* list, album_t* album);
// removes the album at the given index and shifts the rest down
bool album_list_remove(album_list_t* list, size_t index);
// removes the album with the given title if it exists
bool album_list_remove_by_title(album_list_t* list, const char* title);
// clears the list of albums (this does not free it)
bool album_list_clear(album_list_t* list);
// gets the album at the provided index in a list of albums
album_t* album_list_get(album_list_t* list, size_t index);
// gets the album with the provided title if it exists
album_t* album_list_find_by_title(album_list_t* list, const char* title);
// checks if a list of albums contains an album
bool album_list_contains(album_list_t* list, const album_t* album);

// ========================================================================
// GENRES
// ========================================================================

// genres store pointers to albums in the central album_list
// they don't own the album data, only reference it
typedef struct genre {
    char* name; // unique but shouldn't be treated as such functionally
    album_t** albums; // array of pointers to albums in central list
    size_t album_count;
    size_t album_capacity;
} genre_t;

typedef struct genre_list {
    genre_t* items;
    size_t count;
    size_t capacity;
} genre_list_t;

// genre functions
// ---------------
// creates a genre with only a name
genre_t* genre_create(const char* name);
// frees a genre (does NOT free the albums, only the pointer array)
void genre_free(genre_t* genre);
// adds a pointer to an album in the central list
bool genre_add_album(genre_t* genre, album_t* album);
// removes an album pointer from a genre
bool genre_remove_album(genre_t* genre, album_t* album);
// checks if a genre contains a pointer to this album
bool genre_has_album(genre_t* genre, const album_t* album);
// gets the number of albums in a genre
size_t genre_get_album_count(const genre_t* genre);
// gets an album pointer at the given index
album_t* genre_get_album(const genre_t* genre, size_t index);

// genre list functions
// --------------------
// creates an empty list of genres
genre_list_t* genre_list_create();
// frees a list of genres
void genre_list_free(genre_list_t* list);
// appends a genre to a list of genres
bool genre_list_append(genre_list_t* list, genre_t* genre);
// removes the genre at the given index and shifts the rest down
bool genre_list_remove(genre_list_t* list, size_t index);
// removes the genre with the given name if it exists
bool genre_list_remove_by_name(genre_list_t* list, const char* name);
// clears the list of genres (this does not free it)
bool genre_list_clear(genre_list_t* list);
// gets the genre at the provided index in a list of genres
genre_t* genre_list_get(genre_list_t* list, size_t index);
// gets the genre with the provided name if it exists
genre_t* genre_list_find_by_name(genre_list_t* list, const char* name);
// checks if a list of genres contains a genre
bool genre_list_contains(genre_list_t* list, const genre_t* genre);

// ========================================================================
// ARTISTS
// ========================================================================

// artists store pointers to albums in the central album_list
// they don't own the album data only reference it
typedef struct artist {
    char* name; // NOT unique
    album_t** albums; // array of pointers to albums in central list
    size_t album_count;
    size_t album_capacity;
} artist_t;

typedef struct artist_list {
    artist_t* items;
    size_t count;
    size_t capacity;
} artist_list_t;

// artist functions
// ----------------
// creates an artist with only a name
artist_t* artist_create(const char* name);
// frees an artist (does NOT free the albums, only the pointer array)
void artist_free(artist_t* artist);
// adds a pointer to an album in the central list
bool artist_add_album(artist_t* artist, album_t* album);
// removes an album pointer from an artist
bool artist_remove_album(artist_t* artist, album_t* album);
// checks if an artist has a pointer to this album
bool artist_has_album(artist_t* artist, const album_t* album);
// gets the number of albums by an artist
size_t artist_get_album_count(const artist_t* artist);
// gets an album pointer at the given index
album_t* artist_get_album(const artist_t* artist, size_t index);

// artist list functions
// ---------------------
// creates an empty list of artists
artist_list_t* artist_list_create();
// frees a list of artists
void artist_list_free(artist_list_t* list);
// appends an artist to a list of artists
bool artist_list_append(artist_list_t* list, artist_t* artist);
// removes the artist at the given index and shifts the rest down
bool artist_list_remove(artist_list_t* list, size_t index);
// removes the artist with the given name if it exists
bool artist_list_remove_by_name(artist_list_t* list, const char* name);
// clears the list of artists (this does not free it)
bool artist_list_clear(artist_list_t* list);
// gets the artist at the provided index in a list of artists
artist_t* artist_list_get(artist_list_t* list, size_t index);
// gets the artist with the provided name if it exists
artist_t* artist_list_find_by_name(artist_list_t* list, const char* name);
// checks if a list of artists contains an artist
bool artist_list_contains(artist_list_t* list, const artist_t* artist);
