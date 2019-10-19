#ifndef GUARD_split
#define GUARD_split

#include <string>

bool notspace(char c);

bool space(char c);

template<class Out>
void split(const std::string& str, Out os);

// test if this is necessary for the template
#include "split.cpp"

#endif
