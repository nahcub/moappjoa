#pragma once
#include "database.h"
#define SONG_TABLE_SIZE 1000

typedef struct song_node{
    SONG* song;
    struct song_node* next;
}SONG_NODE;


extern SONG_NODE* song_table[SONG_TABLE_SIZE];

int hash_song(int songId);
void add_song_hash(SONG* song);
SONG* get_song_hash(int songId);
void clear_song_hash();

void load_song_hash(sqlite3* db);