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
    printf( "\nUsage:\ntwitterClient -u username -p password -t usernameToMimic\nOptional arguments:\n\t-d\n\t\trun continuously, generating tweets once in a while\n\t-w\n\t\twait time between tweets. Important to keep this long to avoid going over rate limits for the twitter API. If not running with -l make it as short as you want. 15 minutes by default.\n\t-l\n\t\tlive mode. Posts generated tweets to twitter as user specified as -u.\n\n" );
}

int main( int argc, char* argv[] )
{
    /* Get username and password from command line args */
    std::string userName( "" );
    std::string passWord( "" );
    std::string targetUser( "twitter" );
    int tweetTimer = 60*15; // 15 minutes
    bool live = false;
    bool daemon = false;
    if( argc > 4 )
    {
        for( int i = 1; i < argc; i += 1 )
        {
            if( 0 == strncmp( argv[i], "-u", strlen("-u") ) )
            {
                userName = argv[i+1];
            }
            else if( 0 == strncmp( argv[i], "-p", strlen("-p") ) )
            {
                passWord = argv[i+1];
            }
            else if( 0 == strncmp( argv[i], TWEET_TIMER_FLAG, strlen(TWEET_TIMER_FLAG) ) )
            {
	      tweetTimer = (int) strtol(argv[i+1], 0, 10);
	      std::cout << tweetTimer << std::endl;
            }
            else if( 0 == strncmp( argv[i], LIVE_FLAG, strlen(LIVE_FLAG) ) )
            {
	      live = true;
            }
            else if( 0 == strncmp( argv[i], TARGET_FLAG, strlen(TARGET_FLAG) ) )
            {
	      targetUser = argv[i+1];
            }
            else if( 0 == strncmp( argv[i], DAEMON_FLAG, strlen(DAEMON_FLAG) ) )
            {
	      daemon = true;
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

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myCustomerAccessTokenKey("");
    std::string myCustomerAccessTokenSecret("");
    std::ifstream customerTokenKeyIn;
    std::ifstream customerTokenSecretIn;
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn;

    oAuthTokenKeyIn.open( "access_token" );
    oAuthTokenSecretIn.open( "access_token_secret");
    customerTokenKeyIn.open( "customer_token" );
    customerTokenSecretIn.open( "customer_token_secret" );

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    customerTokenKeyIn >> tmpBuf;
    myCustomerAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    customerTokenSecretIn >> tmpBuf;
    myCustomerAccessTokenSecret = tmpBuf;

    twitterObj.getOAuth().setConsumerKey(myCustomerAccessTokenKey);
    twitterObj.getOAuth().setConsumerSecret(myCustomerAccessTokenSecret);

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();
    customerTokenKeyIn.close();
    customerTokenSecretIn.close();

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
        json response = json::parse(replyMsg);
	auto name = response["name"].get<std::string>();
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
    }

    int numTweets = 200;
    twitterObj.timelineUserGet(true, false, numTweets, targetUser, false, true);
    twitterObj.getLastWebResponse(replyMsg);
    json response2 = json::parse(replyMsg);

    std::unordered_map<std::string, Node*> dict;
    Node* currentNode;
    Node* startNode = new Node();
    Node* nextNode;
    startNode->value = START;
    dict.insert({START, startNode});
    std::string tweet;
    std::string word;

    
    for (int i = 0; i < response2.size() - 1; i++) {
      tweet = response2[i].at("full_text").get<std::string>();
      currentNode = startNode;

      std::locale loc;
      std::string str = tweet;

      
      bool linkFound = false;

      for (std::string::iterator it=str.begin(); it!=str.end() - 1; ++it)
      {
        if (*it == '\"' || *it == '(' || *it == ')') {
          str.erase(it, it+1);
        }
      }

      tweet = str;

      int pos = 0;
      do {

        word = tweet.substr(pos, tweet.find(" ", pos) - pos);
	std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        pos = tweet.find(" ", pos) + 1;
        std::unordered_map<std::string, Node*>::const_iterator included = dict.find(word);

        if (included == dict.end()) {
          nextNode = new Node();
          nextNode->value = word; 
          dict.insert({word, nextNode});
        }
        else {
          nextNode = included->second;
        } 

        if (!currentNode->increaseWeight(nextNode)) {
          currentNode->addLink(nextNode);
        }

        currentNode = nextNode;
      } while (tweet.find(" ", pos) != std::string::npos);
    }

    do {
	currentNode = dict.at(START);
	int outputLength = 0;
	std::string outputTweet;
	outputTweet.reserve(TWEET_LENGTH * sizeof(char));
	outputTweet = "";

	while (outputTweet.length() < TWEET_LENGTH) {
	  if(currentNode->getNextNode() != NULL){
	    currentNode = currentNode->getNextNode();
	    if (outputTweet.length() + currentNode->value.length() < TWEET_LENGTH) {
	      outputTweet += (std::string) currentNode->value;
		outputTweet += " ";
	    }
	  }
	  else {
	    break;
	  }
	}

	if (live) {
	    bool statusUpdateResult = twitterObj.statusUpdate(outputTweet);
	    twitterObj.getLastWebResponse(replyMsg);
	    if (!replyMsg.find("error"))  {
		std::cout << "Tweet sent: :" ;
	    }
	    else {
		std::cout << "Tweet failed! :" ;
	    }
	}
	std::cout << outputTweet << "\n" << std::endl;

     	sleep(tweetTimer);
    } while(daemon);
    
    
    for (auto i : dict) {
      free(i.second);
      

    }


}
