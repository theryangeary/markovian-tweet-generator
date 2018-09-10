tweetbot: tweetbot.cpp
	g++ tweetbot.cpp -I./include/ -L /usr/local/lib/ -ltwitcurl -o tweetbot

debug: tweetbot.cpp
	g++ tweetbot.cpp -g -I./include/ -L /usr/local/lib/ -ltwitcurl -o tweetbot
