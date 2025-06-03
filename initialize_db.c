#include "initialize_db.h"
#include <stdio.h>
#include <sqlite3.h>

int initialize_database(sqlite3* db) {
    char* err_msg = 0;
    const char* sql;

    // Enable foreign key constraints
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
    printf("[SQLite] Foreign keys enabled\n");

    // Create album table
    sql = "CREATE TABLE IF NOT EXISTS album ("
          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "name TEXT NOT NULL, "
          "artist TEXT NOT NULL, "
          "year INTEGER NOT NULL, "
          "n_songs INTEGER NOT NULL);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create 'album' table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    printf("[SQLite] Table 'album' is ready\n");

    // Create song table
    sql = "CREATE TABLE IF NOT EXISTS song ("
          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "album_id INTEGER NOT NULL, "
          "title TEXT NOT NULL, "
          "artist TEXT NOT NULL, "
          "FOREIGN KEY(album_id) REFERENCES album(id) ON DELETE CASCADE);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create 'song' table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    printf("[SQLite] Table 'song' is ready\n");

    // Create member table
    sql = "CREATE TABLE IF NOT EXISTS member ("
          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "name TEXT NOT NULL, "
          "age INTEGER NOT NULL, "
          "grade INTEGER NOT NULL);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create 'member' table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    printf("[SQLite] Table 'member' is ready\n");

    // Create playlist table
    sql = "CREATE TABLE IF NOT EXISTS playlist ("
          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "member_id INTEGER NOT NULL, "
          "name TEXT NOT NULL, "
          "UNIQUE(member_id, name), "
          "FOREIGN KEY(member_id) REFERENCES member(id) ON DELETE CASCADE);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create 'playlist' table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    printf("[SQLite] Table 'playlist' is ready\n");

    // Create playlist_song table
    sql = "CREATE TABLE IF NOT EXISTS playlist_song ("
          "playlist_id INTEGER NOT NULL, "
          "album_id INTEGER NOT NULL, "
          "song_id INTEGER NOT NULL, "
          "FOREIGN KEY(playlist_id) REFERENCES playlist(id) ON DELETE CASCADE, "
          "FOREIGN KEY(album_id) REFERENCES album(id) ON DELETE CASCADE, "
          "FOREIGN KEY(song_id) REFERENCES song(id) ON DELETE CASCADE);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create 'playlist_song' table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    printf("[SQLite] Table 'playlist_song' is ready\n");

    // Create member_album table
    sql = "CREATE TABLE IF NOT EXISTS member_album ("
          "member_id INTEGER NOT NULL, "
          "album_id INTEGER NOT NULL, "
          "FOREIGN KEY(member_id) REFERENCES member(id), "
          "FOREIGN KEY(album_id) REFERENCES album(id));";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create 'member_album' table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    printf("[SQLite] Table 'member_album' is ready\n");

    // Create member_song table
    sql = "CREATE TABLE IF NOT EXISTS member_song ("
          "member_id INTEGER NOT NULL, "
          "song_id INTEGER NOT NULL, "
          "FOREIGN KEY(member_id) REFERENCES member(id), "
          "FOREIGN KEY(song_id) REFERENCES song(id));";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create 'member_song' table: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    printf("[SQLite] Table 'member_song' is ready\n");

    // Create indexes to improve search performance
    sql = "CREATE INDEX IF NOT EXISTS idx_song_title ON song(title);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create index 'idx_song_title': %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    sql = "CREATE INDEX IF NOT EXISTS idx_album_name ON album(name);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create index 'idx_album_name': %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    sql = "CREATE INDEX IF NOT EXISTS idx_song_album_id ON song(album_id);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create index 'idx_song_album_id': %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    sql = "CREATE INDEX IF NOT EXISTS idx_album_id ON album(id);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create index 'idx_album_id': %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    sql = "CREATE INDEX IF NOT EXISTS idx_member_id ON member(id);";
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to create index 'idx_member_id': %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    printf("[SQLite] Indexes created successfully\n");

    return 0;
}