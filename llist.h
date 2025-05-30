#ifndef LLIST_H
#define LLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define MAX_SONGS 20

typedef struct {
    char artist[20];
    char title[50];
} SONG;

typedef struct {
    char            name[40];
    char            artist[20];
    unsigned int    year;
    unsigned char   n_songs;
   SONG            s_list[20]; 
} ALBUM;

typedef struct node{
    char fileName[20];
    ALBUM* p_album;
    struct node* pre;
    struct node* next; 
}NODE;

typedef struct llist{
    int num;
    void* head;  //void 여서 재정의 가능
    void* tail;
}LLIST;

typedef struct buy_album_node{
    ALBUM* album;
    struct buy_album_node* pre;
    struct buy_album_node* next;
}BUYALBUMNODE;

//송노드를 연결해서 플레이리스트 만들자.
typedef struct songnode{ 
    SONG* song;
    char album_name[50];
    struct songnode* pre;
    struct songnode* next;
}SONGNODE;

typedef struct {
    char name[40];
    LLIST song;
} PLAYLIST;

typedef struct member{
    char name[100];
    int age;
    int grade;
    LLIST buy_album;
    int n_playlists;
    PLAYLIST playlist[100]; //연결리스트가 100개
}MEMBER;

typedef struct membernode{
    MEMBER* member;
    struct membernode *pre;
    struct membernode *next;
}MEMBERNODE;

NODE* search_file(LLIST* llist, char* filename);
LLIST* insert_file(LLIST* llist, char* s_fileName, char* a_fileName, ALBUM* content);
LLIST* delete_file(LLIST* llist, char* d_fileName);
LLIST* modify_file(LLIST* llist, char* m_fileName, char* content);
LLIST* insert_song(LLIST* llist, char* album, char* index, char* title, char* artist);
SONG* find_title(LLIST* llist, char* title);
LLIST* find_artist(LLIST* llist, char* artist);
void print_all(LLIST* llist);

MEMBERNODE* search_member(LLIST* mllist,char* name);
LLIST* insert_member(LLIST* mllist,char* search_member,MEMBER* content);
LLIST* buy_album(LLIST* mllist, LLIST* llist, char* member_name, char* album_name);
void print_member(LLIST* mllist);

ALBUM* search_album(LLIST* llist, char* album_name);
LLIST* insert_playlist(LLIST* mllist, LLIST* llist, 
    char* member_name, char* playlist_name, char* album_name, char* song_name);

#endif
