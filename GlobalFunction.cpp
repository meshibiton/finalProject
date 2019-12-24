
// Created by meshi on 24/12/2019.

#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include<unordered_map>
#include "Interpreter.h"
#include "Command.h"
#include <algorithm>

using namespace std;

namespace Global_Functions {

    //our global maps:
    unordered_map<string, Command *> command_table;
    unordered_map<string, Var *> symbolTable;


//remove right space-------------------------
    string TrimRight(std::string &str) {
        str.erase(str.find_last_not_of(' ') + 1);
        return str;
    }


//remove prefixing space--------------------------------------------
    string TrimLeft(const std::string &str) {
        auto pos = str.find_first_not_of(' ');
        return str.substr(pos != std::string::npos ? pos : 0);
    }


//create the commend table--------------------------------------------
    void creatMapCommend() {
        command_table["openDataServer"] = new OpenSeverCommand();
        command_table["connectControlClient"] = new ConnectCommand();
        command_table["if"] = new ConditionParser();
        command_table["while"] = new ConditionParser();
        command_table["var"] = new DefineVarCommand();
        command_table["Print"] = new PrintCommand();
        command_table["Sleep"] = new SleepCommand();
    }


//remove space from the end--------------------------------------------

    string earseChar(string string1, string chars) {
        for (char c: chars) {
            string1.erase(std::remove(string1.begin(), string1.end(), c), string1.end());
            return string1;
        }

    }

//print the vector--------------------------------------------
    void printVector(vector<string> tokens) {
        // Printing the token vector
        for (int i = 0; i < tokens.size(); i++)
            //    cout << tokens[i] + ' ';
            cout << tokens[i] + ' ';
    }


//create vector by split by the ' ' char--------------------------------------------

    vector<string> lineVector(string line, char char1) {
        // Vector of string to save tokens
        vector<string> tokens;
        stringstream check1(line);
        string intermediate;
        // Tokenizing w.r.t. space ' '
        while (getline(check1, intermediate, char1)) {
            tokens.push_back(intermediate);
        }
        return tokens;
    }

//implement the execute of each line--------------------------------------------

    void parser(vector<string> vectorLexer) {
        vector<string>::iterator it1;
        string tempString;
        int index = 0;
        int oldVectorSize = vectorLexer.size();
        for (int i = 0; i < oldVectorSize;) {
            tempString = vectorLexer[0];

            //when to delete the first object before sending
            if (tempString == "connectControlClient" || tempString == "openDataServer" || tempString == "var"
                || tempString == "Sleep" || tempString == "Print") {
                vectorLexer.erase(vectorLexer.begin());
                i = i + 1;

            }
            it1 = vectorLexer.begin();
            // if that  var  already in the map
            //ex:  warp = 32000
            if (symbolTable.count(tempString) > 0) {
                index = command_table.at("var")->execute(vectorLexer);
                vectorLexer.erase(it1, it1 + index);
                i = i + index;
            } else {
                index = command_table.at(tempString)->execute(vectorLexer);
                vectorLexer.erase(it1, it1 + index);
                i = i + index;
            }

        }
    }


//create tokens -vector of tokens--------------------------------------------

    vector<string> lexer() {
        creatMapCommend();
        //read from file
        string line;
        vector<string> lexerVactor;
        ifstream myfile("fly.txt");
        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                vector<string> tempResultVector;
                //check if the prefix is openDataServer
                //example:  openDataServer(5400)
                line = earseChar(line, "\t");
                line = TrimLeft(line);

                if (line.rfind("openDataServer", 0) == 0) {
                    line = earseChar(line, ")");
                    replace(line.begin(), line.end(), '(', ' ');
                    tempResultVector = lineVector(line, ' ');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());


                } else
                    //check if the prefix is connectControlClient
                    //example:   connectControlClient("127.0.0.1",5402)
                if (line.rfind("connectControlClient", 0) == 0) {
                    line = earseChar(line, ")");
                    line = earseChar(line, "\"");
                    replace(line.begin(), line.end(), '(', ' ');
                    replace(line.begin(), line.end(), ',', ' ');
                    tempResultVector = lineVector(line, ' ');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                } else
                    //check if the prefix is var
                if (line.rfind("var", 0) == 0) {
                    //example:    var warp = 0 +4
                    if (line.find('=') != std::string::npos) {
                        tempResultVector = lineVector(line, '=');
                        lexerVactor.push_back(lineVector(line, ' ')[0]);
                        lexerVactor.push_back(lineVector(line, ' ')[1]);
                        lexerVactor.push_back("=");
                        lexerVactor.push_back(tempResultVector[1]);
                        //ex:    var warp -> sim("/sim/time/warp")
                    } else {
                        line = earseChar(line, ")");
                        line = earseChar(line, "\"");
                        replace(line.begin(), line.end(), '(', ' ');
                        tempResultVector = lineVector(line, ' ');
                        lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                    }
                } else
                    //check if the prefix is while
                    //ex:  while rpm <= 750 {
                if (line.rfind("while", 0) == 0) {
                    string condition = "";
                    line = earseChar(line, ")");
                    replace(line.begin(), line.end(), '(', ' ');
                    tempResultVector = lineVector(line, ' ');

                    for (int i = 1; tempResultVector.size(); i++) {
                        if (tempResultVector[i] == "{") {
                            break;
                        } else {
                            condition = condition + tempResultVector[i];
                        }
                    }
                    lexerVactor.push_back("while");
                    lexerVactor.push_back(condition);
                    lexerVactor.push_back("{");
                } else
                    //	check if the prefix is Print
                    //ex:  Print(rpm)
                if (line.rfind("Print", 0) == 0) {
                    line = earseChar(line, ")");
//                line = earseChar(line, "\"");
                    tempResultVector = lineVector(line, '(');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                } else
                    //check if the prefix is while
                    //example: if (a == b) {
                if (line.rfind("if", 0) == 0) {

                    replace(line.begin(), line.end(), '(', ' ');
                    replace(line.begin(), line.end(), ')', ' ');
                    tempResultVector = lineVector(line, '=');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                } else
                    //ex: primer = 3, 	aileron = -roll / 70
                    //ex: if line contain '='
                if (line.find('=') != std::string::npos) {
//                replace(line.begin(), line.end(), '(', ' ');
//                replace(line.begin(), line.end(), ')', ' ');
                    tempResultVector = lineVector(line, '=');
                    lexerVactor.push_back(tempResultVector[0]);
                    lexerVactor.push_back("=");
                    lexerVactor.push_back(tempResultVector[1]);

                }
//            all the other line
//           ex: Print(rpm)
                else {
                    replace(line.begin(), line.end(), '(', ' ');
                    line = earseChar(line, ")");
                    tempResultVector = lineVector(line, ' ');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                }
            }
            //remove the spaces from the and the beginning
            for (int i = 0; i < lexerVactor.size(); i++) {
                lexerVactor[i] = TrimLeft(lexerVactor[i]);
                lexerVactor[i] = TrimRight(lexerVactor[i]);
            }
            myfile.close();
            return lexerVactor;
        } else {
            cout << "Unable to open file" << endl;
        }
    }


//return the result of expression--------------------------------------------

    double interpreter(string expression) {
        Interpreter *i = new Interpreter();
        Expression *e = nullptr;
        try {
            e = i->interpret(expression);
            double result = e->calculate();
            std::cout << "6: " << result << std::endl;//-10
            delete e;
            delete i;
            return result;
        } catch (const char *e) {
            if (e != nullptr) {
                delete e;
            }
            if (i != nullptr) {
                delete i;
            }
        }
    }
}