#ifndef INTERMEDIATE_CODE_H
#define INTERMEDIATE_CODE_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

// Generador de código C++ desde lenguaje Cardrep
class IntermediateCode {
private:
    std::vector<std::string> cppLines;
    std::string gameType;
    std::vector<std::string> playerCards;
    std::vector<std::string> dealerCards;
    std::vector<std::string> mesaPlayerCards;
    std::vector<std::string> mesaOpponentCards;
    
    std::string vectorToInitializer(const std::vector<std::string>& vec) {
        std::stringstream ss;
        ss << "{";
        for (size_t i = 0; i < vec.size(); i++) {
            ss << "\"" << vec[i] << "\"";
            if (i < vec.size() - 1) ss << ", ";
        }
        ss << "}";
        return ss.str();
    }
    
public:
    void emitGame(const std::string& game) {
        gameType = game;
        cppLines.clear();
        playerCards.clear();
        dealerCards.clear();
        mesaPlayerCards.clear();
        mesaOpponentCards.clear();
    }
    
    void emitLoadPlayer(const std::string& card) {
        playerCards.push_back(card);
    }
    
    void emitLoadDealer(const std::string& card) {
        dealerCards.push_back(card);
    }
    
    void emitLoadMesaPlayer(const std::string& card) {
        mesaPlayerCards.push_back(card);
    }
    
    void emitLoadMesaOpponent(const std::string& card) {
        mesaOpponentCards.push_back(card);
    }
    
    void emitHalt() {
        generateCppFile();
    }
    
    void generateCppFile() {
        cppLines.clear();
        
        // Header
        cppLines.push_back("// Código generado automáticamente por Cardrep Compiler");
        cppLines.push_back("// Juego: " + gameType);
        cppLines.push_back("");
        cppLines.push_back("#include <iostream>");
        cppLines.push_back("#include <vector>");
        cppLines.push_back("#include <string>");
        cppLines.push_back("#include <algorithm>");
        cppLines.push_back("#include <cmath>");
        cppLines.push_back("#include <set>");
        cppLines.push_back("");
        cppLines.push_back("using namespace std;");
        cppLines.push_back("");
        
        // Incluir las funciones de análisis según el juego
        if (gameType == "Blackjack" || gameType == "blackjack") {
            generateBlackjackFunctions();
            generateBlackjackMain();
        } else if (gameType == "Truco" || gameType == "truco") {
            generateTrucoFunctions();
            generateTrucoMain();
        }
    }
    
    void generateBlackjackFunctions() {
        cppLines.push_back("// === FUNCIONES BLACKJACK ===");
        cppLines.push_back("");
        cppLines.push_back("int calcularValorCarta(const string& carta) {");
        cppLines.push_back("    size_t pos = carta.find('_');");
        cppLines.push_back("    if (pos == string::npos) return 0;");
        cppLines.push_back("    string valor = carta.substr(0, pos);");
        cppLines.push_back("    if (valor == \"A\") return 11;");
        cppLines.push_back("    if (valor == \"J\" || valor == \"Q\" || valor == \"K\") return 10;");
        cppLines.push_back("    return stoi(valor);");
        cppLines.push_back("}");
        cppLines.push_back("");
        cppLines.push_back("int calcularSumaMano(const vector<string>& mano) {");
        cppLines.push_back("    int suma = 0, ases = 0;");
        cppLines.push_back("    for (const auto& carta : mano) {");
        cppLines.push_back("        int valor = calcularValorCarta(carta);");
        cppLines.push_back("        suma += valor;");
        cppLines.push_back("        if (carta[0] == 'A') ases++;");
        cppLines.push_back("    }");
        cppLines.push_back("    while (suma > 21 && ases > 0) {");
        cppLines.push_back("        suma -= 10;");
        cppLines.push_back("        ases--;");
        cppLines.push_back("    }");
        cppLines.push_back("    return suma;");
        cppLines.push_back("}");
        cppLines.push_back("");
        cppLines.push_back("string predecirBlackjack(const vector<string>& jugador, const vector<string>& dealer) {");
        cppLines.push_back("    int sumaJugador = calcularSumaMano(jugador);");
        cppLines.push_back("    int cartaDealer = calcularValorCarta(dealer[0]);");
        cppLines.push_back("    cout << \"  Suma jugador: \" << sumaJugador << endl;");
        cppLines.push_back("    cout << \"  Carta dealer: \" << cartaDealer << endl << endl;");
        cppLines.push_back("    if (sumaJugador > 21) return \"PERDISTE - Te pasaste de 21\";");
        cppLines.push_back("    if (sumaJugador == 21) return \"PLANTARSE - Tienes 21!\";");
        cppLines.push_back("    if (sumaJugador >= 17) return \"PLANTARSE - Suma alta\";");
        cppLines.push_back("    if (sumaJugador <= 11) return \"PEDIR - Es imposible pasarse\";");
        cppLines.push_back("    if (sumaJugador >= 12 && sumaJugador <= 16) {");
        cppLines.push_back("        if (cartaDealer >= 7) return \"PEDIR - Dealer tiene carta alta\";");
        cppLines.push_back("        return \"PLANTARSE - Dealer puede pasarse\";");
        cppLines.push_back("    }");
        cppLines.push_back("    return \"PEDIR\";");
        cppLines.push_back("}");
        cppLines.push_back("");
    }
    
