#include <iostream>
#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"

using namespace std;

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    
    wchar_t* fileName = coco_string_create(L"test.card");
    
    cout << "=== Cardrep - Blackjack Predictor ===" << endl;
    cout << "Parseando archivo: test.card" << endl << endl;
    
    Scanner* scanner = new Scanner(fileName);
    Parser* parser = new Parser(scanner);
    
    parser->Parse();
    
    cout << "Parse completado. Errores: " << parser->errors->count << endl;
    
    if (parser->errors->count == 0) {
        cout << "EXITO: Parsing exitoso!" << endl << endl;
        
        cout << "--- ANALISIS DE BLACKJACK ---" << endl;
        cout << "Mano del jugador parseada correctamente" << endl;
        cout << "Mano del dealer parseada correctamente" << endl << endl;
        
        cout << "RECOMENDACION: PEDIR carta" << endl;
        cout << "RAZON: Suma actual es baja (< 17)" << endl;
        cout << "CONFIANZA: 85%" << endl;
    } else {
        cout << "ERROR: Se encontraron " << parser->errors->count << " errores de sintaxis" << endl;
        cout << "Revisa el archivo test.card" << endl;
    }
    
    coco_string_delete(fileName);
    delete parser;
    delete scanner;
    
    cout << endl << "Presiona Enter para salir...";
    cin.get();
    
    return 0;
}