#include <iostream>
#include <wchar.h>
#include <vector>
#include <string>
#include "Parser.h"
#include "Scanner.h"

using namespace std;

// Definir las variables globales
string gameType;
vector<string> playerCards;
vector<string> dealerCards;

// Función auxiliar para convertir wchar_t* a string
string wcharToString(const wchar_t* wstr) {
    if (!wstr) return "";
    wstring ws(wstr);
    string str(ws.begin(), ws.end());
    return str;
}

// Funciones de predicción
int calcularValorCarta(const string& carta) {
    // Extraer el valor (antes del _)
    size_t pos = carta.find('_');
    if (pos == string::npos) return 0;
    
    string valor = carta.substr(0, pos);
    
    if (valor == "A") return 11;  // Por ahora As = 11
    if (valor == "J" || valor == "Q" || valor == "K") return 10;
    
    return stoi(valor);  // Convierte string a int
}

int calcularSumaMano(const vector<string>& mano) {
    int suma = 0;
    int ases = 0;
    
    for (const auto& carta : mano) {
        int valor = calcularValorCarta(carta);
        suma += valor;
        if (carta[0] == 'A') ases++;
    }
    
    // Ajustar por Ases si nos pasamos
    while (suma > 21 && ases > 0) {
        suma -= 10;  // Cambiar As de 11 a 1
        ases--;
    }
    
    return suma;
}

string predecirJugada(const vector<string>& jugador, const vector<string>& dealer) {
    int sumaJugador = calcularSumaMano(jugador);
    int cartaVisibleDealer = calcularValorCarta(dealer[0]);
    
    cout << "  Suma jugador: " << sumaJugador << endl;
    cout << "  Carta visible dealer: " << cartaVisibleDealer << endl << endl;
    
    // Estrategia básica de Blackjack
    if (sumaJugador >= 17) {
        return "PLANTARSE - Suma alta, riesgo de pasarse";
    } else if (sumaJugador <= 11) {
        return "PEDIR - Es imposible pasarse";
    } else if (sumaJugador >= 12 && sumaJugador <= 16) {
        if (cartaVisibleDealer >= 7) {
            return "PEDIR - Dealer tiene carta alta";
        } else {
            return "PLANTARSE - Dealer puede pasarse";
        }
    }
    
    return "PEDIR - Jugada estándar";
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    
    // Limpiar datos anteriores
    gameType.clear();
    playerCards.clear();
    dealerCards.clear();
    
    wchar_t* fileName = coco_string_create(L"test.card");
    
    cout << "=== Cardrep - Blackjack Predictor ===" << endl;
    cout << "Parseando archivo: test.card" << endl << endl;
    
    Scanner* scanner = new Scanner(fileName);
    Parser* parser = new Parser(scanner);
    
    parser->Parse();
    
    if (parser->errors->count == 0) {
        cout << "✓ Parsing exitoso!" << endl << endl;
        
        cout << "--- ANALISIS DE BLACKJACK ---" << endl;
        cout << "Juego: " << gameType << endl;
        cout << "Mano jugador: ";
        for (const auto& carta : playerCards) {
            cout << carta << " ";
        }
        cout << endl;
        
        cout << "Mano dealer: ";
        for (const auto& carta : dealerCards) {
            cout << carta << " ";
        }
        cout << endl << endl;
        
        // PREDICCIÓN REAL
        string recomendacion = predecirJugada(playerCards, dealerCards);
        cout << "RECOMENDACION: " << recomendacion << endl;
        
    } else {
        cout << "✗ ERROR: " << parser->errors->count << " errores" << endl;
    }
    
    coco_string_delete(fileName);
    delete parser;
    delete scanner;
    
    return 0;
}