    void generateBlackjackMain() {
        cppLines.push_back("int main() {");
        cppLines.push_back("    cout << \"========================================\" << endl;");
        cppLines.push_back("    cout << \"  BLACKJACK - Análisis de Jugada\" << endl;");
        cppLines.push_back("    cout << \"========================================\" << endl << endl;");
        cppLines.push_back("");
        cppLines.push_back("    vector<string> jugador = " + vectorToInitializer(playerCards) + ";");
        cppLines.push_back("    vector<string> dealer = " + vectorToInitializer(dealerCards) + ";");
        cppLines.push_back("");
        cppLines.push_back("    cout << \"Mano jugador: \";");
        cppLines.push_back("    for (const auto& c : jugador) cout << c << \" \";");
        cppLines.push_back("    cout << endl << \"Mano dealer: \";");
        cppLines.push_back("    for (const auto& c : dealer) cout << c << \" \";");
        cppLines.push_back("    cout << endl << endl;");
        cppLines.push_back("");
        cppLines.push_back("    string resultado = predecirBlackjack(jugador, dealer);");
        cppLines.push_back("");
        cppLines.push_back("    cout << \"========================================\" << endl;");
        cppLines.push_back("    cout << \"RECOMENDACIÓN: \" << resultado << endl;");
        cppLines.push_back("    cout << \"========================================\" << endl;");
        cppLines.push_back("");
        cppLines.push_back("    return 0;");
        cppLines.push_back("}");
    }
    
