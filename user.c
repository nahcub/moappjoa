#include <stdio.h>
#include <stdlib.h>
#include "user.h"

MEMBER* current_user = NULL;

void load_user(sqlite3* db, int memberId) {
    // 기존 사용자 정보가 있다면 해제
    if (current_user != NULL) {
        free(current_user);
        current_user = NULL;
    }

    // DB에서 사용자 정보 가져오기
    current_user = get_member(db, memberId);
    if (current_user) {
        printf("[load_user] User loaded: %s (ID: %d)\n", 
               current_user->name, current_user->memberId);
    } else {
        printf("[load_user] Failed to load user (ID: %d)\n", memberId);
    }
}

MEMBER* get_current_user() {
    if (current_user) {
        printf("[get_current_user] Current user: %s (ID: %d)\n", 
               current_user->name, current_user->memberId);
        return current_user;
    }
    printf("[get_current_user] No user loaded\n");
    return NULL;
}

void clear_user() {
    if (current_user) {
        printf("[clear_user] Clearing user: %s (ID: %d)\n", 
               current_user->name, current_user->memberId);
        free(current_user);
        current_user = NULL;
    }
}
