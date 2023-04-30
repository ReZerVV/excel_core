#include <stddef.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <bits/stdc++.h>

enum token_type : uint8_t {
    null,
    number,
    string,
    address,
    expression,
};

std::string token_type_to_string(const token_type type) {
    switch (type) {
        case token_type::null: return "null";
        case token_type::number: return "number";
        case token_type::string: return "string";
        case token_type::address: return "address";
        case token_type::expression: return "expression";
        default: return "none";
    }
} 

class token {
public:
    token()
        :
        value(std::string{ "NULL" }),
        type(token_type::null) {}
    explicit token(const std::string &lexeme)
        : 
        value(lexeme),
        type(define_token_type(lexeme)) {}
private:
    static token_type define_token_type(const std::string &lexeme) {
        if (is_empty(lexeme)) return token_type::null;
        if (is_address(lexeme)) return token_type::address;
        if (is_expression(lexeme)) return token_type::expression;
        if (is_number(lexeme)) return token_type::number;
        return token_type::string;
    }
public:
    static bool is_empty(const std::string &lexeme) {
        return lexeme.empty();
    }
    static bool is_address(const std::string &lexeme) {
        return lexeme.front() == ':' && lexeme.find(";") != std::string::npos;
    }
    static bool is_expression(const std::string &lexeme) {
        return lexeme.front() == '=';
    }
    static bool is_number(const std::string &lexeme) {
        for (auto it = lexeme.begin(); it != lexeme.end(); ++it) {
            if (!isdigit(*it) && *it != '.') {
                return false;
            }
        }
        return true;
    }
    static bool is_string(const std::string &lexeme) {
        return (lexeme.front() == '"' && lexeme.back() == '"') || (lexeme.front() == '\'' && lexeme.back() == '\''); 
    }
public:
    std::string value;
    token_type  type;
};

class lexer {
public:
    lexer(const std::string &source)
        :
        _index(0),
        _source(source) {}
public:
    bool is_end() const { 
        return _index >= _source.size() || _source[_index] == '\r' || _source[_index] == '\0'; 
    }
    token next_token() {
        skip();

        std::string lexeme{  };
        char sym_end = (char)0;
        
        if (_source[_index] == '"') { sym_end = '\"'; advance(); }
        else sym_end = ',';

        while (!is_end() && _source[_index] != sym_end) {
            lexeme += _source[_index];
            advance();
        }
        
        if (_source[_index] == '"') advance();
        advance();

        return token{ lexeme };
    }
private:
    void advance() {
        if (!is_end()) {
            _index += 1;
        }
    }
    void skip() {
        if (_source[_index] == '#') {
            while (!is_end() && _source[_index] != '\n') {
                advance();
            }
        }
        while (isspace(_source[_index])) {
            advance();
        }
    }
private:
    std::string _source;
    size_t      _index;
};

class table {
public:
    friend void table_print(const table t);
public:
    table(const std::string &file_path) {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "File is not found." << std::endl;
            return;
        }
        
        size_t cols = 0;
        std::string line{  };
        while (getline(file, line)) {
            if (line.front() == '#') continue;
            _buffer.push_back(std::vector<token>{  });
            lexer _lexer{line};
            while (!_lexer.is_end()) {
                _buffer[cols].push_back(_lexer.next_token());
            }
            cols += 1;
        }
        
        file.close();

        for (size_t y = 0; y < _buffer.size(); ++y) {
            for (size_t x = 0; x < _buffer[y].size(); ++x) {
                if (_buffer[y][x].type == token_type::address) {
                    _buffer[y][x] = get_cell_from_address(_buffer[y][x].value);
                }
            }
        }
    }
private:
    token get_cell_from_address(const std::string& lexeme) {
        size_t index = lexeme.find(";");

        std::string str_x = lexeme.substr(1).substr(0, index-1);
        if (!token::is_number(str_x)) {
            std::cout << "[x] address can only be a number" << std::endl;
            return token{  };
        }
        size_t x = std::stoi(str_x);

        std::string str_y = lexeme.substr(index+1);
        if (!token::is_number(str_y)) {
            std::cout << "[y] address can only be a number" << std::endl;
            return token{  };
        }
        size_t y = std::stoi(str_y);
        
        if (x >= 0 && x < _buffer.size() && 
            y >= 0 && y < _buffer[0].size()) {
            return _buffer[x][y];
        }
        return token{  };
    }
private:
    std::vector<std::vector<token> >    _buffer;
};

void table_print(const table t) {
    if (t._buffer.empty()) {
        return;
    }
    for (size_t y = 0; y < t._buffer.size(); ++y) {
        for (size_t x = 0; x < t._buffer[y].size(); ++x) {
            size_t max_size = 0;
            for (size_t dy = 0; dy < t._buffer.size(); ++dy) {
                if (t._buffer[dy][x].value.size() > max_size) {
                    max_size = t._buffer[dy][x].value.size();
                }
            }
            std::cout << std::setw(max_size) << std::setfill(' ') << std::left << t._buffer[y][x].value << '|';
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    std::string file_path{"/home/cyril/Programming/projects_c++/mini_excel/input.txt"};
    table t{file_path};
    table_print(t);
    return 0;
}