    void generateTrucoFunctions() {
        cppLines.push_back("// === FUNCIONES TRUCO ===");
        cppLines.push_back("");
        cppLines.push_back("int calcularValorCartaTruco(const string& carta) {");
        cppLines.push_back("    size_t pos = carta.find('_');");
        cppLines.push_back("    if (pos == string::npos) return 0;");
        cppLines.push_back("    string valor = carta.substr(0, pos);");
        cppLines.push_back("    string palo = carta.substr(pos + 1);");
        cppLines.push_back("    transform(palo.begin(), palo.end(), palo.begin(), ::tolower);");
        cppLines.push_back("    if (valor == \"1\" && palo == \"espada\") return 14;");
        cppLines.push_back("    if (valor == \"1\" && palo == \"basto\") return 13;");
        cppLines.push_back("    if (valor == \"7\" && palo == \"espada\") return 12;");
        cppLines.push_back("    if (valor == \"7\" && palo == \"oro\") return 11;");
        cppLines.push_back("    if (valor == \"3\") return 10;");
        cppLines.push_back("    if (valor == \"2\") return 9;");
        cppLines.push_back("    if (valor == \"1\") return 8;");
        cppLines.push_back("    if (valor == \"12\") return 7;");
        cppLines.push_back("    if (valor == \"11\") return 6;");
        cppLines.push_back("    if (valor == \"10\") return 5;");
        cppLines.push_back("    if (valor == \"7\") return 4;");
        cppLines.push_back("    if (valor == \"6\") return 3;");
        cppLines.push_back("    if (valor == \"5\") return 2;");
        cppLines.push_back("    if (valor == \"4\") return 1;");
        cppLines.push_back("    return 0;");
        cppLines.push_back("}");
        cppLines.push_back("");
        cppLines.push_back("string getNombreCarta(const string& carta) {");
        cppLines.push_back("    size_t pos = carta.find('_');");
        cppLines.push_back("    if (pos == string::npos) return carta;");
        cppLines.push_back("    string valor = carta.substr(0, pos);");
        cppLines.push_back("    string palo = carta.substr(pos + 1);");
        cppLines.push_back("    string nombre = (valor == \"1\") ? \"As\" : (valor == \"10\") ? \"Sota\" : (valor == \"11\") ? \"Caballo\" : (valor == \"12\") ? \"Rey\" : valor;");
        cppLines.push_back("    return nombre + \" de \" + palo;");
        cppLines.push_back("}");
        cppLines.push_back("");
        
        // Función de movimiento
        cppLines.push_back("string analizarMovimiento(const vector<string>& mano, const vector<string>& mesaJ, const vector<string>& mesaO) {");
        cppLines.push_back("    if (mano.empty()) return \"ERROR - Sin cartas\";");
        cppLines.push_back("    int maxVal = 0, cartasAltas = 0;");
        cppLines.push_back("    string mejorCarta;");
        cppLines.push_back("    for (const auto& c : mano) {");
        cppLines.push_back("        int v = calcularValorCartaTruco(c);");
        cppLines.push_back("        if (v > maxVal) { maxVal = v; mejorCarta = c; }");
        cppLines.push_back("        if (v >= 10) cartasAltas++;");
        cppLines.push_back("    }");
        cppLines.push_back("    int ronda = mesaJ.size();");
        cppLines.push_back("    cout << \"  Mejor carta: \" << getNombreCarta(mejorCarta) << \" (poder: \" << maxVal << \")\" << endl;");
        cppLines.push_back("    if (!mesaO.empty() && mesaO.size() > mesaJ.size()) {");
        cppLines.push_back("        int valOp = calcularValorCartaTruco(mesaO.back());");
        cppLines.push_back("        cout << \"  Oponente jugó: \" << getNombreCarta(mesaO.back()) << \" (\" << valOp << \")\" << endl;");
        cppLines.push_back("        bool puedoGanar = false;");
        cppLines.push_back("        for (const auto& c : mano) if (calcularValorCartaTruco(c) > valOp) { puedoGanar = true; break; }");
        cppLines.push_back("        return puedoGanar ? \"JUGAR carta ganadora\" : \"JUGAR carta más baja\";");
        cppLines.push_back("    }");
        cppLines.push_back("    if (maxVal >= 10) return cartasAltas >= 2 ? \"JUGAR fuerte\" : \"JUGAR bajo\";");
        cppLines.push_back("    return \"JUGAR bajo y esperar\";");
        cppLines.push_back("}");
        cppLines.push_back("");
        
        // Función de estrategia
        cppLines.push_back("string analizarEstrategia(const vector<string>& mano, const vector<string>& mesaJ, const vector<string>& mesaO) {");
        cppLines.push_back("    if (mano.empty()) return \"\";");
        cppLines.push_back("    int maxVal = 0, cartasAltas = 0;");
        cppLines.push_back("    for (const auto& c : mano) {");
        cppLines.push_back("        int v = calcularValorCartaTruco(c);");
        cppLines.push_back("        if (v > maxVal) maxVal = v;");
        cppLines.push_back("        if (v >= 10) cartasAltas++;");
        cppLines.push_back("    }");
        cppLines.push_back("    if (maxVal >= 12 && cartasAltas >= 2) return \"CANTA TRUCO - Mano fuerte\";");
        cppLines.push_back("    if (maxVal >= 10) return \"ESPERA - Mano decente\";");
        cppLines.push_back("    return \"NO CANTAR - Mano débil\";");
        cppLines.push_back("}");
        cppLines.push_back("");
        
        // Función de envido
        // Obtener palo (minúsculas)
        cppLines.push_back("string getPalo(const string& carta){");
        cppLines.push_back("  size_t p = carta.find('_'); if(p==string::npos) return \"\";");
        cppLines.push_back("  string palo = carta.substr(p+1);");
        cppLines.push_back("  transform(palo.begin(), palo.end(), palo.begin(), ::tolower);");
        cppLines.push_back("  return palo;");
        cppLines.push_back("}");
        // Valor para Envido: 10/11/12 valen 0; As=1; resto su número
        cppLines.push_back("int valorEnvido(const string& carta){");
        cppLines.push_back("  size_t p = carta.find('_'); if(p==string::npos) return 0;");
        cppLines.push_back("  string v = carta.substr(0,p);");
        cppLines.push_back("  if(v==\"10\"||v==\"11\"||v==\"12\") return 0;");
        cppLines.push_back("  if(v==\"1\") return 1;");
        cppLines.push_back("  return stoi(v);");
        cppLines.push_back("}");
        // Cálculo de Envido (máximo por palo, dos mejores +20 o 1 carta)
        cppLines.push_back("int calcularEnvido(const vector<string>& mano){");
        cppLines.push_back("  if(mano.empty()) return 0;");
        cppLines.push_back("  vector<pair<string,int>> oro,copa,esp,bas;");
        cppLines.push_back("  for(const auto& c: mano){");
        cppLines.push_back("    string p = getPalo(c); int val = valorEnvido(c);");
        cppLines.push_back("    if(p==\"oro\") oro.push_back({c,val});");
        cppLines.push_back("    else if(p==\"copa\") copa.push_back({c,val});");
        cppLines.push_back("    else if(p==\"espada\") esp.push_back({c,val});");
        cppLines.push_back("    else if(p==\"basto\") bas.push_back({c,val});");
        cppLines.push_back("  }");
        cppLines.push_back("  auto calcPalo=[&](const vector<pair<string,int>>& v){");
        cppLines.push_back("    if(v.size()>=2){ int m1=0,m2=0; for(auto &x:v){");
        cppLines.push_back("      if(x.second>m1){ m2=m1; m1=x.second; }");
        cppLines.push_back("      else if(x.second>m2){ m2=x.second; } } return 20+m1+m2; }");
        cppLines.push_back("    else if(v.size()==1) return v[0].second; return 0; };");
        cppLines.push_back("  return max({calcPalo(oro),calcPalo(copa),calcPalo(esp),calcPalo(bas)});");
        cppLines.push_back("}");
        // Analizar Envido (solo en 1ra, umbrales realistas)
        cppLines.push_back("string analizarEnvido(const vector<string>& mano, int ronda){");
        cppLines.push_back("  int envido = calcularEnvido(mano);");
        cppLines.push_back("  if(ronda>0) return string(\"NO CANTAR ENVIDO - Ya paso el momento (solo en primera)\");");
        cppLines.push_back("  if(envido>=33) return string(\"CANTA ENVIDO/REAL ENVIDO/FALTA ENVIDO - Envido excelente (33+)\");");
        cppLines.push_back("  if(envido>=29) return string(\"CANTA ENVIDO - Envido muy bueno (29-32)\");");
        cppLines.push_back("  if(envido>=26) return string(\"CANTA ENVIDO - Envido bueno (26-28)\");");
        cppLines.push_back("  if(envido>=24) return string(\"CONSIDERA CANTAR - Decente (24-25)\");");
        cppLines.push_back("  if(envido>=20) return string(\"NO CANTES - Bajo (20-23)\");");
        cppLines.push_back("  return string(\"NO CANTES - Muy bajo (<20)\");");
        cppLines.push_back("}");
    }
    
