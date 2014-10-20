#include "todo.h"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::fstream;
using std::string;
using std::vector;
using std::find;

void load(const string& fileName, TODOMap& map);
void save(const string& fileName, TODOMap& map);


int main(int argc, char** argv)
{
	string fileName = "fileName";
	string category = "";
	TODOMap	todoMap;
	WorkMode workmode = none;
	load(fileName, todoMap);
	int res = 0;

	while ((res = getopt(argc, argv, "a:ef:hlr:s:t")) != -1) {
		if (workmode != none)
			break;

		switch (res) {
		case 'a':
			workmode = add;
			category = optarg;
		break;
		case 'e':
			workmode = erase;
		break;
		case 'f':
			fileName = optarg;
		break;
		case 'h':
			cout << "todo - make a private todo-list! This application help to ";
			cout << "create todo-list for everything." << endl;
			cout << "Flags:" << endl;
			cout << "'-a'\tAdd todo item to todo category (category must be created)." << endl;
			cout << "\tFor example:" << endl;
			cout << "\t>todo -a film2watch." << endl;
			cout << "\tFor example:" << endl;
			cout << "\t>todo -a film2watch." << endl;
			cout << "'-f'\tSet processing file name. Must be set before work mode!" << endl;
			cout << "\tFor example:" << endl;
			cout << "\t>todo -f ./todoFilename -a film2watch" << endl;
			cout << "'-e'\tClear todo list." << endl;
			cout << "'-h'\tShow this help." << endl;
			cout << "'-l'\tShow all todo items." << endl;
			cout << "'-r'\tRemove todo item." << endl;
			cout << "'-s'\tShow item of todo category." << endl;
			cout << "'-t'\tShow todo categories." << endl;
		return 0;
		case 'l':
			workmode = showAll;
			category = "";
		break;
		case 'r':
			workmode = removeItem;
			category = optarg;
		break;
		case 's':
			workmode = show;
			category = optarg;
		break;
		case 't':
			workmode = showCat;
		break;
		case '?':
			cerr << "Get option error." << endl;
		break;
		}
	}

	switch (workmode) {
	case show:
	{
		for (std::vector<std::string>::const_iterator i = todoMap[category].begin(); i != todoMap[category].end(); ++i)
			std::cout << (*i) << std::endl;
		break;
	}
	case showAll:
	{
		for (TODOIterator it = todoMap.begin(); it != todoMap.end(); ++it) {
			std::cout << (*it).first << ":" << std::endl;
			for (std::vector<std::string>::const_iterator i = (*it).second.begin(); i != (*it).second.end(); ++i)
				std::cout << "\t" << (*i) << std::endl;
		}
		break;
	}
	case add:
	{
		cout << "Enter item name: " << endl;
		string token;
		cin >> token;
		todoMap[category].push_back(token);
		save(fileName, todoMap);
		break;
	}
	case showCat:
	{
		for (TODOIterator it = todoMap.begin(); it != todoMap.end(); ++it)
			std::cout << (*it).first << std::endl;
		break;
	}
	case erase:
	{
		todoMap.clear();
		save(fileName, todoMap);
		break;
	}
	case removeItem:
	{
		cout << "Enter item name: " << endl;
		string token;
		cin >> token;
		todoMap[category].erase(find(todoMap[category].begin(), todoMap[category].end(), token));
		save(fileName, todoMap);
		break;
	}
	}

	return 0;
}

void load(const string& fileName, TODOMap& todoMap)
{
	ifstream input(fileName.c_str());
	if (!input) {
		cerr << "Coldn't open file " << fileName << endl;
		return;
	}

	do {
		std::string str;
		getline(input, str);
		std::istringstream iss(str);
		std::string token;

		if (getline(iss, token, ':')) {
			std::string title = token;
			if (todoMap.find(token) == todoMap.end()) {
				std::vector<std::string> items;
				todoMap.insert(std::pair< std::string, std::vector<std::string> >(title, items));
			}
			if (getline(iss, token, ':')) {
				std::istringstream subIss(token);
				while (getline(subIss, token, '|'))
					todoMap[title].push_back(token);
			}
		}
	} while (input.good());
	input.close();

}

void save(const string& fileName, TODOMap& map)
{
	ofstream output(fileName.c_str());
	if (!output) {
		cerr << "Coldn't open file to save " << fileName << endl;
		return;
	}

	for (TODOIterator it = map.begin(); it != map.end(); ++it) {
		output << (*it).first << ":";
		for (std::vector<std::string>::const_iterator i = (*it).second.begin(); i != (*it).second.end(); ++i) {
			if (i != (*it).second.begin())
				output << "|";
			output << (*i);
		}
		output << std::endl;
	}
}
