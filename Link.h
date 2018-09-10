#include <string.h>
#include <vector>

class Node;

class Link {
  int weight;
  Node* target;
  public:
    void increaseWeight();
} Link;

void Link::increaseWeight() {
  this->weight++;
}  
