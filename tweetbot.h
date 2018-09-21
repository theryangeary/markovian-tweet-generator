/*
   Markov Chain Tweet Bot - read another twitter user's tweets, createa  Markov
   chain model from it, and post tweet's in their style back to twitter.

   Ryan Geary - September 3, 2018

   Using twitcurl - twitter API library for C++
   https://github.com/swatkat/twitcurl
   
   Using example code from https://github.com/swatkat/twitcurl/tree/master/twitterClient
   */

#include <string.h>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "oauthlib.h"
#include "curl/curl.h"
#include "include/twitcurl.h"
#include "include/nlohmann/json.hpp"

#define START "ASTARTINGWORDSORIDICULOUSITWOULDNEVERBEUSEDINATWEET"
#define TWEET_LENGTH 280
#define TWEET_TIMER_FLAG "-w"
#define LIVE_FLAG "-l"
#define TARGET_FLAG "-t"
#define DAEMON_FLAG "-d"

class Node {
  public:
    std::vector<int> weights;
    std::vector<Node* > targets;
    std::string value;
    void setValue(std::string value);
    bool increaseWeight(Node* targetNode);
    void increaseWeight(int index);
    void addLink(Node* newNode);
    Node* getNextNode();
    //~Node();
} ;

void Node::setValue(std::string value) {
  this->value = value;
}

bool Node::increaseWeight(Node* targetNode) {
  int i = 0;
  for (std::vector<Node*>::iterator it = targets.begin();
      it != targets.end(); it++) {
    if (targetNode == *it) {
      weights[i]++;
      return true;
    }
  }
  return false;
}

void Node::increaseWeight(int index) {
  weights[index]++;
}

void Node::addLink(Node* newNode) {
  weights.push_back(1);
  targets.push_back(newNode);
}

Node* Node::getNextNode() {
  int count = 0;
  for (int i = 0; i < weights.size(); i++) {
    count += weights[i];
  }
  if (count == 0) {
    return NULL;
  }
  int selection = rand(); 
  selection = selection % count;
  for (int i = 0; i < weights.size(); i++) {
    selection -= weights[i];
    if (selection <= 0) {
      return targets[i];
    }
  }
  return NULL;
}

