#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"

SONG_NODE* song_table[SONG_TABLE_SIZE] = {0};

int hash_song(int songId){
    return songId % SONG_TABLE_SIZE;
}

void add_song_hash(SONG* song){
    int hash_index = hash_song(song->songId);
    SONG_NODE* new_node = (SONG_NODE*)malloc(sizeof(SONG_NODE));
    new_node->song = song;
    new_node->next = song_table[hash_index];
    song_table[hash_index] = new_node;
    printf("[add_song_hash] %s by %s to hash table\n", song->title, song->artist);
}

SONG* get_song_hash(int songId){
    int hash_index = hash_song(songId);
    SONG_NODE* current = song_table[hash_index];
    //chaining 방식으로 구현
    while(current){
        if(current->song->songId == songId){
            printf("[get_song_hash] %s by %s found in hash table\n", current->song->title, current->song->artist);
            return current->song;
        }
        current = current->next;
    }
    printf("[get_song_hash] %d not found in hash table\n", songId);
    return NULL;
}

void clear_song_hash(){
    for(int i = 0; i < SONG_TABLE_SIZE; i++){
        SONG_NODE* current = song_table[i];
        while(current){
            SONG_NODE* temp = current;
            current = current->next;
            free(temp);
        }
        song_table[i] = NULL;
    }
}

void load_song_hash(sqlite3* db){
    char* sql = "SELECT id FROM song";
    sqlite3_stmt* stmt;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK){
        fprintf(stderr, "[SQLite ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    while(sqlite3_step(stmt) == SQLITE_ROW){
        int songId = sqlite3_column_int(stmt, 0);
        SONG* song = get_song(db, songId);
        if(song){
            add_song_hash(song);
        }
    }

    sqlite3_finalize(stmt);
    printf("[load_song_hash] 해시 테이블에 로드되었습니다.\n");
}