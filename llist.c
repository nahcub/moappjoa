#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "llist.h"

NODE* search_file(LLIST* llist, char* filename){
    int i = 0;
    NODE* current = (NODE*)(llist->head);
    while(current != NULL && strcmp(current->fileName, filename) != 0) {
        current = current->next;
        i++;
    }
    if (current == NULL) {
        fprintf(stderr, "[search_file] There is no file!\n");
        return NULL;
    }

    printf("[search_file] Found File name: %s, index : %d\n", current->fileName, i);

    // SQLite에서 앨범 정보 읽기
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    rc = sqlite3_open("music.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Cannot open DB: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    const char* sql = "SELECT name, artist, year, n_songs FROM album WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // 바인딩
    sqlite3_bind_text(stmt, 1, filename, -1, SQLITE_STATIC);

    // 결과 가져오기
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ALBUM* p_album = (ALBUM*)malloc(sizeof(ALBUM));
        strcpy(p_album->name, (const char*)sqlite3_column_text(stmt, 0));
        strcpy(p_album->artist, (const char*)sqlite3_column_text(stmt, 1));
        p_album->year = sqlite3_column_int(stmt, 2);
        p_album->n_songs = sqlite3_column_int(stmt, 3);

        current->p_album = p_album;
    } else {
        fprintf(stderr, "[search_file] No album with id = %s in database\n", filename);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return NULL;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return current;
}

// SQLite callback function for album data
static int album_callback(void* data, int argc, char** argv, char** azColName) {
    LLIST* llist = (LLIST*)data;
    NODE* new_node = (NODE*)malloc(sizeof(NODE));
    new_node->p_album = (ALBUM*)malloc(sizeof(ALBUM));
    
    strcpy(new_node->fileName, argv[0]);  // id
    strcpy(new_node->p_album->name, argv[1]);  // name
    strcpy(new_node->p_album->artist, argv[2]);  // artist
    new_node->p_album->year = atoi(argv[3]);  // year
    new_node->p_album->n_songs = atoi(argv[4]);  // n_songs
    
    new_node->next = NULL;
    
    if(llist->head == NULL) {
        new_node->pre = NULL;
        llist->head = new_node;
        llist->tail = new_node;
    } else {
        new_node->pre = (NODE*)llist->tail;
        ((NODE*)llist->tail)->next = new_node;
        llist->tail = new_node;
    }
    llist->num++;
    
    return 0;
}
LLIST* insert_file(LLIST* llist, char* s_fileName, char* a_fileName, ALBUM* album){
    sqlite3* db;
    char* err_msg = 0;
    
    if(sqlite3_open("music.db", &db)) {
        fprintf(stderr, "[SQLite ERROR] Cannot open database: %s\n", sqlite3_errmsg(db));
        return llist;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), 
        "INSERT INTO album (id, name, artist, year, n_songs) VALUES ('%s', '%s', '%s', %d, %d);",
        a_fileName, album->name, album->artist, album->year, album->n_songs);
    
    if(sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to insert data: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return llist;
    }
    
    NODE* new_node = (NODE*)malloc(sizeof(NODE));
    new_node->p_album = album;
    strcpy(new_node->fileName, a_fileName);
    new_node->next = NULL;
    
    if(llist->head == NULL) {
        new_node->pre = NULL;
        llist->head = new_node;
        llist->tail = new_node;
    } else {
        new_node->pre = (NODE*)llist->tail;
        ((NODE*)llist->tail)->next = new_node;
        llist->tail = new_node;
    }
    llist->num++;
    
    sqlite3_close(db);
    return llist;
}

