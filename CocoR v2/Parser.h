

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__



#include "Scanner.h"
#include <vector>
#include <string>

std::string wcharToString(const wchar_t* wstr);

// Variables globales para almacenar el estado del juego
extern std::string gameType;
extern std::vector<std::string> playerCards;
extern std::vector<std::string> dealerCards;


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void Cardrep();
	void Estado();
	void ListaCartasJugador();
	void ListaCartasDealer();
	void CartaJugador();
	void CartaDealer();
	void ValorCarta();

	void Parse();

}; // end Parser



#endif

