#include <string.h>
#include <vector>

class Link;

class Node {
  std::vector<Link> links;
  public:
  std::string value;
    void addLink(Node* target);
    void setValue(std::string value);
} ;