LLIST* delete_file(LLIST* llist, char* id) {
    sqlite3* db;
    char* err_msg = 0;
    
    if(sqlite3_open("music.db", &db)) {
        fprintf(stderr, "[SQLite ERROR] Cannot open database: %s\n", sqlite3_errmsg(db));
        return llist;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), "DELETE FROM album WHERE id = '%s';", id);
    
    if(sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to delete data: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return llist;
    }
    
    NODE* current = llist->head;
    while(current != NULL) {
        if(strcmp(current->fileName, id) == 0) {
            if(current == llist->head) {
                llist->head = current->next;
                if(llist->head != NULL) {
                    ((NODE*)llist->head)->pre = NULL;
                }
            } else if(current == llist->tail) {
                llist->tail = current->pre;
                ((NODE*)llist->tail)->next = NULL;
            } else {
                ((NODE*)current->pre)->next = current->next;
                ((NODE*)current->next)->pre = current->pre;
            }
            free(current->p_album);
            free(current);
            llist->num--;
            break;
        }
        current = current->next;
    }
    
    sqlite3_close(db);
    return llist;
}

LLIST* modify_file(LLIST* llist, char* id, char* new_name) {
    sqlite3* db;
    char* err_msg = 0;
    
    if(sqlite3_open("music.db", &db)) {
        fprintf(stderr, "[SQLite ERROR] Cannot open database: %s\n", sqlite3_errmsg(db));
        return llist;
    }
    
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE album SET name = '%s' WHERE id = '%s';", new_name, id);
    
    if(sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to update data: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return llist;
    }
    
    NODE* current = llist->head;
    while(current != NULL) {
        if(strcmp(current->fileName, id) == 0) {
            strcpy(current->p_album->name, new_name);
            break;
        }
        current = current->next;
    }
    
    sqlite3_close(db);
    return llist;
}

LLIST* insert_song(LLIST* llist, char* album, char* index, char* title, char* artist){      
	NODE* node;

	if((node = search_file(llist, album)) == NULL){
        printf("응 아니야");
		return NULL;
	}
	strcpy(node->p_album->s_list[atoi(index) - 1].title, title);
	strcpy(node->p_album->s_list[atoi(index) - 1].artist, artist);
	node->p_album->n_songs++;

    sqlite3* db;
    char* err_msg = 0;
    char sql[256];

    // 데이터베이스 열기
    if (sqlite3_open("music.db", &db)) {
        fprintf(stderr, "[SQLite ERROR] Cannot open DB: %s\n", sqlite3_errmsg(db));
        return llist;
    }

    // SQL 쿼리 구성
    sprintf(sql,
        "INSERT INTO song (album_id, title, artist) VALUES ('%s', '%s', '%s');",
        album, title, artist);

    // SQL 실행
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to insert song: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    sqlite3_close(db);
        

	return llist;
}

SONG* find_title(LLIST* llist, char* title){
   int i, found = 0;
   NODE* current = llist->head;
   while(current != NULL) {
      for(i = 0 ; i < current->p_album->n_songs ; i++){
         if(strcmp(current->p_album->s_list[i].title, title) == 0){
            found++;
            break;
         }
      }
      if(found) break;
      current = current->next;
   }
   if(current == NULL){
      fprintf(stderr, "[Error] There is no file!\n");
      return NULL;
   }
   printf("***** Information of Album searched by %s *****\n", title);
   printf("Album Title: %s\n", current->p_album->name);
   printf("Album Artist: %s\n", current->p_album->artist);
   printf("Album Year: %d\n", current->p_album->year);
   printf("Album Number of Songs: %d\n", current->p_album->n_songs);
   printf("***** Information of Song searched by %s *****\n", title);
   printf("Title : %s, Artist : %s\n", current->p_album->s_list[i].title, current->p_album->s_list[i].artist);

   return &(current->p_album->s_list[i]);
}

