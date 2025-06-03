#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include "database.h"

ALBUM* get_album(sqlite3* db, int albumID){
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name, artist, year, n_songs FROM album WHERE id = ?";
    ALBUM* album = NULL;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[get_album] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Bind the albumID parameter
    sqlite3_bind_int(stmt, 1, albumID);

    // Execute the query
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Create new album structure
        album = (ALBUM*)malloc(sizeof(ALBUM));
        if (album == NULL) {
            fprintf(stderr, "[get_album] Failed to allocate memory for album\n");
            sqlite3_finalize(stmt);
            return NULL;
        }

        // Fill the album structure with data from database
        album->albumID = sqlite3_column_int(stmt, 0);
        strncpy(album->name, (const char*)sqlite3_column_text(stmt, 1), sizeof(album->name) - 1);
        album->name[sizeof(album->name) - 1] = '\0';
        strncpy(album->artist, (const char*)sqlite3_column_text(stmt, 2), sizeof(album->artist) - 1);
        album->artist[sizeof(album->artist) - 1] = '\0';
        album->year = sqlite3_column_int(stmt, 3);
        album->n_songs = sqlite3_column_int(stmt, 4);

        // Initialize song list
        memset(album->s_list, 0, sizeof(album->s_list));

        // Print album title
        printf("[get_album] 앨범 제목: %s\n", album->name);
    } else {
        printf("[get_album] 앨범을 찾을 수 없습니다. (ID: %d)\n", albumID);
    }

    sqlite3_finalize(stmt);
    return album;
}

void add_album(sqlite3* db, ALBUM* album){
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO album (name, artist, year, n_songs) VALUES (?, ?, ?, ?)";
    char* err_msg = 0;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[add_album] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the values to the prepared statement
    sqlite3_bind_text(stmt, 1, album->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, album->artist, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, album->year);
    sqlite3_bind_int(stmt, 4, album->n_songs);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[add_album] Failed to insert album: %s\n", sqlite3_errmsg(db));
    } else {
        // Get the last inserted row id
        album->albumID = sqlite3_last_insert_rowid(db);
        printf("[add_album] 앨범이 성공적으로 추가되었습니다. (ID: %d)\n", album->albumID);
    }

    sqlite3_finalize(stmt);
}

void delete_album(sqlite3* db, int albumID){
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM album WHERE id = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[delete_album] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the albumID parameter
    sqlite3_bind_int(stmt, 1, albumID);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[delete_album] Failed to delete album: %s\n", sqlite3_errmsg(db));
    } else {
        // sqlite3_changes()를 사용하여 실제로 삭제된 행이 있는지 확인
        if (sqlite3_changes(db) > 0) {
            printf("[delete_album] 앨범이 성공적으로 삭제되었습니다. (ID: %d)\n", albumID);
        } else {
            printf("[delete_album] 삭제할 앨범이 존재하지 않습니다. (ID: %d)\n", albumID);
        }
    }

    sqlite3_finalize(stmt);
}

SONG* get_song(sqlite3* db, int songId){
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, album_id, title, artist FROM song WHERE id = ?";
    SONG* song = NULL;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[get_song] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Bind only the songId parameter
    sqlite3_bind_int(stmt, 1, songId);

    // Execute the query
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Create new song structure
        song = (SONG*)malloc(sizeof(SONG));
        if (song == NULL) {
            fprintf(stderr, "[get_song] Failed to allocate memory for song\n");
            sqlite3_finalize(stmt);
            return NULL;
        }

        // Fill the song structure with data from database
        song->songId = songId;
        song->albumID = sqlite3_column_int(stmt, 1);
        strncpy(song->title, (const char*)sqlite3_column_text(stmt, 2), sizeof(song->title) - 1);
        song->title[sizeof(song->title) - 1] = '\0';
        strncpy(song->artist, (const char*)sqlite3_column_text(stmt, 3), sizeof(song->artist) - 1);
        song->artist[sizeof(song->artist) - 1] = '\0';

        printf("[get_song] 노래 제목: %s (앨범 ID: %d)\n", song->title, song->albumID);
    } else {
        printf("[get_song] 노래를 찾을 수 없습니다. (노래 ID: %d)\n", songId);
    }

    sqlite3_finalize(stmt);
    return song;
}

