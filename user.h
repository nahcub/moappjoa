#pragma once
#include "database.h"

// 현재 로그인한 사용자의 정보를 저장하는 전역 변수
extern MEMBER* current_user;

// 사용자 정보를 캐시에 로드
void load_user(sqlite3* db, int memberId);

// 캐시된 사용자 정보 가져오기
MEMBER* get_current_user();

// 캐시된 사용자 정보 초기화
void clear_user();

