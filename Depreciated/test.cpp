#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ios>

std::vector<std::string> v_fileRead;
std::string inp;

void fileRead(std::string fileName)

{

    std::ifstream fileStreamRead(fileName);

    while (std::getline(fileStreamRead, inp))

    {

        v_fileRead.push_back(inp);
    }

    fileStreamRead.close();
}

void write()

{

    std::cout << " Enter filename to write to ";

    std::getline(std::cin, inp);

    std::ofstream writeFile(inp);

    for (std::string i : v_fileRead)

    {
        writeFile << i << "\n";
    }

    writeFile.close();
}

int main()
{
    std::ifstream f("x.png", std::ios::binary);

    std::cout << " enter name of file to write to " << std::endl;
    std::getline(std::cin, inp);

    std::ofstream writeFile(inp);


    
    char c;
    int length = 0;
    while (f.get(c))
    {
        length++;
    }

    f.close();

    std::ifstream fe("x.png", std::ios::binary | std::ios::in);

    char *filedat = new char[length];

    for (int i = 0; i < length - 1;)
    {
        while (fe.get(c))
        {
            filedat[i] = c;
            std::cout << c;
            i++;
        }
    }

    fe.close();

    for (int i = 0; i < length; i++)
    {
        writeFile << filedat[i];
    }

    writeFile.close();

    return 0;
}
