#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

typedef struct {
    int             songId;
    int             albumID;
    char artist[20];
    char title[50];
} SONG;

typedef struct {
    int             albumID;
    char            name[40];
    char            artist[20];
    unsigned int    year;
    unsigned char   n_songs;
   SONG            s_list[20]; 
} ALBUM;

typedef struct member{
    int memberId;
    char name[100];
    int age;
    int grade;
    int n_playlists;
}MEMBER;


//앨범 관련 함수
ALBUM* get_album(sqlite3* db, int albumID); //검색 a
void add_album(sqlite3* db, ALBUM* album); //추가 b
void delete_album(sqlite3* db, int albumID); //삭제 c

//노래 관련 함수
SONG* get_song(sqlite3* db, int songId); //검색 d
void add_song(sqlite3* db, int albumID, SONG* song); //추가 e
void delete_song(sqlite3* db, int songId); //삭제 f

//회원 관련 함수
MEMBER* get_member(sqlite3* db, int memberId); //검색 g
void add_member(sqlite3* db, MEMBER* member); //추가 h
void delete_member(sqlite3* db, int memberId); //삭제 i

//구매 관련 함수
void buy_song(sqlite3* db, int memberId, int songId); //o 순서 섞임 조심 
void buy_album(sqlite3* db, int memberId, int albumId); //j

//플레이리스트 관련 함수
void create_playlist(sqlite3* db, int memberId, const char* playlistName); //k
void get_playlist(sqlite3* db, int playlistId); //l
void add_song_to_playlist(sqlite3* db, int playlistId, int albumId, int songId); //m
void delete_playlist(sqlite3* db, int playlistId); //n











