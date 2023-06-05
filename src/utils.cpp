#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>

static std::unordered_map<std::string, std::string> parseEnvFile(const std::string& fileName) {
    std::unordered_map<std::string, std::string> envVariables;
    std::ifstream file(fileName);
    std::istringstream stream;
    std::string line, key, value;

    while (std::getline(file, line)) {
        stream.str(line);
        if (std::getline(stream, key, '=') && std::getline(stream, value)) {
            envVariables[key] = value;
        }
        stream.clear();
    }
    return envVariables;
}

static int envStrToInt(const std::string& str) {
    try {
        return std::stoi(str);
    }
    catch (const std::invalid_argument& err) {
        throw std::runtime_error("Cannot convert " + str + " to an integer\n");
    }
}