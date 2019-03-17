#ifndef SRC_PGREP_HPP
#define SRC_PGREP_HPP
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono> 
#include <iomanip>
#include <experimental/filesystem>

namespace pgrep
{
    class File
    {
        public:
            File(std::string name,int fileBytes);
            std::string getName();
            std::string getFileBytes();
            int getWordN();
            void setWordN(int n);
            static bool compare(File*,File*);
        private:
            std::string name;
            std::string fileBytes;
            int wordN;
    };
	//Recebe uma string qualquer e uma palavra para verificar o número da palavra nessa string
	int getWordOccurence(const std::string& text,const std::string& word);
	//Recebe uma string com o nome do arquivo e uma palavra para verificar o número da palavra nesse arquivo
	int getFileWordOcurrence(const std::string& file_name,const std::string& word);
    //Retorna os arquivos mapeados pelo nome
	std::map<std::string,File*> getFilesMap();
    void init(std::string word,int seconds);
    //Função Operária;
    void worker(std::string word, File* file);
    //Função Ranking
    void ranking(std::map<std::string,File*>& fileMap);
    //Função Despachante
    void callWorkers(std::queue<File*>& fileQueue,std::string);
}
#endif