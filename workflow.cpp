#include "workflow.h"

map<string, int> dict = {
        { "readfile", 1 },
        { "writefile", 2 },
        { "grep", 3 },
        { "sort", 4 },
        { "replace", 5 },
        { "dump", 6 }
};
map<int, pair<int, string>> com;
vector<int> order;


void Config::read_desc() {
    string str;
    block temp;
    getline(file, str);
    if(str!="desc"){
        while(str!="desc" && file){
            getline(file, str);
        }
    }
    getline(file, str);
    while(str != "csed"){
        if (str.size()>1) {
            temp = get_block_from_str(str);
            com.insert(make_pair(temp.id, make_pair(temp.blocks_name, temp.args)));
        }
        getline(file, str);
    }
}

Config::block Config::get_block_from_str(string str) {
    block temp;
    int i = 0;
    string id, block_name, args = "";
    for (; str[i]!=' '; i++){
        id += str[i];
    }
    i+=3;
    for (; (str[i]!=' ')&&(i<str.size()); i++){
        block_name += str[i];
    }
    i++;
    while ((i<str.size())&&(str[i]==' '))
        i++;
    for (;(i<str.size()); i++){
        args += str[i];
    }
    temp.id = stoi(id);
    temp.blocks_name = dict[block_name];
    temp.args = args;
    return temp;
}

void Config::read_csed() {//not finished yet
    string str;
    string word;
    while (!file.eof()){
        file >> word;
        if(word!="->"){
            //cout<<word;
            order.push_back(stoi(word));
        }
    }
}

Config::Config(const string& input) {
    file.open(input);
}

IWorker *Factory::create_worker(int ind) {
    IWorker *worker = nullptr;
    switch (ind) {
        case 1:
            worker = new Readfile;
            break;
        case 2:
            worker = new Writefile;
            break;
        case 3:
            worker = new Grep;
            break;
        case 4:
            worker = new ::Sort;
            break;
        case 5:
            worker = new ::Replace;
            break;
        case 6:
            worker = new Dump;
            break;
        default:
            throw runtime_error("Wrong block name");
    }
    return worker;
}

Text* Readfile::do_work(Text *text, string *args) {
    string path = *args;
    ifstream file;
    file.open(path, ifstream::in);
    text = new Text;
    if (!file.is_open())
        throw runtime_error("file isn't open: "+path);
    for (string str; getline(file, str); )
    {
        text->push_back(str);
        //cout<<str<<endl;
    }
    return text;
}

Text* IWorker::do_work(Text *text, string *args) {

}

Text* Writefile::do_work(Text *text, string *args) {
    ofstream file;
    file.open(*args);
    if (!file.is_open())
        throw runtime_error("Something went wrong");
    for (const auto& it : *text){
        //cout << it << "\n";
        file << it << "\n";
    }
    file.close();
}

bool Grep::contains(string str, string word) {
    int i, j;
    for (i = 0; i < str.size(); i++){
        if (str[i] == word[0]) {
            for (j = 1; j < word.size(); j++){
                if (i+j>=str.size())
                    return false;
                if (str[i+j]!=word[j])
                    break;
                if(j == word.size()-1)
                    return true;
            }
            i += j;
        }
    }
    return false;
}

Text* Grep::do_work(Text *text, string *args) {
    Text* returns = new Text;
    string word = *args;
    for (string str : *text){
        if(contains(str, word)){
            returns->push_back(str);
            //cout<<str<<"\n";
        }
    }
    return returns;
}

bool Sort::comp(string a, string b) {
    return (a.compare(b) < 0);
}

Text *Sort::do_work(Text *text, string *args) {
    std::sort(text->begin(), text->end(), comp);
    return text;
}

string Replace::find(string str, string word1, string word2) {
    int i, j;
    for (i = 0; i < str.size(); i++){
        if (str[i] == word1[0]) {
            for (j = 1; j < word1.size(); j++){
                if (i+j>=str.size())
                    return str;
                if (str[i+j]!=word1[j])
                    break;
                if(j == word1.size()-1)
                    str = rep(str, i, word1, word2);
            }
        }
    }
    return str;
}


Text* Replace::do_work(Text *text, string *args) {
    string word1, word2;
    Text* out = new Text;
    int i;
    while (args[0][i] == ' ')
        i++;
    for(;(args[0][i]!=' '); i++){
        word1+=args[0][i];
    }
    while (args[0][i] == ' ')
        i++;
    for(;(args[0][i]!=' ')&&(i<args->size()); i++){
        word2+=args[0][i];
    }
    for (auto it : *text){
        out->push_back(find(it, word1, word2));
    }
    return out;
}

string Replace::rep(string str, int start, string& word1, string& word2) {
    string out;
    for (int i = 0; i < start; i++){
        out += str[i];
    }
    for (char i : word2){
        out += i;
    }
    for (int i = start+word1.size(); i < str.size()-word1.size()+word2.size(); i++){
        out += str[i];
    }
    return out;
}

Text *Dump::do_work(Text *text, string *args) {
    ofstream file;
    file.open(*args);
    if (!file.is_open())
        throw runtime_error("Something went wrong");
    for (const auto& it : *text){
        //cout << it << "\n";
        file << it << "\n";
    }
    file.close();
    return text;
}

void WorkFlow::do_work(string path) {
    Config conf(path);
    conf.read_desc();
    conf.read_csed();
    Text *text;
    string args;
    Factory factory;
    IWorker *worker;
    bool flag_read = 0, flag_write = 0;
    for(auto it : order){
        pair<int, string> p = com[it];
        if((flag_read&&(p.first==1))||(flag_write))
            throw runtime_error("file read/write can't be in the middle of the schema.");
        if (p.first==1)
            flag_read = 1;
        if (p.first==2)
            flag_write = 1;
        worker = factory.create_worker(p.first);
        text = worker->do_work(text, &p.second);
    }
}