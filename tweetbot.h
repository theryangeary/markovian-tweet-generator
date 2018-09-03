/*
   Markov Chain Tweet Bot - read another twitter user's tweets, createa  Markov
   chain model from it, and post tweet's in their style back to twitter.

   Ryan Geary - September 3, 2018

   Using twitcurl - twitter API library for C++
   https://github.com/swatkat/twitcurl
   
   Using example code from https://github.com/swatkat/twitcurl/tree/master/twitterClient
   */

#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include "oauthlib.h"
#include "curl/curl.h"
