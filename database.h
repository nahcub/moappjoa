#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "llist.h"

LLIST* load_albums_from_db(LLIST* llist, sqlite3* db);
SONG* find_title_db(sqlite3* db, const char* title);

#endif