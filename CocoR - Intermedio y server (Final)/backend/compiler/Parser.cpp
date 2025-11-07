

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"




void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::Cardrep() {
		Expect(3 /* "JUEGO" */);
		Expect(_ident);
		gameType = wcharToString(t->val); 
		intermediateCode.emitGame(gameType); 
		Estado();
		Expect(4 /* "PREDECIR" */);
		Expect(5 /* "jugada" */);
		intermediateCode.emitHalt(); 
}

void Parser::Estado() {
		if (la->kind == 6 /* "BLACKJACK_JUGADOR" */) {
			EstadoBlackjack();
		} else if (la->kind == 12 /* "TRUCO_JUGADOR" */) {
			EstadoTruco();
		} else SynErr(16);
}

void Parser::EstadoBlackjack() {
		Expect(6 /* "BLACKJACK_JUGADOR" */);
		Expect(7 /* "[" */);
		ListaCartasJugador();
		Expect(8 /* "]" */);
		if (playerCards.size() < 2) {
		   SemErr(L"El jugador debe tener al menos 2 cartas");
		}
		if (playerCards.size() > 11) {
		   SemErr(L"El jugador no puede tener más de 11 cartas");
		} 
		
		Expect(9 /* "BLACKJACK_DEALER" */);
		Expect(7 /* "[" */);
		ListaCartasDealer();
		Expect(8 /* "]" */);
		if (dealerCards.size() < 1) {
		   SemErr(L"El dealer debe tener al menos 1 carta");
		}
		
}

void Parser::EstadoTruco() {
		Expect(12 /* "TRUCO_JUGADOR" */);
		Expect(7 /* "[" */);
		ListaCartasTrucoJugador();
		Expect(8 /* "]" */);
		if (playerCards.empty()) {
		   SemErr(L"El jugador debe tener al menos 1 carta");
		}
		if (playerCards.size() > 3) {
		   SemErr(L"El jugador no puede tener más de 3 cartas");
		} 
		
		Expect(13 /* "TRUCO_MESA_JUGADOR" */);
		Expect(7 /* "[" */);
		ListaCartasMesaJugador();
		Expect(8 /* "]" */);
		Expect(14 /* "TRUCO_MESA_OPONENTE" */);
		Expect(7 /* "[" */);
		ListaCartasMesaOponente();
		Expect(8 /* "]" */);
		if (mesaJugadorCards.size() > 3) {
		   SemErr(L"No se pueden jugar más de 3 cartas en la mesa");
		}
		if (mesaOponenteCards.size() > 3) {
		   SemErr(L"El oponente no puede jugar más de 3 cartas");
		}
		
		// Total cartas (mano + mesa) debe ser <= 3
		size_t totalJugador = playerCards.size() + mesaJugadorCards.size();
		if (totalJugador > 3) {
		   SemErr(L"Total cartas jugador (mano + mesa) excede 3");
		}
		
		
		// Diferencia de rondas máxima 1
		int diff = abs((int)mesaJugadorCards.size() - (int)mesaOponenteCards.size());
		if (diff > 1) {
		   SemErr(L"Diferencia de cartas jugadas debe ser máximo 1");
		}
		
		// Las cartas en mesa no pueden estar en mano
		std::set<std::string> cartasMano;
		for (const auto& c : playerCards) cartasMano.insert(c);
		for (const auto& c : mesaJugadorCards) {
		   if (cartasMano.count(c)) {
		       std::wstring msg = L"Carta en mesa también está en mano: ";
		       std::wstring cardW(c.begin(), c.end());
		       msg += cardW;
		       SemErr(msg.c_str());
		   }
		}
		
}

void Parser::ListaCartasJugador() {
		CartaJugador();
		while (la->kind == 10 /* "," */) {
			Get();
			CartaJugador();
		}
}

void Parser::ListaCartasDealer() {
		CartaDealer();
}

