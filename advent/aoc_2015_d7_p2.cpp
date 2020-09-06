#include <iostream>
#include <map>
#include <sstream>
#include <list>
#include <algorithm>

std::map<std::string, unsigned short> values;

class Command {
public:
    Command(std::string op, std::string first, std::string second, std::string result)
        : op(op)
        , first(first)
        , second(second)
        , result(result) {
    }

    void print() const {
        std::cout << op;

        if (!first.empty()) {
            std::cout << " " << first;
        }

        if (!second.empty()) {
            std::cout << " " << second;
        }

        std::cout << " -> " << result << std::endl;
    }

    bool compute() const {
        if (op == "NOT") {
            if (values.find(first) != values.end()) {
                values[result] = ~values[first];
                return true;
            }
        } else if (op == "AND") {
            if (values.find(first) != values.end() && values.find(second) != values.end()) {
                values[result] = values[first] & values[second];
                return true;
            }
        } else if (op == "OR") {
            if (values.find(first) != values.end() && values.find(second) != values.end()) {
                values[result] = values[first] | values[second];
                return true;
            }
        } else if (op == "LSHIFT") {
            if (values.find(first) != values.end()) {
                std::stringstream stream;
                stream << second;
                int amount;
                stream >> amount;

                values[result] = values[first] << amount;
                return true;
            }
        } else if (op == "RSHIFT") {
            if (values.find(first) != values.end()) {
                std::stringstream stream;
                stream << second;
                int amount;
                stream >> amount;

                values[result] = values[first] >> amount;
                return true;
            }
        } else {
            std::stringstream stream;
            stream << first;

            std::string label;
            stream >> label;
            stream.clear();

            if (values.find(label) != values.end()) {
                values[result] = values[label];
                return true;
            } else {
                stream << first;
                unsigned short amount;
                stream >> amount;

                if (amount != 0) {
                    values[result] = amount;
                    return true;
                }
            }
        }

        return false;
    }

public:
    std::string op;
    std::string first;
    std::string second;
    std::string result;
};

int main(int argc, char* argv[])
{

    std::list<Command> commands;
    values["1"] = 1;
    values["0"] = 0;
    std::string input;
    while (std::getline(std::cin, input)) {
        std::stringstream stream;

        if (input.find("->") == std::string::npos) {
            return 1;
        }

        std::string command(input.substr(0, input.find("->")));

        std::string result(input.substr(input.find("->") + 2));
        stream << result;
        stream >> result;
        stream.clear();

        if (command.find("AND") != std::string::npos) {
            stream << command.substr(0, command.find("AND")) << " "
                   << command.substr(command.find("AND") + 3);
            std::string first;
            std::string second;
            stream >> first >> second;

            commands.push_back(Command("AND", first, second, result));
        } else if (command.find("LSHIFT") != std::string::npos) {
            stream << command.substr(0, command.find("LSHIFT")) << " "
                   << command.substr(command.find("LSHIFT") + 6);

            std::string first;
            std::string second;
            stream >> first >> second;

            commands.push_back(Command("LSHIFT", first, second, result));
        } else if (command.find("RSHIFT") != std::string::npos) {
            stream << command.substr(0, command.find("RSHIFT")) << " "
                   << command.substr(command.find("RSHIFT") + 6);

            std::string first;
            std::string second;
            stream >> first >> second;

            commands.push_back(Command("RSHIFT", first, second, result));
        } else if (command.find("NOT") != std::string::npos) {
            stream << command.substr(command.find("NOT") + 3);

            std::string first;
            stream >> first;

            commands.push_back(Command("NOT", first, "", result));
        } else if (command.find("OR") != std::string::npos) {
            stream << command.substr(0, command.find("OR")) << " "
                   << command.substr(command.find("OR") + 2);

            std::string first;
            std::string second;
            stream >> first >> second;

            commands.push_back(Command("OR", first, second, result));
        } else {
            stream << command;
            std::string first;
            stream >> first;
            commands.push_back(Command("", first, "", result));
        }
    }

    std::list<Command> orig_commands = commands;

    while (!commands.empty()) {
        Command cmd = commands.front();
        commands.pop_front();

        if (!cmd.compute()) {
            commands.push_back(cmd);
        }
    }

    std::stringstream stream;
    stream << values["a"];
    std::string a_val;
    stream >> a_val;

    values.clear();
    values["1"] = 1;
    values["0"] = 0;

    while (!orig_commands.empty()) {
        Command cmd = orig_commands.front();
        orig_commands.pop_front();

        if (cmd.result == "b") {
            cmd.first = a_val;
        }

        if (!cmd.compute()) {
            orig_commands.push_back(cmd);
        }
    }

    std::cout << "a: " << values["a"] << std::endl;
    return 0;
}
