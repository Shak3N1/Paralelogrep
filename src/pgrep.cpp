#include "../src/pgrep.hpp"
std::mutex M1;
int WORKERS = 0;
void pgrep::init(std::string word,int seconds)
{
	std::queue<File*> fileQueue; // FILA DE ARQUIVOS
	std::map<std::string,File*> filesMap = getFilesMap();// MAPA COM OS ARQUIVOS DA PASTA FILESET
	std::map<std::string,File*> newMap;
	for (auto const& f : filesMap) //ADD TODOS ARQUIVOS DA LISTA NA FILA
		fileQueue.push(f.second);	
	if(!fileQueue.empty()) //SE A FILA NÃO ESTIVER VAZIA CHAMA AS TRABALHADORAS
		callWorkers(fileQueue,word);
	std::thread (ranking,std::ref(filesMap)).detach(); //CHAMA A THREAD RANKING
	while(true)
	{
		newMap = getFilesMap(); //GERA UM NOVO MAPA COM OS ARQUIVOS ATUAIS DO DIRETORIO
		for (auto const& f : newMap)
		{
			// SE O MAPA JÁ CONTÉM O ARQUIVO
			if (filesMap.count(f.second->getName())){ 
				//VERIFICA SE OS ARQUIVOS SÃO DIFERENTES ATRAVES DO N DE BYTES	
				if (filesMap[f.second->getName()]->getFileBytes() != f.second->getFileBytes())
				{
					//ADD NA FILA, DELETA O OBJETO ANTIGO, E APONTA PARA O NOVO
					fileQueue.push(f.second);
					delete filesMap[f.second->getName()];
					filesMap[f.second->getName()] = f.second;
				}					
			}
			// SE NÃO CONTÉM
			else
			{
			// ADD NA FILA E ADICIONA NO MAPA QUE É PASSADO PARA A THREAD RANKING
				fileQueue.push(f.second);
				filesMap[f.second->getName()] = f.second;
			}				
		}	
		// SE A FILA NÃO ESTIVER VAZIA CHAMA AS WORKERS E A RANKING
		if(!fileQueue.empty())
		{
			callWorkers(fileQueue,word);
			std::thread (ranking,std::ref(filesMap)).detach();
		}
		std::this_thread::sleep_for(std::chrono::seconds(seconds));
	}
}
void pgrep::callWorkers(std::queue<File*>& fileQueue,std::string word)
{

	while(!fileQueue.empty()) // ENQTO EXISTIR ARQUIVO NA FILA
	{
		if (WORKERS < 10)
		{
			std::thread (worker,word,fileQueue.front()).detach();//CRIA UMA THREAD QUE AO TERMINAR 
														//O TRABALHO FINALIZA DE UMA MANEIRA SEGURA
			fileQueue.pop();//TIRA ARQUIVO DA FILA
			M1.lock(); //ENTRA NA ZONA CRITICA M1
			WORKERS++;
			M1.unlock(); //SAI DA ZONA CRITICA M1
		}	
	}
	while (WORKERS>0) 
	{				
		//ESPERA AS TRABALHADORES PARAREM PARA CHAMAR A FUNÇÃO RANKING
	}
}
void pgrep::worker(std::string word, File* file)
{
	file->setWordN(getFileWordOcurrence(file->getName(),word));//SETA O N DE PALAVRAS DO ARQUIVO
	M1.lock(); //ENTRA NA ZONA CRITICA M1
	WORKERS--;
	M1.unlock(); //SAI DA ZONA CRITICA M1
}

void pgrep::ranking(std::map<std::string,File*>& filesMap)
{
	std::vector<File*> fileList;
	File* top10[filesMap.size()-1];
	system("clear");
	for (auto i : filesMap)
	{
		fileList.push_back(i.second); //ADICIONAR TODOS ITENS DO MAPA NO VETOR
	}
	std::cout << std::left;
	std::cout << "	  > TOP10 arquivos com mais ocorrências < \n\n";	
		std::cout << std::setw(25) << "[RANKING]" << std::setw(25) <<"[NOME]"
																		<< "[OCORRÊNCIAS]\n";
	//ORDENA O VETOR 
	std::sort(fileList.begin(),fileList.end(),[](File* file,File* file2){
												return (file->getWordN() > file2->getWordN());
												}); 

	int i = 0;
	while(i<10 && i < filesMap.size()) //ATRIBUI O TOP10
	{
		top10[i] = fileList.at(i);
		i++;
	}
	i = 0;
	while(i<10 && i < filesMap.size()) //PRINTA O TOP10
	{
		std::cout << "   "<< std::setw(7) << i+1 << std::setw(45) << top10[i]->getName() << std::setw(20) 
											<< top10[i]->getWordN() << "\n";
		i++;
	}
}
pgrep::File::File(std::string name,int fileBytes)
{
    this->name = name;
    this->fileBytes = fileBytes;
	this->wordN = 0;
}
std::string pgrep::File::getFileBytes()
{
    return this->fileBytes;
}
std::string pgrep::File::getName()
{
    return this->name;
}
void pgrep::File::setWordN(int n)
{
    this->wordN = n;
}
int pgrep::File::getWordN()
{
    return this->wordN;
}
bool pgrep::File::compare(File* i,File* j)
{
	return(i->getWordN() < j->getWordN());
}
int pgrep::getWordOccurence(const std::string& text,const std::string& word)
{
	std::size_t found = text.find(word);
	int n = 0;
	while (found!=std::string::npos)//Até não achar mais palavras
	{
		n++;
		found = text.find(word,found+word.length());
	}	
	return n;
}
int pgrep::getFileWordOcurrence(const std::string& file_name,const std::string& word)
{
	int n = 0;
	std::string str;
	std::ifstream input_stream;
	input_stream.open(file_name.c_str()); 
	while (!input_stream.eof()) //Enquanto não chegar ao final do arquivo
	{
		getline(input_stream,str);
		n+=getWordOccurence(str,word);
	}
	return n;
}
std::map<std::string,pgrep::File*> pgrep::getFilesMap()
{
	std::map<std::string,File*> fileMap;//Mapa de ponteiros para objetos da classe File
	std::string name;
	int fileBytes = 0 ;
    for (const auto p : std::experimental::filesystem::directory_iterator("fileset"))
	{
		name = p.path().string();
		fileBytes = std::experimental::filesystem::file_size(p);
		fileMap[name] = new File(name,fileBytes);
	}
	return fileMap;
}

