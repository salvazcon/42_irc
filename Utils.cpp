#include "Utils.hpp"

std::vector<std::string> ft_split(const char* str, char delimiter) 
{
    std::vector<std::string> result;
    std::string current;
    
    for (const char* it = str; *it != '\0'; ++it) 
    {
        if (*it == delimiter)
        {
            if (!current.empty()) 
            {
                result.push_back(current);
                current.clear();
            }
        } 
        else
            current += *it;
    }
    if (!current.empty())
        result.push_back(current);
    return result;
}

std::string ft_normalize_spaces(const std::string& input) 
{
    if (input.empty())
        return "";

    std::string result = input;

    size_t start = result.find_first_not_of(' ');
    if (start != std::string::npos)
        result.erase(0, start);
    size_t end = result.find_last_not_of(' ');
    if (end != std::string::npos)
        result.erase(end + 1);
    size_t pos = 0;
    while ((pos = result.find(' ', pos)) != std::string::npos) 
    {
        size_t next_non_space = result.find_first_not_of(' ', pos);
        if (next_non_space != pos + 1) {
            result.erase(pos + 1, next_non_space - pos - 1);
        }
        pos = pos + 1;
    }
    return result;
}