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

string predecirTrucoMovimiento(const vector<string>& jugador, const vector<string>& oponente, const vector<string>& mesaJugador, const vector<string>& mesaOponente) {
    if (jugador.empty()) {
        return "ERROR - No tienes cartas en mano";
    }
    
    cout << "  Analizando mano del jugador..." << endl;
    
    int maxValorJugador = 0;
    string mejorCartaJugador;
    int cantidadCartasAltas = 0;
    
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
    else {
        if (maxValorJugador >= 10) {
        if (cantidadCartasAltas >= 2) {
            if (ronda == 0) {
                return "JUGA BAJO Y ESPERA AL OPONENTE, O GANA PRIMERA JUGANDO FUERTE";
            }
            else {
                return "JUGA A GANAR LA MANO";
            }
        } else {
            return "JUGA LA MAS BAJA, O FUERTE SI PERDISTE PRIMERA";
        }
    } else if (maxValorJugador >= 7) {
        if (ronda == 0) {
            return "JUGA BAJO, Y ESPERA AL OPONENTE";
        } else {
            return "JUGA LA MAS ALTA";
        }
    } else {
        if (ronda == 0) {
            return "JUGA BAJO, Y ESPERA AL OPONENTE";
        } else {
            return "JUGA LA MAS ALTA";
        }
    }
    }
}

string predecirTrucoEstrategia(const vector<string>& jugador, const vector<string>& oponente, const vector<string>& mesaJugador, const vector<string>& mesaOponente) {
    if (jugador.empty()) {
        return "";
    }
    
    int maxValorJugador = 0;
    string mejorCartaJugador;
    int cantidadCartasAltas = 0;
    
    for (const auto& carta : jugador) {
        int valor = calcularValorCartaTruco(carta);

        if (valor > maxValorJugador) {
            maxValorJugador = valor;
            mejorCartaJugador = carta;
        }
        if (valor >= 10) {
            cantidadCartasAltas++;
        }
    }
    int ronda = mesaJugador.size();

    // Estrategia según la calidad de las cartas
    if (!mesaOponente.empty() && mesaOponente.size() > mesaJugador.size()) {
        string ultimaCartaOponente = mesaOponente.back();
        int valorOponente = calcularValorCartaTruco(ultimaCartaOponente);

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
            return "CANTA TRUCO - Ganaste primera, podes apurar al oponente";
        } else {
            return "JUGAR LA CARTA MAS BAJA - No podes ganar esta mano, guarda las buenas";
        }
    } else if (maxValorJugador >= 12) {
        if (cantidadCartasAltas >= 2) {
            if (ronda == 0) {
                return "NO CANTES TRUCO - Tienes 2+ cartas fuertes, espera para que el oponente cante";
            }
            else {
                return "CANTA TRUCO - Tienes 2+ cartas fuertes, apretale para que decida";
            }
        } else {
            return "NO CANTES TRUCO - Tienes una carta muy fuerte, espera a ver qué hace el oponente";
        }
    } else if (maxValorJugador >= 10) {
        if (cantidadCartasAltas >= 2) {
            if (ronda == 0){
                return "NO CANTES TRUCO - Tienes 2+ cartas buenas, pero espera al oponente";
            }
            else {
                return "CANTA TRUCO - Tienes 2+ cartas buenas, asustalo con un Truco";
            }
        }
        else {
            return "NO CANTES TRUCO - Tienes una carta buena, espera al oponente";
        }
    } else if (maxValorJugador >= 7) {
        if (ronda == 0) {
            return "NO CANTES TRUCO - Cartas medias, juga tranquilo";
        } else {
            return "ESPERA AL OPONENTE, PENSA EN IRTE - Cartas débiles y ya avanzaste en el juego";
        }
    } else {
        if (ronda == 0) {
            return "PODES CANTAR PARA ASUSTAR, PERO CONSIDERA IRTE - Cartas muy débiles, mejor no arriesgar puntos";
        } else {
            return "PODES CANTAR PARA ASUSTAR, PERO CONSIDERA IRTE - Ya jugaste con cartas débiles, corta las pérdidas";
        }
    }
}

int obtenerValorEnvido(const string& carta) {
    size_t pos = carta.find('_');
    if (pos == string::npos) return 0;
    
    string valor = carta.substr(0, pos);
    
    // Figuras valen 0 para el envido
    if (valor == "10" || valor == "11" || valor == "12") return 0;
    
    // As vale 1
    if (valor == "1") return 1;
    
    // Resto: valor numérico
    return stoi(valor);
}

