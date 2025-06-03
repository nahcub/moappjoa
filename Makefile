LIBS = -lsqlite3

all:
	make clean
	gcc -o fm fm.c database.c initialize_db.c hash.c user.c $(LIBS)

add_album:
# fm [i/d/s/m] [into] [new] [title] [artst] [year] [songs]
	./fm b flower iu 2010 0
	./fm b island iu 2012 0
	./fm b sos sza 2019 0
	./fm b gemini_rights steve_lacy 2021 0
	./fm b album_to_delete artist 2025 0

get_album:
	./fm a 3
delete_album:
	./fm c 5

add_song:
# IU - flower 앨범 노래들
	./fm e 1 iu flower
	./fm e 1 iu good_day
	./fm e 1 iu you_and_i

# IU - island 앨범 노래들
	./fm e 2 iu island
	./fm e 2 iu every_end_of_the_day
	./fm e 2 iu peach

# SZA - SOS 앨범 노래들
	./fm e 3 sza seek_and_destroy
	./fm e 3 sza low
	./fm e 3 sza love_language
	./fm e 3 sza blind
	./fm e 3 sza used
	./fm e 3 sza snooze
	./fm e 3 sza notice_me
	./fm e 3 sza gone_girl
	./fm e 3 sza smoking_on_my_ex_pack
	./fm e 3 sza ghost_in_the_machine

# Steve Lacy - Gemini Rights 앨범 노래들
	./fm e 4 steve_lacy static
	./fm e 4 steve_lacy helmet
	./fm e 4 steve_lacy mercury
	./fm e 4 steve_lacy buttons
	./fm e 4 steve_lacy bad_habit
get_song:
	./fm d 5

delete_song:
	./fm f 5
add_member:
	./fm h "John_Smith" 20 1
	./fm h "Emma_Wilson" 22 2
	./fm h "Michael_Brown" 25 3
	./fm h "Sarah_Davis" 19 1
	./fm h "David_Lee" 23 2
get_member:
	./fm g 1
delete_member:
#	./fm i 1


buy_album:
	./fm j 1 3  # John Smith가 sos 앨범 구매
	./fm j 1 4  # John Smith가 gemini_rights 구매
	./fm j 2 2  # Emma Wilson이 island 앨범 구매
create_playlist:
	./fm k 2 "brooklyn"
	./fm k 2 "Workout_Mix"
	./fm k 3 "Chill"

add_song_to_playlist:
	./fm m 1 3 7  # John Smith의 brooklyn 플레이리스트에 sos의 seek_and_destroy 추가
	./fm m 1 3 8  # John Smith의 brooklyn 플레이리스트에 sos의 low 추가
	./fm m 1 3 9  # John Smith의 brooklyn 플레이리스트에 sos의 love_language 추가
	./fm m 3 4 21  # Emma Wilson의 Chill 플레이리스트에 steve_lacy의 gemini_rights앨범의 bad_habit 추가가

get_playlist:
	./fm l 1 # brooklyn playlist 검색
delete_playlist:
	./fm n 3 # Chill playlist 삭제 
run:
	./fm z
	make add_album
	make get_album
	make delete_album
	make add_song
	make get_song
	make delete_song
	make add_member
	make get_member
	make delete_member
	make create_playlist
	make buy_album
	make add_song_to_playlist
	./fm y
	make get_playlist
	make delete_playlist

clean:
	rm -rf *.o 
	rm -rf *.db

help:
	@echo "my first program hahahah"
	@echo "do enter make all"