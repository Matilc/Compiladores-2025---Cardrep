#include <iostream>
#include <wchar.h>
#include <vector>
#include <string>
#include <algorithm>
#include "Parser.h"
#include "Scanner.h"

using namespace std;

// Definir las variables globales
string gameType;
vector<string> playerCards;
vector<string> dealerCards;

// Para Truco
vector<string> opponentCards;
vector<string> mesaJugadorCards;
vector<string> mesaOponenteCards;

// Función auxiliar para convertir wchar_t* a string
string wcharToString(const wchar_t* wstr) {
    if (!wstr) return "";
    wstring ws(wstr);
    string str(ws.begin(), ws.end());
    return str;
}

// ========================================
// FUNCIONES DE BLACKJACK
// ========================================

int calcularValorCartaBlackjack(const string& carta) {
    size_t pos = carta.find('_');
    if (pos == string::npos) return 0;
    
    string valor = carta.substr(0, pos);
    
    if (valor == "A") return 11;
    if (valor == "J" || valor == "Q" || valor == "K") return 10;
    
    return stoi(valor);
}

int calcularSumaManoBlackjack(const vector<string>& mano) {
    int suma = 0;
    int ases = 0;
    
    for (const auto& carta : mano) {
        int valor = calcularValorCartaBlackjack(carta);
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

string predecirBlackjack(const vector<string>& jugador, const vector<string>& dealer) {
    int sumaJugador = calcularSumaManoBlackjack(jugador);
    int cartaVisibleDealer = calcularValorCartaBlackjack(dealer[0]);
    
    cout << "  Suma jugador: " << sumaJugador << endl;
    cout << "  Carta visible dealer: " << cartaVisibleDealer << endl << endl;
    
    // Estrategia básica de Blackjack
    if (sumaJugador > 21) {
        return "PERDISTE - Te pasaste de 21";
    } else if (sumaJugador == 21) {
        return "PLANTARSE - Tienes 21!";
    } else if (sumaJugador >= 17) {
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

// ========================================
// FUNCIONES DE TRUCO
// ========================================

int calcularValorCartaTruco(const string& carta) {
    size_t pos = carta.find('_');
    if (pos == string::npos) return 0;
    
    string valor = carta.substr(0, pos);
    string palo = carta.substr(pos + 1);
    
    // Convertir palo a minúsculas para comparación
    transform(palo.begin(), palo.end(), palo.begin(), ::tolower);
    
    // Jerarquía del Truco Argentino (de mayor a menor)
    if (valor == "1" && palo == "espada") return 14;   // Ancho de espadas
    if (valor == "1" && palo == "basto") return 13;    // Ancho de bastos
    if (valor == "7" && palo == "espada") return 12;   // Siete de espadas
    if (valor == "7" && palo == "oro") return 11;      // Siete de oro
    if (valor == "3") return 10;                       // Todos los 3
    if (valor == "2") return 9;                        // Todos los 2
    if (valor == "1") return 8;                        // Resto de 1 (copa y oro)
    if (valor == "12") return 7;                       // Rey
    if (valor == "11") return 6;                       // Caballo
    if (valor == "10") return 5;                       // Sota
    if (valor == "7") return 4;                        // Resto de 7
    if (valor == "6") return 3;
    if (valor == "5") return 2;
    if (valor == "4") return 1;
    
    return 0;
}

string obtenerNombreCarta(const string& carta) {
    size_t pos = carta.find('_');
    if (pos == string::npos) return carta;
    
    string valor = carta.substr(0, pos);
    string palo = carta.substr(pos + 1);
    
    string nombreValor;
    if (valor == "1") nombreValor = "As";
    else if (valor == "10") nombreValor = "Sota";
    else if (valor == "11") nombreValor = "Caballo";
    else if (valor == "12") nombreValor = "Rey";
    else nombreValor = valor;
    
    return nombreValor + " de " + palo;
}

string predecirTruco(const vector<string>& jugador, const vector<string>& oponente, 
                     const vector<string>& mesaJugador, const vector<string>& mesaOponente) {
    
    if (jugador.empty()) {
        return "ERROR - No tienes cartas en mano";
    }
    
    cout << "  Analizando mano del jugador..." << endl;
    
    // Encontrar la carta más alta del jugador
    int maxValorJugador = 0;
    string mejorCartaJugador;
    int cantidadCartasAltas = 0;  // Cartas con valor >= 10
    
    for (const auto& carta : jugador) {
        int valor = calcularValorCartaTruco(carta);
        cout << "    - " << obtenerNombreCarta(carta) << " (poder: " << valor << ")" << endl;
        
        if (valor > maxValorJugador) {
            maxValorJugador = valor;
            mejorCartaJugador = carta;
        }
        if (valor >= 10) {
            cantidadCartasAltas++;
        }
    }
    
    cout << endl;
    cout << "  Mejor carta: " << obtenerNombreCarta(mejorCartaJugador) 
         << " (poder: " << maxValorJugador << ")" << endl;
    cout << "  Cartas altas: " << cantidadCartasAltas << endl;
    
    // Analizar cartas jugadas en la mesa
    int ronda = mesaJugador.size();  // Cuántas cartas ya se jugaron
    cout << "  Ronda actual: " << (ronda + 1) << endl << endl;
    
    // Si hay cartas en la mesa del oponente, ver qué jugó
    if (!mesaOponente.empty() && mesaOponente.size() > mesaJugador.size()) {
        string ultimaCartaOponente = mesaOponente.back();
        int valorOponente = calcularValorCartaTruco(ultimaCartaOponente);
        
        cout << "  Oponente juega: " << obtenerNombreCarta(ultimaCartaOponente) 
             << " (poder: " << valorOponente << ")" << endl;
        
        // Buscar si tenemos carta para ganar
        bool puedoGanar = false;
        string cartaParaGanar;
        int menorCartaGanadora = 99;
        
        for (const auto& carta : jugador) {
            int valor = calcularValorCartaTruco(carta);
            if (valor > valorOponente && valor < menorCartaGanadora) {
                puedoGanar = true;
                cartaParaGanar = carta;
                menorCartaGanadora = valor;
            }
        }
        
        if (puedoGanar) {
            return "JUGAR " + obtenerNombreCarta(cartaParaGanar) + " - Puedes ganar la mano";
        } else {
            return "JUGAR LA CARTA MAS BAJA - No puedes ganar esta mano, guarda las buenas";
        }
    }
    
    // Estrategia según la calidad de las cartas
    if (maxValorJugador >= 12) {
        // Tienes cartas muy fuertes (Ancho de espadas, bastos, o 7 de espadas/oro)
        if (cantidadCartasAltas >= 2) {
            return "CANTAR TRUCO - Tienes 2+ cartas fuertes, presiona al oponente";
        } else {
            return "JUGAR - Tienes una carta muy fuerte, espera a ver qué hace el oponente";
        }
    } else if (maxValorJugador >= 10) {
        // Tienes cartas buenas (3 o 2)
        return "JUGAR - Tienes buenas cartas, pero no cantes truco todavía";
    } else if (maxValorJugador >= 7) {
        // Tienes cartas medias (Rey, Caballo, Sota)
        if (ronda == 0) {
            return "JUGAR - Cartas medias, juega conservador";
        } else {
            return "CONSIDERAR RETIRARSE - Cartas débiles y ya avanzaste en el juego";
        }
    } else {
        // Tienes cartas bajas
        if (ronda == 0) {
            return "RETIRARSE - Cartas muy débiles, mejor no arriesgar puntos";
        } else {
            return "RETIRARSE - Ya jugaste con cartas débiles, corta las pérdidas";
        }
    }
}

// ========================================
// MAIN
// ========================================

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    
    // Limpiar datos anteriores
    gameType.clear();
    playerCards.clear();
    dealerCards.clear();
    opponentCards.clear();
    mesaJugadorCards.clear();
    mesaOponenteCards.clear();
    
    wchar_t* fileName = coco_string_create(L"test.card");
    
    cout << "========================================" << endl;
    cout << "  Cardrep - Multi-Game Card Predictor" << endl;
    cout << "========================================" << endl << endl;
    
    Scanner* scanner = new Scanner(fileName);
    Parser* parser = new Parser(scanner);
    
    parser->Parse();
    
    if (parser->errors->count == 0) {
        cout << "Parsing exitoso!" << endl << endl;
        
        cout << "--- ANALISIS DEL JUEGO ---" << endl;
        cout << "Juego detectado: " << gameType << endl << endl;
        
        string recomendacion;
        
        // ========== BLACKJACK ==========
        if (gameType == "Blackjack" || gameType == "blackjack") {
            cout << "=== BLACKJACK ===" << endl;
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
            
            recomendacion = predecirBlackjack(playerCards, dealerCards);
        }
        // ========== TRUCO ==========
        else if (gameType == "Truco" || gameType == "truco") {
            cout << "=== TRUCO ===" << endl;
            cout << "Mano jugador: ";
            for (const auto& carta : playerCards) {
                cout << carta << " ";
            }
            cout << endl;
            
            cout << "Mano oponente: ";
            for (const auto& carta : opponentCards) {
                cout << carta << " ";
            }
            cout << endl;
            
            cout << "Mesa jugador: ";
            if (mesaJugadorCards.empty()) {
                cout << "(vacia)";
            } else {
                for (const auto& carta : mesaJugadorCards) {
                    cout << carta << " ";
                }
            }
            cout << endl;
            
            cout << "Mesa oponente: ";
            if (mesaOponenteCards.empty()) {
                cout << "(vacia)";
            } else {
                for (const auto& carta : mesaOponenteCards) {
                    cout << carta << " ";
                }
            }
            cout << endl << endl;
            
            recomendacion = predecirTruco(playerCards, opponentCards, 
                                         mesaJugadorCards, mesaOponenteCards);
        }
        // ========== JUEGO NO RECONOCIDO ==========
        else {
            recomendacion = "ERROR - Juego no reconocido: '" + gameType + "'";
            cout << "Juegos soportados: Blackjack, Truco" << endl;
        }
        
        cout << "========================================" << endl;
        cout << "RECOMENDACION: " << recomendacion << endl;
        cout << "========================================" << endl;
        
    } else {
        cout << "✗ ERROR DE PARSING: " << parser->errors->count << " error(es) encontrado(s)" << endl;
        cout << "Verifica la sintaxis del archivo test.card" << endl;
    }
    
    coco_string_delete(fileName);
    delete parser;
    delete scanner;
    
    return 0;
}