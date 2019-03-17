#include <string>
#include "../src/pgrep.hpp"
using namespace std;
int main(int argc, char *argv[])
{
    if(argc == 2) //SE PASSAR 2 ARGS O TEMPO PARA NOVA VERIFICAÇÃO É 5s
	{
		string word = argv[1];
        pgrep::init(word,5);
	}
    else if( argc == 3) //SE PASSAR 3 ARGS O 3º ARG É O TEMPO EM SECS DE ESPERA PARA NOVA VERIFICAÇÃO
    {
        string word = argv[1];
        int secs = stoi(argv[2]);
        pgrep::init(word,secs);
    }
    else
        cout << "Número de argumentos invalidos\n";
}
