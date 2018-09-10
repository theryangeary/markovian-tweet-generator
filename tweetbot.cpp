/*
   Markov Chain Tweet Bot - read another twitter user's tweets, createa  Markov
   chain model from it, and post tweet's in their style back to twitter.

   Ryan Geary - September 3, 2018

   Using twitcurl - twitter API library for C++
   https://github.com/swatkat/twitcurl
   
   Using example code from https://github.com/swatkat/twitcurl/tree/master/twitterClient
   */

#include "tweetbot.h"

using json = nlohmann::json;

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
    
    twitCurl twitterObj;
    std::string tmpStr, tmpStr2;
    std::string replyMsg;
    char tmpBuf[1024];

    /* Set twitter username and password */
    twitterObj.setTwitterUsername( userName );
    twitterObj.setTwitterPassword( passWord );

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "uHcsBRso2bnduxflFwKDUfGFu" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "eCOvdRZtYohTpOwDiGJxS23MRG8AJK1tg4LfWtt0yDdUOLRVbP" ) );

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

    oAuthTokenKeyIn.open( "access_token" );
    oAuthTokenSecretIn.open( "access_token_secret");

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();

    if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        /* If we already have these keys, then no need to go through auth again */
        printf( "\nUsing:\nKey: %s\nSecret: %s\n\n", myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str() );

        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }
    else
    {
        /* Step 2: Get request token key and secret */
        std::string authUrl;
        twitterObj.oAuthRequestToken( authUrl );

        /* Step 3: Get PIN  */
        memset( tmpBuf, 0, 1024 );
        printf( "\nDo you want to visit twitter.com for PIN (0 for no; 1 for yes): " );
        fgets( tmpBuf, sizeof( tmpBuf ), stdin );
        tmpStr = tmpBuf;
        if( std::string::npos != tmpStr.find( "1" ) )
        {
            /* Ask user to visit twitter.com auth page and get PIN */
            memset( tmpBuf, 0, 1024 );
            printf( "\nPlease visit this link in web browser and authorize this application:\n%s", authUrl.c_str() );
            printf( "\nEnter the PIN provided by twitter: " );
            fgets( tmpBuf, sizeof( tmpBuf ), stdin );
            tmpStr = tmpBuf;
            twitterObj.getOAuth().setOAuthPin( tmpStr );
        }
        else
        {
            /* Else, pass auth url to twitCurl and get it via twitCurl PIN handling */
            twitterObj.oAuthHandlePIN( authUrl );
        }

        /* Step 4: Exchange request token with access token */
        twitterObj.oAuthAccessToken();

        /* Step 5: Now, save this access token key and secret for future use without PIN */
        twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

        /* Step 6: Save these keys in a file or wherever */
        std::ofstream oAuthTokenKeyOut;
        std::ofstream oAuthTokenSecretOut;

        oAuthTokenKeyOut.open( "twitterClient_token_key.txt" );
        oAuthTokenSecretOut.open( "twitterClient_token_secret.txt" );

        oAuthTokenKeyOut.clear();
        oAuthTokenSecretOut.clear();

        oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
        oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

        oAuthTokenKeyOut.close();
        oAuthTokenSecretOut.close();
    }
    /* OAuth flow ends */

    /* Account credentials verification */
    if( twitterObj.accountVerifyCredGet() )
    {
        twitterObj.getLastWebResponse( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet web response:\n%s\n", replyMsg.c_str() );
        json response = json::parse(replyMsg);
        auto name = response["name"].get<std::string>();
        std::cout << name << std::endl;
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
    }

    int numTweets = 4;
    twitterObj.timelineUserGet(true, false, numTweets, "BarackObama", false, true);
    twitterObj.getLastWebResponse(replyMsg);
    json response2 = json::parse(replyMsg);

    std::cout << response2 << std::endl;
    for (int i = 0; i < numTweets - 1; i++) {
      std::cout << response2[i].at("full_text").get<std::string>() << std::endl;
    }

    std::unordered_map<std::string, Node*> dict;
    Node* currentNode;
    Node* startNode = new Node();
    startNode->value = START;
    dict.insert({START, startNode});
    std::string tweet;
    std::string word;

    for (int i = 0; i < numTweets - 1; i++) {
      tweet = response2[i].at("full_text").get<std::string>();
      currentNode = startNode;
      int pos = 0;
        do {
          std::cout << word << std::endl;
          word = tweet.substr(pos, tweet.find(" "));
          pos = tweet.find(" ") + 1;
        } while (pos < tweet.length());
    }
}
