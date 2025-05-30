LIBS = -lsqlite3

all:
	make clean
	gcc -o fm llist.c fm.c database.c $(LIBS)

db_create:
# fm [i/d/s/m] [into] [new] [title] [artst] [year] [songs]
	./fm i iu2003.db iu2010.db flower iu 2010 0
	./fm i iu2010.db iu2012.db island iu 2012 0
	./fm i iu2012.db sza.db sos sza 2019 0
	./fm i sza.db steve_lacy.db gemini_rights steve_lacy 2021 0
song_insert:
# fm is [album]   [index of song]   [title]   [artist]
	./fm is iu2010.db 1 title1 artist1
	./fm is iu2010.db 2 title3 artist3
	./fm is iu2012.db 1 title4 artist4
	./fm is iu2012.db 2 title5 artist5
	./fm is sza.db 1 zinzzaggt sza
	./fm is sza.db 2 kill_bill sza
	./fm is sza.db 3 seek_and_destroy sza
	./fm is sza.db 4 low sza
	./fm is sza.db 5 love_language sza
	./fm is sza.db 6 blind sza
	./fm is steve_lacy.db 1 static steve_lacy
	./fm is steve_lacy.db 2 helmet steve_lacy
	./fm is steve_lacy.db 3 mercury steve_lacy
	./fm is steve_lacy.db 4 buttons steve_lacy
	./fm is steve_lacy.db 5 bad_habit steve_lacy

find_title:
# fm ft [title]
	./fm ft title1
#   ./fm ft title2
	./fm ft title3

find_artist:
# fm fx [artist]
	./fm fx artist1
#   ./fm fx artist2
	./fm fx artist3

db_all_print:
# fm p
	./fm p

db_print:
	./fm s iu2010.db
	./fm s iu2012.db
	./fm s iu2005.db

add_member:
	./fm im member lee 10 1
	./fm im lee kate 20 2
	./fm im kate kim 30 3

buy:
	./fm buy kate island
	./fm buy kim flower
#	./fm buy kim iu2012.db
#	./fm buy lee iu2010.db

add_playlist:
	./fm ip kate myList island title4
	./fm ip kate myList island title5
	

run:
#   echo "" > iu2003.db
	echo "" > member_db.txt
	echo "" > db.txt
	make db_create
	make song_insert
#	make find_title
#	make find_artist
	make db_all_print
	make add_member
	make buy
	make add_playlist
#	./fm pm

clean:
	rm -rf *.o 
	rm -rf *.db

help:
	@echo "my first program hahahah"
	@echo "do enter make all"