tweetbot: tweetbot.cpp tweetbot.h
	g++ tweetbot.cpp -I./include/ -L /usr/local/lib/ -ltwitcurl -o tweetbot -D_GLIBCXX_DEBUG -lcurl

debug: tweetbot.cpp
	g++ tweetbot.cpp -g -I./include/ -L /usr/local/lib/ -ltwitcurl -o tweetbot -D_GLIBCXX_DEBUG
