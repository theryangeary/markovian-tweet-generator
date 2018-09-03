/*
   Markov Chain Tweet Bot - read another twitter user's tweets, createa  Markov
   chain model from it, and post tweet's in their style back to twitter.

   Ryan Geary - September 3, 2018

   Using twitcurl - twitter API library for C++
   https://github.com/swatkat/twitcurl
   
   Using example code from https://github.com/swatkat/twitcurl/tree/master/twitterClient
   */

#include "tweetbot.h"

void printUsage()
{
    printf( "\nUsage:\ntwitterClient -u username -p password\n" );
}

int main( int argc, char* argv[] )
{
    /* Get username and password from command line args */
    std::string userName( "" );
    std::string passWord( "" );
    if( argc > 4 )
    {
        for( int i = 1; i < argc; i += 2 )
        {
            if( 0 == strncmp( argv[i], "-u", strlen("-u") ) )
            {
                userName = argv[i+1];
            }
            else if( 0 == strncmp( argv[i], "-p", strlen("-p") ) )
            {
                passWord = argv[i+1];
            }
        }
        if( ( 0 == userName.length() ) || ( 0 == passWord.length() ) )
        {
            printUsage();
            return 0;
        }
    }
    else
    {
        printUsage();
        return 0;
    }
    



}