void add_song(sqlite3* db, int albumID, SONG* song){
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO song (album_id, title, artist) VALUES (?, ?, ?)";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[add_song] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the values to the prepared statement
    sqlite3_bind_int(stmt, 1, albumID);
    sqlite3_bind_text(stmt, 2, song->title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, song->artist, -1, SQLITE_STATIC);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[add_song] Failed to insert song: %s\n", sqlite3_errmsg(db));
    } else {
        // Get the last inserted row id and update song structure
        song->songId = sqlite3_last_insert_rowid(db);
        song->albumID = albumID;
        
        // Update album's n_songs count
        const char* update_sql = "UPDATE album SET n_songs = n_songs + 1 WHERE id = ?";
        sqlite3_stmt* update_stmt;
        if (sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_int(update_stmt, 1, albumID);
            if (sqlite3_step(update_stmt) != SQLITE_DONE) {
                fprintf(stderr, "[add_song] Failed to update album song count: %s\n", sqlite3_errmsg(db));
            }
            sqlite3_finalize(update_stmt);
        }

        printf("[add_song] 노래가 성공적으로 추가되었습니다. (노래 ID: %d, 앨범 ID: %d)\n", 
               song->songId, albumID);
    }

    sqlite3_finalize(stmt);
}

void delete_song(sqlite3* db, int songId){
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM song WHERE id = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[delete_song] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the songId parameter
    sqlite3_bind_int(stmt, 1, songId);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[delete_song] Failed to delete song: %s\n", sqlite3_errmsg(db));
    } else {
        // sqlite3_changes()를 사용하여 실제로 삭제된 행이 있는지 확인
        if (sqlite3_changes(db) > 0) {
            printf("[delete_song] 노래가 성공적으로 삭제되었습니다. (노래 ID: %d)\n", songId);
        } else {
            printf("[delete_song] 삭제할 노래가 존재하지 않습니다. (노래 ID: %d)\n", songId);
        }
    }

    sqlite3_finalize(stmt);
}

MEMBER* get_member(sqlite3* db, int memberId) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, name, age, grade FROM member WHERE id = ?";
    MEMBER* member = NULL;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[get_member] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    // Bind the memberId parameter
    sqlite3_bind_int(stmt, 1, memberId);

    // Execute the query
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Create new member structure
        member = (MEMBER*)malloc(sizeof(MEMBER));
        if (member == NULL) {
            fprintf(stderr, "[get_member] Failed to allocate memory for member\n");
            sqlite3_finalize(stmt);
            return NULL;
        }

        // Fill the member structure with data from database
        member->memberId = sqlite3_column_int(stmt, 0);
        strncpy(member->name, (const char*)sqlite3_column_text(stmt, 1), sizeof(member->name) - 1);
        member->name[sizeof(member->name) - 1] = '\0';
        member->age = sqlite3_column_int(stmt, 2);
        member->grade = sqlite3_column_int(stmt, 3);

        printf("[get_member] 회원 이름: %s (ID: %d)\n", member->name, member->memberId);
    } else {
        printf("[get_member] 회원을 찾을 수 없습니다. (ID: %d)\n", memberId);
    }

    sqlite3_finalize(stmt);
    return member;
}

void add_member(sqlite3* db, MEMBER* member) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO member (name, age, grade) VALUES (?, ?, ?)";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[add_member] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the values to the prepared statement
    sqlite3_bind_text(stmt, 1, member->name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, member->age);
    sqlite3_bind_int(stmt, 3, member->grade);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[add_member] Failed to insert member: %s\n", sqlite3_errmsg(db));
    } else {
        // Get the last inserted row id
        member->memberId = sqlite3_last_insert_rowid(db);
        printf("[add_member] 회원이 성공적으로 추가되었습니다. (ID: %d)\n", member->memberId);
    }

    sqlite3_finalize(stmt);
}

void delete_member(sqlite3* db, int memberId) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM member WHERE id = ?";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[delete_member] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the memberId parameter
    sqlite3_bind_int(stmt, 1, memberId);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[delete_member] Failed to delete member: %s\n", sqlite3_errmsg(db));
    } else {
        // sqlite3_changes()를 사용하여 실제로 삭제된 행이 있는지 확인
        if (sqlite3_changes(db) > 0) {
            printf("[delete_member] 회원이 성공적으로 삭제되었습니다. (ID: %d)\n", memberId);
        } else {
            printf("[delete_member] 삭제할 회원이 존재하지 않습니다. (ID: %d)\n", memberId);
        }
    }

    sqlite3_finalize(stmt);
}

