#include "config.h"

bool only_enemies;
bool aimbot;
bool kernel;
int fov;
int speed;
int smooth;
bool bhop;

void ConfigRead() 
{
    only_enemies = stringToBool(config("only-enemies"));
    kernel = stringToBool(config("kernel-driver"));

    aimbot = stringToBool(config("aimbot"));
    fov = std::stoi(config("aimbot-fov"));
    speed = std::stoi(config("aimbot-speed"));
    smooth = std::stoi(config("aimbot-smooth"));

    bhop = stringToBool(config("bunnyhop"));

}

void createConfigFile() {
    std::ofstream configFile("config.ini");
    if (configFile.is_open()) {
        configFile << "[Settings]\n";
        configFile << "kernel-driver=false\n";
        configFile << "only-enemies=true    # works only on default players models\n\n";
        configFile << "[Aimbot]\n";
        configFile << "aimbot=true\n";
        configFile << "aimbot-fov=100\n";
        configFile << "aimbot-speed=3\n";
        configFile << "aimbot-smooth=3\n\n";
        configFile << "[Bunnyhop]\n";
        configFile << "bunnyhop=true";
        configFile.close();
        std::cout << "[Z3BRA] Config file created successfully.\n";
    }
    else {
        std::cerr << "Unable to create config file.\n";
    }
}

std::string config(std::string param)
{
    std::ifstream cFile("config.ini");
    if (cFile.is_open())
    {
        std::string line;
        while (getline(cFile, line)) {
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                line.end());
            if (line[0] == '[' || line[0] == '#' || line.empty())
                continue;

            auto commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line.erase(commentPos);
            }

            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            if (name == param) {
                return value;
            }
        }

    }
    else {
        std::cout << "[Z3BRA] Didnt find config file, creating one...\n";

        createConfigFile();

        std::cout << "[Z3BRA] Please restart cheat.\n";

        system("pause");
        exit(0);
    }
}