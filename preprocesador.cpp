#include "preprocesador.h"
#include <sstream>
#include <unordered_set>

const std::unordered_set<std::string> stopwords = {
    "a", "an", "the", "and", "or", "but", "in", "on", "at", "to", "for",
    "of", "with", "by", "from", "is", "are", "was", "were", "be", "been",
    "being", "have", "has", "had", "do", "does", "did", "will", "would",
    "could", "should", "may", "might", "can", "shall", "you", "he", "she",
    "it", "we", "they", "his", "her", "its", "our", "their", "this", "that",
    "these", "those", "not", "no", "nor", "so", "if", "then", "than",
    "too", "just", "about", "also", "very", "s", "t", "ll", "ve", "re"
};

std::string normalizar(const std::string &txt) {
    std::string normalizado;
    normalizado.reserve(txt.size());
    bool dentro_de_corchete = false;

    for (size_t i = 0; i < txt.length(); ++i) {
        unsigned char c = txt[i];

        if (c == '[') {
            dentro_de_corchete = true;
        }
        else if (c == ']') {
            dentro_de_corchete = false;
        }
        else if (dentro_de_corchete) {
            continue;
        }
        else if (c >= 0xC0) {
            if (i + 1 < txt.length()) {
                unsigned char next_c = txt[i + 1];
                if ((c == 0xC3 && next_c == 0xA1) || (c == 0xC3 && next_c == 0x81)) { normalizado += 'a'; i++; }
                else if ((c == 0xC3 && next_c == 0xA9) || (c == 0xC3 && next_c == 0x89)) { normalizado += 'e'; i++; }
                else if ((c == 0xC3 && next_c == 0xAD) || (c == 0xC3 && next_c == 0x8D)) { normalizado += 'i'; i++; }
                else if ((c == 0xC3 && next_c == 0xB3) || (c == 0xC3 && next_c == 0x93)) { normalizado += 'o'; i++; }
                else if ((c == 0xC3 && next_c == 0xBA) || (c == 0xC3 && next_c == 0x9A)) { normalizado += 'u'; i++; }
                else if ((c == 0xC3 && next_c == 0xB1) || (c == 0xC3 && next_c == 0x91)) { normalizado += "ni"; i++; }
            }
        }
        else {
            if (std::isalnum(c)) {
                normalizado += std::tolower(c);
            }
            else {
                normalizado += ' ';
            }
        }
    }
    return normalizado;
}

std::vector<std::string> tokenizar(const std::string& txt_normalizado) {
    std::vector<std::string> tokens;
    std::stringstream ss(txt_normalizado);
    std::string palabra;

    while (ss >> palabra) {
        if (palabra.length() > 1 && stopwords.find(palabra) == stopwords.end()) {
            tokens.push_back(palabra);
        }
    }
    return tokens;
}
