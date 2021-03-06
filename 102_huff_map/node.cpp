#include "node.h"
void Node::buildMap(BitString b, std::map<unsigned, BitString> & theMap) {
  //WRITE ME!
  if (sym == NO_SYM) {
    assert(right);
    assert(left);
    right->buildMap(b.plusOne(), theMap);
    left->buildMap(b.plusZero(), theMap);
    return;
  }
  assert(left == NULL && right == NULL);
  theMap[sym] = b;
  return;
}