string obtenerPalo(const string& carta) {
    size_t pos = carta.find('_');
    if (pos == string::npos) return "";
    
    string palo = carta.substr(pos + 1);
    transform(palo.begin(), palo.end(), palo.begin(), ::tolower);
    return palo;
}

int calcularEnvido(const vector<string>& mano) {
    if (mano.empty()) return 0;
    
    vector<pair<string, int>> oro, copa, espada, basto;
    
    for (const auto& carta : mano) {
        string palo = obtenerPalo(carta);
        int valor = obtenerValorEnvido(carta);
        
        if (palo == "oro") oro.push_back({carta, valor});
        else if (palo == "copa") copa.push_back({carta, valor});
        else if (palo == "espada") espada.push_back({carta, valor});
        else if (palo == "basto") basto.push_back({carta, valor});
    }
    
    int maxEnvido = 0;
    
    // Función auxiliar para calcular envido de un palo
    auto calcularPalo = [&](const vector<pair<string, int>>& cartas) {
        if (cartas.size() >= 2) {
            int max1 = 0, max2 = 0;
            for (const auto& c : cartas) {
                if (c.second > max1) {
                    max2 = max1;
                    max1 = c.second;
                } else if (c.second > max2) {
                    max2 = c.second;
                }
            }
            return 20 + max1 + max2;
        } else if (cartas.size() == 1) {
            // Una sola carta: su valor
            return cartas[0].second;
        }
        return 0;
    };
    
    maxEnvido = max({calcularPalo(oro), calcularPalo(copa), calcularPalo(espada), calcularPalo(basto)});
    
    return maxEnvido;
}

string predecirTrucoEnvido(const vector<string>& jugador, int ronda) {
    int envido = calcularEnvido(jugador);
    
    cout << "  Puntos de Envido: " << envido << endl;
    
    vector<string> palos = {"oro", "copa", "espada", "basto"};
    
    // Estrategia de Envido
    if (ronda > 0) {
        return "NO CANTAR ENVIDO - Ya paso el momento (solo se canta en primera)";
    }
    
    if (envido >= 33) {
        return "CANTA ENVIDO/REAL ENVIDO/FALTA ENVIDO - Envido excelente (33+)";
    } else if (envido >= 29) {
        return "CANTA ENVIDO - Envido muy bueno (29-32)";
    } else if (envido >= 26) {
        return "CANTA ENVIDO - Envido bueno (26-28)";
    } else if (envido >= 24) {
        return "CONSIDERA CANTAR - Envido decente (24-25), depende de tu estrategia";
    } else if (envido >= 20) {
        return "NO CANTES - Envido bajo (20-23), mejor no arriesgar";
    } else {
        return "NO CANTES - Envido muy bajo (<20), espera al oponente o pasa";
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    
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

        cout << "--- ANALISIS DEL JUEGO ---" << endl;
        cout << "Juego detectado: " << gameType << endl << endl;
        
        string recomendacion;
        string recomendacionMovimiento;
        string recomendacionEstrategia;
        string recomendacionEnvido;

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
            
            recomendacionMovimiento = predecirTrucoMovimiento(playerCards, opponentCards, mesaJugadorCards, mesaOponenteCards);
            recomendacionEstrategia = predecirTrucoEstrategia(playerCards, opponentCards, mesaJugadorCards, mesaOponenteCards);
            recomendacionEnvido = predecirTrucoEnvido(playerCards, mesaJugadorCards.size());
        }
        // ========== JUEGO NO RECONOCIDO ==========
        else {
            recomendacion = "ERROR - Juego no reconocido: '" + gameType + "'";
            cout << "Juegos soportados: Blackjack, Truco" << endl;
        }
        
        cout << "========================================" << endl;
        if (gameType == "Truco" || gameType == "truco") {
            cout << "RECOMENDACION DE MOVIMIMIENTO:\n" << recomendacionMovimiento <<endl;
            cout << "RECOMENDACION DE TRUCO:\n" << recomendacionEstrategia <<endl;
            cout << "RECOMENDACION DE ENVIDO:\n" << recomendacionEnvido <<endl;
        }
        else {
            cout << "RECOMENDACION: " << recomendacion << endl;
        }
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