LLIST* find_artist(LLIST* llist, char* artist){
   int i, found = 0;
   NODE* current = llist->head;
   while(current != NULL) {
      for(i = 0 ; i < current->p_album->n_songs ; i++){
         if(strcmp(current->p_album->s_list[i].artist, artist) == 0){
            found++;
            break;
         }
      }
      if(found) break;
      current = current->next;
   }
   if(current == NULL){
      fprintf(stderr, "[Error] There is no file!\n");
      return llist;
   }
   printf("***** Information of Album searched by %s *****\n", artist);
   printf("Album Title: %s\n", current->p_album->name);
   printf("Album Artist: %s\n", current->p_album->artist);
   printf("Album Year: %d\n", current->p_album->year);
   printf("Album Number of Songs: %d\n", current->p_album->n_songs);
   printf("***** Information of Song searched by %s *****\n", artist);
   printf("Title : %s, Artist : %s\n", current->p_album->s_list[i].title, current->p_album->s_list[i].artist);
   return llist;
}
void print_all(LLIST* llist) {
    sqlite3* db;
    char* err_msg = 0;
    
    if(sqlite3_open("music.db", &db)) {
        fprintf(stderr, "[SQLite ERROR] Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    const char* sql = "SELECT * FROM album;";
    
    if(sqlite3_exec(db, sql, album_callback, llist, &err_msg) != SQLITE_OK) {
        fprintf(stderr, "[SQLite ERROR] Failed to fetch data: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }
    
    NODE* current = llist->head;
    while(current != NULL) {
        printf("ID: %s\n", current->fileName);
        printf("Name: %s\n", current->p_album->name);
        printf("Artist: %s\n", current->p_album->artist);
        printf("Year: %d\n", current->p_album->year);
        printf("Number of Songs: %d\n", current->p_album->n_songs);
        printf("------------------------\n");
        current = current->next;
    }
    
    sqlite3_close(db);
}

MEMBERNODE* search_member(LLIST* mllist, char* name){
	int i = 0;
    MEMBERNODE* current = (MEMBERNODE*)(mllist->head);
    while(current != NULL && strcmp(current->member->name, name) != 0) {
        current = current->next;
        i++;
    }
    
    if (current == NULL) {
        fprintf(stderr, "[Member] There is no member!\n");
        return NULL;
    }
    
    printf("Found the member!\n");
    printf("    member name : %s, index : %d\n", current->member->name, i);
    
    char filename[120];
    strcpy(filename, current->member->name);
    strcat(filename, ".db");

    FILE* fp1 = fopen(filename, "rb");
    if(fp1 == NULL) {
        printf("can't open %s\n", filename);
        return NULL;
    }
    MEMBER* member = (MEMBER*)malloc(sizeof(MEMBER));
    fread(member, sizeof(MEMBER), 1, fp1);

	//printf("Member name: %s\n", member->name);
	//printf("Member age: %d\n", member->age);
	//printf("Member grade: %d\n", member->grade);
    
    
    return current;
}

/*
PLAYLIST* search_playlist(LLIST* mllist, char* name, char* playlist_name){
    MEMBERNODE* membernode = search_member(mllist, name);
    int i;
    for(i = 0 ; i < membernode->member->playlist->song.num;i++){
        if(strcmp(membernode->member->playlist[i].name,playlist_name)==0){
            return &membernode->member->playlist[i];
        }

    }
    PLAYLIST* playlist = (PLAYLIST*)malloc(sizeof(PLAYLIST));
    playlist.song = (LLIST*)malloc(sizeof(LLIST));
    playlist->song.num = 0;
    playlist->song.head = NULL;
    playlist->song.tail = NULL;
    strcpy(playlist->name, playlist_name);

    return playlist;
    
}

*/



ALBUM* search_album(LLIST* llist, char* album_name){
    NODE* current = (NODE*)(llist->head);
    while(current != NULL && strcmp(current->p_album->name, album_name) != 0) {
        current = current->next;
    }
    if (current == NULL) {
        fprintf(stderr, "[search_album] There is no %s!\n", album_name);
        return NULL;
    }
    printf("Found the Album!\n");
    return current->p_album;
}

LLIST* insert_member(LLIST* mllist, char* s_member, MEMBER* content){
    printf("insert_member\n");
	MEMBERNODE* current = search_member(mllist, s_member);
    if (current == NULL && mllist->num) {
        printf("search member fail\n");
        return mllist; // 검색 실패시 원본 리스트 반환
    }
    
    //mllist에 아무것도 없을때 
   	else if(current==NULL && !(mllist->num)){
      //RAM
      printf("[not ERROR], just empty\n");
      MEMBERNODE* node=(MEMBERNODE*)malloc(sizeof(MEMBERNODE));

      mllist->head=node;
      mllist->tail=node;
      mllist->num++;

      node->pre=NULL;
      node->next=NULL;

      node->member = content;
      //Filesystem
        char filename[100];
      strcpy(filename, content->name);
      strcat(filename, ".db");
      printf("strcat success\n");

      FILE* fp=fopen(filename, "wb");
      fwrite(content, sizeof(MEMBER),1,fp);
	  fclose(fp);
      return mllist;
   }
   //mllist에 일반적으로 추가할때 
   else if(current!=NULL && mllist->num){
        printf("멤버 찾기 성공\n");
		MEMBERNODE* node = (MEMBERNODE*)malloc(sizeof(MEMBERNODE));

		node->pre = current;
		node->next = current->next;
		if(current->next != NULL){
			node->next->pre = node;
		}
		current->next = node;
		node->member = content;
		mllist->num++;
		mllist->tail = node;

        char filename[100];
        strcpy(filename, content->name);
        strcat(filename, ".db");
        printf("strcat success\n");
        //Filesystem
		FILE* fp = fopen(filename, "wb");
		//fprintf(fp, "%s", content);
		fwrite(content, sizeof(MEMBER), 1, fp);
		fclose(fp);
		return mllist;
   	}
}


LLIST* buy_album(LLIST* mllist, LLIST* llist, char* member_name, char* album_name) {
    printf("111\n");
    ALBUM* album = search_album(llist, album_name);
    if (album == NULL) {
        fprintf(stderr, "[ERROR] Album '%s' not found in the list\n", album_name);
        return mllist;
    }
    printf("2222222222\n");
	MEMBERNODE* membernode = search_member(mllist, member_name);
    if (membernode == NULL) {
        fprintf(stderr, "[ERROR] Member '%s' not found in the list\n", member_name);
        return mllist;
    }
    printf("333\n");
	BUYALBUMNODE* banode = (BUYALBUMNODE*)malloc(sizeof(BUYALBUMNODE));
    
	banode->album = album;
	banode->next = NULL;
    banode->pre = NULL;
	if(membernode->member->buy_album.num == 0) {
		membernode->member->buy_album.head = banode;
		membernode->member->buy_album.tail = banode;
		membernode->member->buy_album.num++;
	}
	else {
		((BUYALBUMNODE*)(membernode->member->buy_album.tail))->next = banode;
		banode->pre = (BUYALBUMNODE*)(membernode->member->buy_album.tail);
		membernode->member->buy_album.tail = banode;		
		membernode->member->buy_album.num++;
	}
    /*
    printf("start file system\n");
	FILE* fp = fopen(member_name, "wb");
    if(fp == NULL) {
        printf("file is not opened!\n");
        return NULL;
    }
	fwrite(membernode->member, sizeof(MEMBER), 1, fp);
    BUYALBUMNODE* current = membernode->member->buy_album.head;
    while(current != NULL){
        fwrite(current->album->name, sizeof(char), 100, fp);
        current = current->next;
    }
    
    for (int i = 0; i < 100; i++) {
    
        fwrite(membernode->member->playlist[i].name, sizeof(char), 40, fp);
    
        SONGNODE* curr_song = NULL;
        if (membernode->member->playlist[i].song != NULL) {
            curr_song = membernode->member->playlist[i].song->head;
        }
    
        while (curr_song != NULL) {
            fwrite(curr_song->song->title, sizeof(char), 50, fp);
            curr_song = curr_song->next;
        }
    }
    
    

	fclose(fp);
    */
	return mllist;
}

LLIST* insert_playlist(LLIST* mllist, LLIST* llist, char* member_name, char* playlist_name, char* album_name, char* song_name){
        MEMBERNODE* membernode = search_member(mllist, member_name);
        ALBUM* album = search_album(llist, album_name);
        
        if(album == NULL) {
            return mllist;
        }
        if(membernode == NULL) {
            return mllist;
        }

        int found_song = 0;
        SONG* ptr = NULL;
        for(int j=0;j<album->n_songs;j++){
            if(strcmp(album->s_list[j].title, song_name) == 0){
                ptr = &(album->s_list[j]);
                found_song = 1;
                break;
            }
        }
        if(!found_song){
            fprintf(stderr,"[Error] can't find song\n");
            return mllist;
        }
    
        int index = -1;
    
        for(int i=0;i<membernode->member->n_playlists;i++){
            if(strcmp(membernode->member->playlist[i].name, playlist_name) == 0) {
                index = i;
                break;
            }
        }

        SONGNODE* song_node = (SONGNODE*)malloc(sizeof(SONGNODE));

        if(song_node== NULL){
            printf("메모리 오류");
            return mllist;
        }
        song_node->song = ptr;
        LLIST* target;

        if(index != -1){
            target = &(membernode->member->playlist[index].song);
            if(target->head == NULL){
                target->head = song_node;
                target->tail = song_node;
                song_node->pre = NULL;
                song_node->next = NULL;
                strcpy(song_node->album_name, album_name);
            }
            else{
                ((SONGNODE*)(target->tail))->next = song_node;
                song_node->pre = (SONGNODE*)(target->tail);
                song_node->next = NULL;
                target->tail = song_node;
                strcpy(song_node->album_name, album_name);
            }
            target->num++;
            printf("success\n");
        }
        else{
            index = membernode->member->n_playlists;
            strcpy(membernode->member->playlist[index].name, playlist_name);

            target = &(membernode->member->playlist[index].song);
            target->head = song_node;
            target->tail = song_node;
            song_node->pre = NULL;
            song_node->next = NULL;
            strcpy(song_node->album_name, album_name);
            target->num++;
            membernode->member->n_playlists++;
            printf("success\n");
        }
    
        return mllist;

        

    }

void print_member(LLIST* mllist){

	MEMBERNODE* current = (MEMBERNODE*)(mllist->head);
	BUYALBUMNODE* banode;
	printf("====== member info =====\n");
	for(int i = 0 ; i < mllist->num ; i++){
		printf("name : %s\n", current->member->name);
		printf("age : %d\n", current->member->age);
		printf("grade : %d\n", current->member->grade);
		printf("number of bought album : %d\n", current->member->buy_album.num);
		banode = (BUYALBUMNODE*)(current->member->buy_album.head);
		for(int j = 0 ; j < current->member->buy_album.num ; j++){
			printf("	Album : %s\n", banode->album->name);
			banode = banode->next;
		}
	current = current->next;
	}	
}

void search_song_by_title(sqlite3* db, const char* title) {
    sqlite3_stmt* stmt;
    char* sql = "SELECT s.title, s.artist, a.name as album_name, a.artist as album_artist, a.year "
                "FROM song s "
                "JOIN album a ON s.album_id = a.id "
                "WHERE s.title LIKE ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 검색어에 % 추가 (부분 일치 검색)
    char search_pattern[100];
    snprintf(search_pattern, sizeof(search_pattern), "%%%s%%", title);
    sqlite3_bind_text(stmt, 1, search_pattern, -1, SQLITE_STATIC);

    int found = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = 1;
        printf("\n***** 노래 검색 결과 *****\n");
        printf("노래 제목: %s\n", sqlite3_column_text(stmt, 0));
        printf("노래 아티스트: %s\n", sqlite3_column_text(stmt, 1));
        printf("앨범 제목: %s\n", sqlite3_column_text(stmt, 2));
        printf("앨범 아티스트: %s\n", sqlite3_column_text(stmt, 3));
        printf("앨범 발매년도: %d\n", sqlite3_column_int(stmt, 4));
        printf("************************\n");
    }

    if (!found) {
        printf("'%s' 제목의 노래를 찾을 수 없습니다.\n", title);
    }

    sqlite3_finalize(stmt);
}
