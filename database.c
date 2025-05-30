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

