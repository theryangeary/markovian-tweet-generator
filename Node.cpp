#include "Node.h"
#include "Link.h"

void Node::addLink(Node* target) {
  Link* newLink = malloc(sizeof(Link));
  newLink->weight = 1;
  newLink->target = target;
  this.links.push_back(&newLink);
} 

void Node::setValue(std::string value) {
  this.value = value;
}