void buy_album(sqlite3* db, int memberId, int albumId) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO member_album (member_id, album_id) VALUES (?, ?)";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[buy_album] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the parameters
    sqlite3_bind_int(stmt, 1, memberId);
    sqlite3_bind_int(stmt, 2, albumId);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[buy_album] Failed to buy album: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // 앨범의 모든 노래를 member_song 테이블에 추가
    const char* song_sql = "INSERT INTO member_song (member_id, song_id) "
                          "SELECT ?, id FROM song WHERE album_id = ?";
    if (sqlite3_prepare_v2(db, song_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[buy_album] Failed to prepare song statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the parameters
    sqlite3_bind_int(stmt, 1, memberId);
    sqlite3_bind_int(stmt, 2, albumId);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[buy_album] Failed to add songs to member_song: %s\n", sqlite3_errmsg(db));
    } else {
        printf("[buy_album] 앨범 구매가 완료되었습니다. (회원 ID: %d, 앨범 ID: %d)\n", 
               memberId, albumId);
    }

    sqlite3_finalize(stmt);
}

void buy_song(sqlite3* db, int memberId, int songId){
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO member_song (member_id, song_id) VALUES (?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[buy_song] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    //bind parameters
    sqlite3_bind_int(stmt, 1, memberId);
    sqlite3_bind_int(stmt, 2, songId);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[buy_song] Failed to buy song: %s\n", sqlite3_errmsg(db));
    } else {
        printf("[buy_song] 음악 구매가 완료되었습니다. (회원 ID: %d, 음악 ID: %d)\n", 
               memberId, songId);
    }

    sqlite3_finalize(stmt);

}

void create_playlist(sqlite3* db, int memberId, const char* playlistName) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO playlist (member_id, name) VALUES (?, ?)";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[create_playlist] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the parameters
    sqlite3_bind_int(stmt, 1, memberId);
    sqlite3_bind_text(stmt, 2, playlistName, -1, SQLITE_STATIC);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[create_playlist] Failed to create playlist: %s\n", sqlite3_errmsg(db));
    } else {
        // Get the last inserted row id
        int playlistId = sqlite3_last_insert_rowid(db);
        printf("[create_playlist] 플레이리스트가 생성되었습니다. (ID: %d, 이름: %s)\n", 
               playlistId, playlistName);
    }

    sqlite3_finalize(stmt);
}

void add_song_to_playlist(sqlite3* db, int playlistId, int albumId, int songId) {
    sqlite3_stmt* stmt;
    
    // 먼저 노래가 존재하는지 확인
    const char* check_sql = "SELECT id FROM song WHERE id = ? AND album_id = ?";
    if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[add_song_to_playlist] Failed to prepare check statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, songId);
    sqlite3_bind_int(stmt, 2, albumId);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "[add_song_to_playlist] 노래를 찾을 수 없습니다. (노래 ID: %d, 앨범 ID: %d)\n", songId, albumId);
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // 플레이리스트에 노래 추가
    const char* sql = "INSERT INTO playlist_song (playlist_id, album_id, song_id) VALUES (?, ?, ?)";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "[add_song_to_playlist] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the parameters
    sqlite3_bind_int(stmt, 1, playlistId);
    sqlite3_bind_int(stmt, 2, albumId);
    sqlite3_bind_int(stmt, 3, songId);

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "[add_song_to_playlist] Failed to add song to playlist: %s\n", sqlite3_errmsg(db));
    } else {
        printf("[add_song_to_playlist] 노래가 플레이리스트에 추가되었습니다. (플레이리스트 ID: %d, 앨범 ID: %d, 노래 ID: %d)\n", 
               playlistId, albumId, songId);
    }

    sqlite3_finalize(stmt);
}

void delete_playlist(sqlite3* db, int playlistId){
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM playlist WHERE id = ?";
    if(sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK){
        fprintf(stderr, "[delete_playlist] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, playlistId);
    if(sqlite3_step(stmt) != SQLITE_DONE){
        fprintf(stderr, "[delete_playlist] Failed to delete playlist: %s\n", sqlite3_errmsg(db));
    }else{
        printf("[delete_playlist] 플레이리스트가 성공적으로 삭제되었습니다. (ID: %d)\n", playlistId);
    }


    sqlite3_finalize(stmt);

}
void get_playlist(sqlite3* db, int playlistId){
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id, member_id, name FROM playlist WHERE id = ?";
    if(sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)!= SQLITE_OK){
        fprintf(stderr, "[get_playlist] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(stmt, 1, playlistId);
    int rc = sqlite3_step(stmt); //행을 가져온다 

    if(rc == SQLITE_ROW){
        int member_id = sqlite3_column_int(stmt, 1); //그 행의 열을 읽는다 
        const char* name = sqlite3_column_text(stmt, 2);
        printf("[get_playlist] 플레이리스트 정보: ID = %d, 회원 ID = %d, 이름 = %s\n", playlistId, member_id, name);
    }else{
        printf("[get_playlist] 플레이리스트를 찾을 수 없습니다. (ID: %d)\n", playlistId);
    }
    sqlite3_finalize(stmt);
    }