// Código generado automáticamente por Cardrep Compiler
// Juego: Truco

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

// === FUNCIONES TRUCO ===

int calcularValorCartaTruco(const string& carta) {
    size_t pos = carta.find('_');
    if (pos == string::npos) return 0;
    string valor = carta.substr(0, pos);
    string palo = carta.substr(pos + 1);
    transform(palo.begin(), palo.end(), palo.begin(), ::tolower);
    if (valor == "1" && palo == "espada") return 14;
    if (valor == "1" && palo == "basto") return 13;
    if (valor == "7" && palo == "espada") return 12;
    if (valor == "7" && palo == "oro") return 11;
    if (valor == "3") return 10;
    if (valor == "2") return 9;
    if (valor == "1") return 8;
    if (valor == "12") return 7;
    if (valor == "11") return 6;
    if (valor == "10") return 5;
    if (valor == "7") return 4;
    if (valor == "6") return 3;
    if (valor == "5") return 2;
    if (valor == "4") return 1;
    return 0;
}

string getNombreCarta(const string& carta) {
    size_t pos = carta.find('_');
    if (pos == string::npos) return carta;
    string valor = carta.substr(0, pos);
    string palo = carta.substr(pos + 1);
    string nombre = (valor == "1") ? "As" : (valor == "10") ? "Sota" : (valor == "11") ? "Caballo" : (valor == "12") ? "Rey" : valor;
    return nombre + " de " + palo;
}

string analizarMovimiento(const vector<string>& mano, const vector<string>& mesaJ, const vector<string>& mesaO) {
    if (mano.empty()) return "ERROR - Sin cartas";
    int maxVal = 0, cartasAltas = 0;
    string mejorCarta;
    for (const auto& c : mano) {
        int v = calcularValorCartaTruco(c);
        if (v > maxVal) { maxVal = v; mejorCarta = c; }
        if (v >= 10) cartasAltas++;
    }
    int ronda = mesaJ.size();
    cout << "  Mejor carta: " << getNombreCarta(mejorCarta) << " (poder: " << maxVal << ")" << endl;
    if (!mesaO.empty() && mesaO.size() > mesaJ.size()) {
        int valOp = calcularValorCartaTruco(mesaO.back());
        cout << "  Oponente jugó: " << getNombreCarta(mesaO.back()) << " (" << valOp << ")" << endl;
        bool puedoGanar = false;
        for (const auto& c : mano) if (calcularValorCartaTruco(c) > valOp) { puedoGanar = true; break; }
        return puedoGanar ? "JUGAR carta ganadora" : "JUGAR carta más baja";
    }
    if (maxVal >= 10) return cartasAltas >= 2 ? "JUGAR fuerte" : "JUGAR bajo";
    return "JUGAR bajo y esperar";
}

string analizarEstrategia(const vector<string>& mano, const vector<string>& mesaJ, const vector<string>& mesaO) {
    if (mano.empty()) return "";
    int maxVal = 0, cartasAltas = 0;
    for (const auto& c : mano) {
        int v = calcularValorCartaTruco(c);
        if (v > maxVal) maxVal = v;
        if (v >= 10) cartasAltas++;
    }
    if (maxVal >= 12 && cartasAltas >= 2) return "CANTA TRUCO - Mano fuerte";
    if (maxVal >= 10) return "ESPERA - Mano decente";
    return "NO CANTAR - Mano débil";
}

string getPalo(const string& carta){
  size_t p = carta.find('_'); if(p==string::npos) return "";
  string palo = carta.substr(p+1);
  transform(palo.begin(), palo.end(), palo.begin(), ::tolower);
  return palo;
}
int valorEnvido(const string& carta){
  size_t p = carta.find('_'); if(p==string::npos) return 0;
  string v = carta.substr(0,p);
  if(v=="10"||v=="11"||v=="12") return 0;
  if(v=="1") return 1;
  return stoi(v);
}
int calcularEnvido(const vector<string>& mano){
  if(mano.empty()) return 0;
  vector<pair<string,int>> oro,copa,esp,bas;
  for(const auto& c: mano){
    string p = getPalo(c); int val = valorEnvido(c);
    if(p=="oro") oro.push_back({c,val});
    else if(p=="copa") copa.push_back({c,val});
    else if(p=="espada") esp.push_back({c,val});
    else if(p=="basto") bas.push_back({c,val});
  }
  auto calcPalo=[&](const vector<pair<string,int>>& v){
    if(v.size()>=2){ int m1=0,m2=0; for(auto &x:v){
      if(x.second>m1){ m2=m1; m1=x.second; }
      else if(x.second>m2){ m2=x.second; } } return 20+m1+m2; }
    else if(v.size()==1) return v[0].second; return 0; };
  return max({calcPalo(oro),calcPalo(copa),calcPalo(esp),calcPalo(bas)});
}
string analizarEnvido(const vector<string>& mano, int ronda){
  int envido = calcularEnvido(mano);
  if(ronda>0) return string("NO CANTAR ENVIDO - Ya paso el momento (solo en primera)");
  if(envido>=33) return string("CANTA ENVIDO/REAL ENVIDO/FALTA ENVIDO - Envido excelente (33+)");
  if(envido>=29) return string("CANTA ENVIDO - Envido muy bueno (29-32)");
  if(envido>=26) return string("CANTA ENVIDO - Envido bueno (26-28)");
  if(envido>=24) return string("CONSIDERA CANTAR - Decente (24-25)");
  if(envido>=20) return string("NO CANTES - Bajo (20-23)");
  return string("NO CANTES - Muy bajo (<20)");
}
int main() {
    cout << "========================================" << endl;
    cout << "  TRUCO - Análisis de Jugada" << endl;
    cout << "========================================" << endl << endl;

    vector<string> mano = {"4_espada", "3_basto", "3_oro"};
    vector<string> mesaJugador = {};
    vector<string> mesaOponente = {"6_espada"};

    cout << "Mano: "; for (const auto& c : mano) cout << c << " "; cout << endl;
    cout << "Mesa jugador: "; for (const auto& c : mesaJugador) cout << c << " "; cout << endl;
    cout << "Mesa oponente: "; for (const auto& c : mesaOponente) cout << c << " "; cout << endl << endl;

    string movimiento = analizarMovimiento(mano, mesaJugador, mesaOponente);
    string estrategia = analizarEstrategia(mano, mesaJugador, mesaOponente);
    string envido = analizarEnvido(mano, mesaJugador.size());

    cout << "========================================" << endl;
    cout << "MOVIMIENTO: " << movimiento << endl;
    cout << "TRUCO: " << estrategia << endl;
    cout << "ENVIDO: " << envido << endl;
    cout << "========================================" << endl;

    return 0;
}
