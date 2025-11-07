#include <iostream>
#include <wchar.h>
#include <vector>
#include <string>
#include "Parser.h"
#include "Scanner.h"
#include "IntermediateCode.h"

using namespace std;

// Variables globales para el parser
string gameType;
vector<string> playerCards;
vector<string> dealerCards;
vector<string> mesaJugadorCards;
vector<string> mesaOponenteCards;

// Instancia global del generador de código
IntermediateCode intermediateCode;

// Función auxiliar para convertir wchar_t* a string
string wcharToString(const wchar_t* wstr) {
    if (!wstr) return "";
    wstring ws(wstr);
    string str(ws.begin(), ws.end());
    return str;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    
    // Limpiar estado
    gameType.clear();
    playerCards.clear();
    dealerCards.clear();
    mesaJugadorCards.clear();
    mesaOponenteCards.clear();
    intermediateCode.clear();
    
    // Archivo de entrada
    wchar_t* fileName = coco_string_create(L"test.card");
    
    Scanner* scanner = new Scanner(fileName);
    Parser* parser = new Parser(scanner);
    
    parser->Parse();
    
    if (parser->errors->count == 0) {
        intermediateCode.saveToFile("output.cpp");
        return 0;
    } else {
        std::wcerr << L"✗ " << parser->errors->count << L" error(es) encontrado(s)\n";
        std::wcerr << L"ERROR DE COMPILACIÓN: verifica test.card\n";
        return 1;
    }
    
    coco_string_delete(fileName);
    delete parser;
    delete scanner;
    
    return 0;
}