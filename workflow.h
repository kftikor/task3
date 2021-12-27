//
// Created by Katherine on 27.12.2021.
//

#ifndef TASK3_WORKFLOW_H
#define TASK3_WORKFLOW_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <ostream>
#include <algorithm>


using namespace std;

typedef vector<string> Text;


class Config{
private:
    ifstream file;

    struct block{
        int id;
        int blocks_name;
        string args;
    };
    block get_block_from_str(string str);
public:
    Config(const string& input);
    void read_desc();
    void read_csed();
    int return_count();
};

class IWorker{
protected:
    //static vector<string> text;
public:
    virtual Text* do_work(Text *text, string *args);
};

class Readfile : public IWorker{
private:
    //vector<string>* argparse (string *args);
public:
    Text* do_work(Text *text, string *args) override;
};

class Writefile : public IWorker{

public:
    Text* do_work(Text *text, string *args) override;
};

class Grep : public IWorker{
private:
    bool contains(string str, string word);
public:
    Text* do_work(Text *text, string *args) override;
};

class Sort : public IWorker{
private:
    static bool comp(string a, string b);
public:
    Text* do_work(Text *text, string *args) override;
};

class Replace : public IWorker{
private:
    string find(string str, string word1, string word2);
    string rep(string str, int start, string& word1, string& word2);
public:
    Text* do_work(Text *text, string *args) override;
};

class Dump : public IWorker{

public:
    Text* do_work(Text *text, string *args) override;
};

class Factory{
public:
    IWorker* create_worker(int ind);
};

class WorkFlow{
public:
    void do_work(string path);
};
#endif //TASK3_WORKFLOW_H
