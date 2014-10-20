#ifndef TODO_H
#define TODO_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

typedef std::map< std::string, std::vector<std::string> > TODOMap;
typedef TODOMap::const_iterator	TODOIterator;

enum WorkMode { add, show, showAll, showCat, removeItem, erase, none };

#endif
