#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "llist.h"


LLIST* load_albums_from_db(LLIST* llist, sqlite3* db) {
    const char* sql = "SELECT id, name, artist, year, n_songs FROM album;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to prepare SELECT: %s\n", sqlite3_errmsg(db));
        return llist;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* id = (const char*)sqlite3_column_text(stmt, 0);

        ALBUM a;
        strcpy(a.name, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(a.artist, (const char*)sqlite3_column_text(stmt, 2));
        a.year = sqlite3_column_int(stmt, 3);
        a.n_songs = sqlite3_column_int(stmt, 4);

        // 메모리에만 추가 (DB 저장 안 함)
        NODE* new_node = (NODE*)malloc(sizeof(NODE));
        strcpy(new_node->fileName, id);
        new_node->p_album = (ALBUM*)malloc(sizeof(ALBUM));
        memcpy(new_node->p_album, &a, sizeof(ALBUM));

        new_node->next = NULL;
        new_node->pre = llist->tail;

        if (llist->tail)
            ((NODE*)llist->tail)->next = new_node;
        else
            llist->head = new_node;

        llist->tail = new_node;
        llist->num++;
    }

    sqlite3_finalize(stmt);
    return llist;
}

SONG* find_title_db(sqlite3* db, const char* title) {
    const char* sql = "SELECT title, artist, album_id FROM song WHERE title = ?;";
    sqlite3_stmt* stmt;
    int rc;

    // SQL 준비
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] prepare failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // ?에 title_query 바인딩
    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);


    SONG* result;

    // 쿼리 실행
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = (SONG*)malloc(sizeof(SONG));
        strcpy(result->title, (const char*)sqlite3_column_text(stmt, 0));
        strcpy(result->artist, (const char*)sqlite3_column_text(stmt, 1));
        printf("노래 제목: %s\n", sqlite3_column_text(stmt, 0));
        printf("노래 아티스트: %s\n", sqlite3_column_text(stmt, 1));
    } 
    else {
        printf("'%s' 제목의 노래를 찾을 수 없습니다.\n", title);
        free(result);
        result = NULL;


    sqlite3_finalize(stmt);
    return result;
    }
}


