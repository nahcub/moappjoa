#include <sqlite3.h>
#include "database.h"
#include "llist.h"
#include "initialize_db.h"


int main(int argc, char* argv[]){
    int file_index = 0;
    NODE* current;
    MEMBERNODE* member_current;
    SONGNODE* song_current;

    sqlite3* db;
    if(sqlite3_open("music.db", &db)){
        fprintf(stderr, "[SQLite ERROR] Cannot open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("[SQLite] Opened DB\n");

    /* Initialize data structure */
    LLIST* llist = (LLIST*)malloc(sizeof(LLIST));
    llist->head = NULL;
    llist->tail = NULL;
    llist->num = 0;

    LLIST* mllist = (LLIST*)malloc(sizeof(LLIST));
    mllist->head = NULL;
    mllist->tail = NULL;
    mllist->num = 0;

    llist = load_albums_from_db(llist, db);

    if(*argv[1] == 's'){
        search_file(llist, argv[2]);
    }
    else if(!strcmp(argv[1], "pm")){
        print_member(mllist);
    }
    else if(!strcmp(argv[1], "db")){
        initialize_database(db);
    }
    else if(!strcmp(argv[1], "buy")){
        mllist = buy_album(mllist, llist, argv[2], argv[3]);
        printf("%s 고객님, %s 앨범 구매 완료\n", argv[2], argv[3]);
    }
    else if(!strcmp(argv[1], "ip")){
        insert_playlist(mllist, llist, argv[2], argv[3], argv[4], argv[5]);
    }
    else if(!strcmp(argv[1], "im")){
        MEMBER m1;
        strcpy(m1.name,argv[3]);
        m1.age = atoi(argv[4]);
        m1.grade = atoi(argv[5]);
        m1.buy_album.head = NULL;
        m1.buy_album.tail = NULL;
        m1.buy_album.num = 0;
        m1.n_playlists=0;
        mllist = insert_member(mllist, argv[2], &m1);
        printf("======%s 추가완료===========\n",argv[3]);
    }
    else if(!strcmp(argv[1], "is")){
        llist = insert_song(llist, argv[2], argv[3], argv[4], argv[5]);
    }
    else if(!strcmp(argv[1], "ft")){
        find_title(llist, argv[2]);
    }
    else if(!strcmp(argv[1], "fx")){
        llist = find_artist(llist, argv[2]);
    }
    else if(*argv[1] == 'i'){
        ALBUM a1;
        strcpy(a1.name, argv[4]);
        strcpy(a1.artist, argv[5]);
        a1.year = atoi(argv[6]);
        a1.n_songs = atoi(argv[7]);
        llist = insert_file(llist, argv[2], argv[3], &a1);
    }
    else if(*argv[1] == 'd'){
        llist = delete_file(llist, argv[2]);
    }
    else if(*argv[1] == 'm'){
        llist = modify_file(llist, argv[2], argv[3]);
    }
    else if(*argv[1] == 'p'){
        print_all(llist);
    }
    else if(!strcmp(argv[1], "ftd")){  // search title in database
        find_title_db(db, argv[2]);
    }

    sqlite3_close(db);
    printf("[SQLite] Closed DB\n");
    printf("=========한바퀴 끝=======\n");
    return 0;
}
