#include <iostream>
#include <fstream>
#include <cassert>

#include "Lexer.hpp"
#include "Parser.hpp"

#include "TypeTable.hpp"

#include "Runner.hpp"



std::string ReadFile(const char* name)
{
    std::ifstream    fin;
    fin.open(name);

    if (!fin)
    {
        std::cout << "file not opened";
        return "";
    }

    std::string temp = "";
    std::string text = "";

    while(getline(fin, temp))
        text += temp + '\n';

    return text;
}

int main()
{
    std::string text = ReadFile("code.txt");

    Lexer lex(text);
    std::vector<Token> tokens = lex.Tokenize();
    for(auto token : tokens)
        printf("%s\n", ToString(token).c_str());

    Parser par(tokens);
    std::vector<Function>& functions = par.Parse();

    for(std::vector<Function>::iterator it = functions.begin(); it != functions.end(); ++it)
    {
        fprintf(stdout, "function %s():\n", it->name.c_str());
        for(auto Const : it->consts)
        {
            fprintf(stdout, "%2x ", Const.first);
            fprintf(stdout, "%p\n", Const.second.data);
        }
        for(size_t i = 0; i < it->size; ++i)
        {
            Command* temp = it->Code + i;
            for(size_t j = 0; j < temp->size; ++j)
            {
                fprintf(stdout, "%2x ", temp->data[j]);
            }
            fprintf(stdout, "\n");
        }
    }


    run(&functions);
    return 0;
}
