#include <sqlite3.h>
#include "database.h"
#include "hash.h"
#include "initialize_db.h"
#include "user.h"

int main(int argc, char* argv[]){
    sqlite3* db;
    if(sqlite3_open("music.db", &db)){
        fprintf(stderr, "[SQLite ERROR] Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("[SQLite] Opened DB\n");
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0); //외래키 활성화화


    if(*argv[1]== 'z'){
         initialize_database(db);
    }
    else if(*argv[1] == 'a'){
        get_album(db, atoi(argv[2]));
    }
    else if(*argv[1] == 'b'){ 
        ALBUM a1;
        strcpy(a1.name, argv[2]);
        strcpy(a1.artist, argv[3]);
        a1.year = atoi(argv[4]);
        a1.n_songs = atoi(argv[5]);
        add_album(db,&a1);
    }
    else if(*argv[1] == 'c'){
        delete_album(db, atoi(argv[2]));
    }
    else if(*argv[1] == 'd'){
        get_song(db, atoi(argv[2]));
    }

    else if(*argv[1] == 'e'){
        SONG s1;
        s1.albumID = atoi(argv[2]);  // 앨범 ID
        strcpy(s1.artist, argv[3]);  // 아티스트
        strcpy(s1.title, argv[4]);   // 노래 제목
        add_song(db, s1.albumID, &s1);
    }
    else if(*argv[1] == 'f'){
        delete_song(db, atoi(argv[2]));
    }
    else if(*argv[1] == 'g'){
        get_member(db, atoi(argv[2]));
    }
    else if(*argv[1] == 'h'){
        MEMBER m1;
        strcpy(m1.name, argv[2]);
        m1.age = atoi(argv[3]);
        m1.grade = atoi(argv[4]);
        add_member(db, &m1);
    }
    else if(*argv[1] == 'i'){
        delete_member(db, atoi(argv[2]));
    }
    else if(*argv[1] == 'j') {  // 앨범 구매
        buy_album(db, atoi(argv[2]), atoi(argv[3]));
    }
    else if(*argv[1] == 'k'){
        create_playlist(db, atoi(argv[2]), argv[3]);
    }
    else if(*argv[1] == 'm'){
        add_song_to_playlist(db, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    }
    else if(*argv[1] == 'n'){
        delete_playlist(db, atoi(argv[2]));
    }
    else if(*argv[1] == 'l'){
        get_playlist(db, atoi(argv[2]));
    }
    else if(*argv[1] == 'y'){
        load_song_hash(db);
    }
    else if(*argv[1] == 'u'){
        load_user(db, atoi(argv[2]));
    }
    

    sqlite3_close(db);
    printf("[SQLite] Closed DB\n");
    return 0;
}
