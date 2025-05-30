#include <sqlite3.h>
#include "llist.h"

LLIST* load_albums_from_db(LLIST* llist, sqlite3* db);