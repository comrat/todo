/*********** Includes. ***********/
#include <unistd.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
#include <fstream>
using std::ofstream;
using std::ifstream;
using std::fstream;
#include <string>
using std::string;
using std::getline;
#include <vector>
using std::vector;
using std::iterator;


/******* Type definition. ********/
typedef vector<string>::const_iterator str_iter;

enum mode {add, create, show, showAll, showCat, removeItem, erase, none};

/********* Protitypes. ***********/

void addTodoItem(const string &fileName, const string &category);

void getCategoryItems(const string &fileName, const string &category, vector<string> &items);

void removeTodoItem(const string &fileName, const string &category);

void showCategory(const string &fileName, const string &category);

/***** Programm enter point. *****/
int main(int argc, char** argv)
{
	int res = 0;
	mode workmode = none;
	string category = "";
	string todoFile = "todoFile";

	while ((res = getopt(argc, argv, "a:c:ef:hlr:s:t")) != -1) {
		if (workmode != none) {
			break;
		}

		switch (res) {
		case 'a':
			workmode = add;
			category = optarg;
		break;
		case 'c':
			workmode = create;
			category = optarg;
		break;
		case 'e':
			workmode = erase;
		break;
		case 'f':
			todoFile = optarg;
		break;
		case 'h':
			cout << "todo - make a private todo-list! This application help to ";
			cout << "create todo-list for everything." << endl;
			cout << "Flags:" << endl;
			cout << "'-a'\tAdd todo item to todo category (category must be created)." << endl;
			cout << "\tFor example:" << endl;
			cout << "\t>todo -a film2watch." << endl;
			cout << "'-c'\tCreate new todo category." << endl;
			cout << "\tFor example:" << endl;
			cout << "\t>todo -a film2watch." << endl;
			cout << "'-f'\tSet todoFile. Must be set before work mode!" << endl;
			cout << "\tFor example:" << endl;
			cout << "\t>todo -f ./todoFile -a film2watch" << endl;
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
	case add:
		addTodoItem(todoFile, category);
	break;
	case erase: {
		ofstream output(todoFile.c_str());
		if (!output) { cerr << "Coldn't open file " << todoFile << endl; }
		output << "";
		output.close();
	}
	break;
	case create: {
		fstream fs(todoFile.c_str(), std::fstream::out | std::fstream::app);
		if (!fs) {
			cerr << "Coldn't open file " << todoFile << endl;
		}
		fs << category << "{}" << endl;
		fs.close();
	}
	break;
	case showAll:
	case show: {
		vector<string> *items = new vector<string>();
		getCategoryItems(todoFile, category, *items);
		if (workmode == show) { cout << "Category '" << category << "' todo:" << endl; }
		for (str_iter idx = items->begin(); idx != items->end(); ++idx) {
			cout << "\t" << *idx << endl;
		}
		delete items;
	}
	break;
	case removeItem:
		removeTodoItem(todoFile, category);
	break;
	case showCat:
		showCategory(todoFile, category);
	break;
	}
	return 0;
}


/******* Implementation. *********/
void addTodoItem(const string &fileName, const string &category)
{
	ifstream input(fileName.c_str());

	if (!input) {
		cerr << "Coldn't open file " << fileName << endl;
	}

	string addItem = "";
	cout << "Enter item name:" << endl;
	cin >> addItem;

	// Prevent adding '{' '}' symbols.
	while (addItem.find("{") != string::npos || addItem.find("}") != string::npos) {
		cout << "Please enter name with no '{' or '}' symbols." << endl;
		addItem.erase();
		cin >> addItem;
	}

	vector<string> *fileStrings = new vector<string>();
	do {
		// Find end of list.
		string s;
		getline(input, s);
		size_t end = s.find("}");
		size_t begin = s.find("{");
		// Find corresponding category.
		string currCat = s.substr(0, begin);
		if (currCat != category) {
			fileStrings->push_back(s);
			continue;
		}
		size_t size = end - begin;
		s.replace(end, 1, (size == 1 ? "" : " ") + addItem + "}");
		fileStrings->push_back(s);
	} while (input.good());
	input.close();

	// Write file.
	ofstream output(fileName.c_str());
	if (!output) {
		cerr << "Coldn't open file " << fileName << endl;
	}
	for (str_iter idx = fileStrings->begin(); idx != fileStrings->end(); ++idx) {
		output << *idx << endl;
	}
	output.close();
	delete fileStrings;
}

void getCategoryItems(const string &fileName, const string &category, vector<string> &items)
{
	ifstream input(fileName.c_str());
	if (!input) {
		cerr << "Coldn't open file " << fileName << endl;
	}

	do {
		string s;
		getline(input, s);
		size_t begin = s.find("{") + 1;
		if (begin <= 1) { continue; }
		string currCat = s.substr(0, begin - 1);

		// If not considering category or "" (any) category move forward.
		if (category != "" && currCat != category) { continue; }
		size_t size = s.find("}") - begin;
		if (size <= 0) { continue; }

		// Parse items string.
		string itemsString = s.substr(begin, size);
		size_t last = size - 1;
		string item = "";
		for (size_t i = 0; i < size; ++i) {
			if (itemsString[i] == ' ' || i == last) {
				if (i == last) { item.insert(item.size(), 1, itemsString[i]); }
				items.push_back(item);
				item.clear();
			} else {
				item.insert(item.size(), 1, itemsString[i]);
			}
		}
	} while (input.good());
}

void removeTodoItem(const string &fileName, const string &category)
{
	ifstream input(fileName.c_str());

	if (!input) {
		cerr << "Coldn't open file " << fileName << endl;
	}

	string removeItem = "";
	cin >> removeItem;
	// Prevent adding '{' '}' symbols.
	while (removeItem.find("{") != string::npos || removeItem.find("}") != string::npos) {
		cout << "Please enter name with no '{' or '}' symbols." << endl;
		removeItem.erase();
		cin >> removeItem;
	}

	vector<string> *fileStrings = new vector<string>();
	do {
		// Find end of list.
		string s;
		getline(input, s);
		size_t end = s.find("}");
		size_t begin = s.find("{");
		string currCat = s.substr(0, begin);
		size_t pos = s.find(removeItem);
		if (pos == string::npos) {
			fileStrings->push_back(s);
			continue;
		}
		// Check, is current word fullength equal to considerred.
		bool wholeWord = false;
		size_t next = pos + removeItem.size();
		if (next <= s.size()) {
			if (s[next] != ' ' && s[next] != '}') {
				fileStrings->push_back(s);
				continue;
			}
		}

		// Find corresponding category.
		if (wholeWord && (currCat != category || pos < 0)) {
			fileStrings->push_back(s);
			continue;
		}

		s.replace(pos, removeItem.size(), "");
		size_t tmp = string::npos;
		while ((tmp = s.find("  ")) != string::npos) {
			s.replace(tmp, 2, " ");
		}
		tmp = string::npos;
		while ((tmp = s.find(" }")) != string::npos) {
			s.replace(tmp, 2, "}");
		}
		tmp = string::npos;
		while ((tmp = s.find("{ ")) != string::npos) {
			s.replace(tmp, 2, "{");
		}

		fileStrings->push_back(s);
	} while (input.good());
	input.close();

	// Write file.
	ofstream output(fileName.c_str());
	if (!output) {
		cerr << "Coldn't open file " << fileName << endl;
	}
	for (str_iter idx = fileStrings->begin(); idx != fileStrings->end(); ++idx) {
		output << *idx << endl;
	}
	output.close();
	delete fileStrings;
}

void showCategory(const string &fileName, const string &category)
{
	ifstream input(fileName.c_str());

	if (!input) { cerr << "Coldn't open file " << fileName << endl; }

	do {
		string s;
		getline(input, s);
		size_t pos = string::npos;	

		if ((pos = s.find("{")) != string::npos) { cout << s.substr(0, pos) << endl; }
	} while (input.good());
}