    void generateTrucoMain() {
        cppLines.push_back("int main() {");
        cppLines.push_back("    cout << \"========================================\" << endl;");
        cppLines.push_back("    cout << \"  TRUCO - Análisis de Jugada\" << endl;");
        cppLines.push_back("    cout << \"========================================\" << endl << endl;");
        cppLines.push_back("");
        cppLines.push_back("    vector<string> mano = " + vectorToInitializer(playerCards) + ";");
        cppLines.push_back("    vector<string> mesaJugador = " + vectorToInitializer(mesaPlayerCards) + ";");
        cppLines.push_back("    vector<string> mesaOponente = " + vectorToInitializer(mesaOpponentCards) + ";");
        cppLines.push_back("");
        cppLines.push_back("    cout << \"Mano: \"; for (const auto& c : mano) cout << c << \" \"; cout << endl;");
        cppLines.push_back("    cout << \"Mesa jugador: \"; for (const auto& c : mesaJugador) cout << c << \" \"; cout << endl;");
        cppLines.push_back("    cout << \"Mesa oponente: \"; for (const auto& c : mesaOponente) cout << c << \" \"; cout << endl << endl;");
        cppLines.push_back("");
        cppLines.push_back("    string movimiento = analizarMovimiento(mano, mesaJugador, mesaOponente);");
        cppLines.push_back("    string estrategia = analizarEstrategia(mano, mesaJugador, mesaOponente);");
        cppLines.push_back("    string envido = analizarEnvido(mano, mesaJugador.size());");
        cppLines.push_back("");
        cppLines.push_back("    cout << \"========================================\" << endl;");
        cppLines.push_back("    cout << \"MOVIMIENTO: \" << movimiento << endl;");
        cppLines.push_back("    cout << \"TRUCO: \" << estrategia << endl;");
        cppLines.push_back("    cout << \"ENVIDO: \" << envido << endl;");
        cppLines.push_back("    cout << \"========================================\" << endl;");
        cppLines.push_back("");
        cppLines.push_back("    return 0;");
        cppLines.push_back("}");
    }
    
    void saveToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: No se pudo crear " << filename << std::endl;
            return;
        }
        
        for (const auto& line : cppLines) {
            file << line << std::endl;
        }
        
        file.close();
        std::cout << "✓ Código C++ generado en: " << filename << std::endl;
        std::cout << "  Compilar con: g++ " << filename << " -o programa" << std::endl;
        std::cout << "  Ejecutar con: ./programa" << std::endl;
    }
    
    void print() {
        std::cout << "\n--- CÓDIGO C++ GENERADO ---" << std::endl;
        for (const auto& line : cppLines) {
            std::cout << line << std::endl;
        }
        std::cout << "-----------------------------\n" << std::endl;
    }
    
    void clear() {
        cppLines.clear();
        gameType.clear();
        playerCards.clear();
        dealerCards.clear();
        mesaPlayerCards.clear();
        mesaOpponentCards.clear();
    }
};

extern IntermediateCode intermediateCode;

#endif // INTERMEDIATE_CODE_H