void Parser::CartaJugador() {
		ValorCarta();
		std::string valor = wcharToString(t->val); 
		Expect(11 /* "_" */);
		Expect(_ident);
		std::string palo = wcharToString(t->val); 
		std::string carta = valor + "_" + palo;
		playerCards.push_back(carta);
		intermediateCode.emitLoadPlayer(carta); 
}

void Parser::CartaDealer() {
		ValorCarta();
		std::string valor = wcharToString(t->val); 
		Expect(11 /* "_" */);
		Expect(_ident);
		std::string palo = wcharToString(t->val); 
		std::string carta = valor + "_" + palo;
		dealerCards.push_back(carta);
		intermediateCode.emitLoadDealer(carta); 
}

void Parser::ValorCarta() {
		if (la->kind == _number) {
			Get();
		} else if (la->kind == _ident) {
			Get();
		} else SynErr(17);
}

void Parser::ListaCartasTrucoJugador() {
		CartaTrucoJugador();
		while (la->kind == 10 /* "," */) {
			Get();
			CartaTrucoJugador();
		}
}

void Parser::ListaCartasMesaJugador() {
		if (la->kind == _ident || la->kind == _number) {
			CartaMesaJugador();
			while (la->kind == 10 /* "," */) {
				Get();
				CartaMesaJugador();
			}
		}
}

void Parser::ListaCartasMesaOponente() {
		if (la->kind == _ident || la->kind == _number) {
			CartaMesaOponente();
			while (la->kind == 10 /* "," */) {
				Get();
				CartaMesaOponente();
			}
		}
}

void Parser::CartaTrucoJugador() {
		ValorCarta();
		std::string valor = wcharToString(t->val); 
		Expect(11 /* "_" */);
		Expect(_ident);
		std::string palo = wcharToString(t->val); 
		std::string carta = valor + "_" + palo;
		playerCards.push_back(carta);
		intermediateCode.emitLoadPlayer(carta); 
}

void Parser::CartaMesaJugador() {
		ValorCarta();
		std::string valor = wcharToString(t->val); 
		Expect(11 /* "_" */);
		Expect(_ident);
		std::string palo = wcharToString(t->val); 
		std::string carta = valor + "_" + palo;
		mesaJugadorCards.push_back(carta);
		intermediateCode.emitLoadMesaPlayer(carta); 
}

void Parser::CartaMesaOponente() {
		ValorCarta();
		std::string valor = wcharToString(t->val); 
		Expect(11 /* "_" */);
		Expect(_ident);
		std::string palo = wcharToString(t->val); 
		std::string carta = valor + "_" + palo;
		mesaOponenteCards.push_back(carta);
		intermediateCode.emitLoadMesaOpponent(carta); 
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	Cardrep();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 15;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[1][17] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"number expected"); break;
			case 3: s = coco_string_create(L"\"JUEGO\" expected"); break;
			case 4: s = coco_string_create(L"\"PREDECIR\" expected"); break;
			case 5: s = coco_string_create(L"\"jugada\" expected"); break;
			case 6: s = coco_string_create(L"\"BLACKJACK_JUGADOR\" expected"); break;
			case 7: s = coco_string_create(L"\"[\" expected"); break;
			case 8: s = coco_string_create(L"\"]\" expected"); break;
			case 9: s = coco_string_create(L"\"BLACKJACK_DEALER\" expected"); break;
			case 10: s = coco_string_create(L"\",\" expected"); break;
			case 11: s = coco_string_create(L"\"_\" expected"); break;
			case 12: s = coco_string_create(L"\"TRUCO_JUGADOR\" expected"); break;
			case 13: s = coco_string_create(L"\"TRUCO_MESA_JUGADOR\" expected"); break;
			case 14: s = coco_string_create(L"\"TRUCO_MESA_OPONENTE\" expected"); break;
			case 15: s = coco_string_create(L"??? expected"); break;
			case 16: s = coco_string_create(L"invalid Estado"); break;
			case 17: s = coco_string_create(L"invalid ValorCarta"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}


