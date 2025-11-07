import React, { useState } from 'react';
import { Play, Code, AlertCircle, CheckCircle, Loader } from 'lucide-react';

// Configuración de cartas
const SUITS = ['espada', 'basto', 'oro', 'copa'];
const VALUES = ['1', '2', '3', '4', '5', '6', '7', '10', '11', '12'];
const BLACKJACK_VALUES = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K'];
const BLACKJACK_SUITS = ['espada', 'corazon', 'trebol', 'diamante'];

export default function CardrepUI() {
  const [gameType, setGameType] = useState('');
  const [playerCards, setPlayerCards] = useState([]);
  const [dealerCards, setDealerCards] = useState([]);
  const [mesaJugador, setMesaJugador] = useState([]);
  const [mesaOponente, setMesaOponente] = useState([]);
  const [loading, setLoading] = useState(false);
  const [result, setResult] = useState(null);
  const [error, setError] = useState(null);
  const [showCode, setShowCode] = useState(false);

  const resetAll = () => {
    setPlayerCards([]);
    setDealerCards([]);
    setMesaJugador([]);
    setMesaOponente([]);
    setResult(null);
    setError(null);
  };

  const addCard = (list, setList, value, suit) => {
    const card = `${value}_${suit}`;
    if (!list.includes(card)) {
      setList([...list, card]);
    }
  };

  const removeCard = (list, setList, index) => {
    setList(list.filter((_, i) => i !== index));
  };

  const handleCompile = async () => {
    setLoading(true);
    setError(null);
    setResult(null);

    try {
      const response = await fetch('http://localhost:3001/api/compile', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          gameType,
          playerCards,
          dealerCards: gameType === 'Blackjack' ? dealerCards : [],
          mesaJugador: gameType === 'Truco' ? mesaJugador : [],
          mesaOponente: gameType === 'Truco' ? mesaOponente : []
        })
      });

      const data = await response.json();

      if (data.success) {
        setResult(data);
      } else {
        setError(data.error + (data.details ? `: ${data.details}` : ''));
      }
    } catch (err) {
      setError('Error de conexión con el servidor: ' + err.message);
    } finally {
      setLoading(false);
    }
  };

  const canCompile = () => {
    if (gameType === 'Blackjack') {
      return playerCards.length >= 2 && dealerCards.length >= 1;
    } else if (gameType === 'Truco') {
      return playerCards.length >= 1 && playerCards.length <= 3;
    }
    return false;
  };

  if (!gameType) {
    return (
      <div className="min-h-screen bg-gradient-to-br from-slate-900 via-purple-900 to-slate-900 flex items-center justify-center p-4">
        <div className="text-center">
          <h1 className="text-5xl font-bold text-white mb-4">🎮 Cardrep Compiler</h1>
          <p className="text-gray-300 mb-8">Selecciona el juego para comenzar</p>
          <div className="flex gap-4 justify-center">
            <button
              onClick={() => { setGameType('Blackjack'); resetAll(); }}
              className="bg-red-600 hover:bg-red-700 text-white px-8 py-4 rounded-lg text-xl font-bold transition-all transform hover:scale-105 shadow-lg"
            >
              🃏 Blackjack
            </button>
            <button
              onClick={() => { setGameType('Truco'); resetAll(); }}
              className="bg-blue-600 hover:bg-blue-700 text-white px-8 py-4 rounded-lg text-xl font-bold transition-all transform hover:scale-105 shadow-lg"
            >
              🎴 Truco
            </button>
          </div>
        </div>
      </div>
    );
  }

  const values = gameType === 'Blackjack' ? BLACKJACK_VALUES : VALUES;
  const suits = gameType === 'Blackjack' ? BLACKJACK_SUITS : SUITS;

  return (
    <div className="min-h-screen bg-gradient-to-br from-slate-900 via-purple-900 to-slate-900 p-4">
      <div className="max-w-7xl mx-auto">
        {/* Header */}
        <div className="flex justify-between items-center mb-6">
          <div>
            <h1 className="text-4xl font-bold text-white">
              {gameType === 'Blackjack' ? '🃏 Blackjack' : '🎴 Truco'}
            </h1>
            <p className="text-gray-300">Compilador de análisis de jugadas</p>
          </div>
          <button
            onClick={() => { setGameType(''); resetAll(); }}
            className="bg-gray-700 hover:bg-gray-600 text-white px-4 py-2 rounded-lg transition-all"
          >
            ← Cambiar juego
          </button>
        </div>

        <div className="grid lg:grid-cols-2 gap-6">
          {/* Panel de selección */}
          <div className="bg-white/10 backdrop-blur-md rounded-xl p-6 border border-white/20">
            <h2 className="text-2xl font-bold text-white mb-4">Configurar mano</h2>

            {/* Cartas del jugador */}
            <div className="mb-6">
              <h3 className="text-lg font-semibold text-white mb-2">
                Cartas del jugador ({playerCards.length}/{gameType === 'Blackjack' ? '2-11' : '1-3'})
              </h3>
              <div className="flex flex-wrap gap-2 mb-3 min-h-[50px] bg-black/30 p-3 rounded-lg">
                {playerCards.map((card, i) => (
                  <span key={i} className="bg-blue-500 text-white px-3 py-1 rounded-full text-sm flex items-center gap-2">
                    {card}
                    <button onClick={() => removeCard(playerCards, setPlayerCards, i)} className="hover:text-red-300">×</button>
                  </span>
                ))}
              </div>
              <div className="grid grid-cols-5 gap-2">
                {suits.map(suit => (
                  <select
                    key={suit}
                    onChange={(e) => e.target.value && addCard(playerCards, setPlayerCards, e.target.value, suit)}
                    className="bg-gray-700 text-white p-2 rounded text-sm"
                    value=""
                  >
                    <option value="">🂠 {suit}</option>
                    {values.map(val => (
                      <option key={val} value={val}>{val}</option>
                    ))}
                  </select>
                ))}
              </div>
            </div>

            {/* Cartas del dealer (Blackjack) */}
            {gameType === 'Blackjack' && (
              <div className="mb-6">
                <h3 className="text-lg font-semibold text-white mb-2">Carta del dealer (1)</h3>
                <div className="flex flex-wrap gap-2 mb-3 min-h-[50px] bg-black/30 p-3 rounded-lg">
                  {dealerCards.map((card, i) => (
                    <span key={i} className="bg-red-500 text-white px-3 py-1 rounded-full text-sm flex items-center gap-2">
                      {card}
                      <button onClick={() => removeCard(dealerCards, setDealerCards, i)} className="hover:text-red-300">×</button>
                    </span>
                  ))}
                </div>
                <div className="grid grid-cols-5 gap-2">
                  {BLACKJACK_SUITS.map(suit => (
                    <select
                      key={suit}
                      onChange={(e) => {
                        if (e.target.value && dealerCards.length === 0) {
                          addCard(dealerCards, setDealerCards, e.target.value, suit);
                        }
                      }}
                      className="bg-gray-700 text-white p-2 rounded text-sm"
                      value=""
                      disabled={dealerCards.length >= 1}
                    >
                      <option value="">🂠 {suit}</option>
                      {BLACKJACK_VALUES.map(val => (
                        <option key={val} value={val}>{val}</option>
                      ))}
                    </select>
                  ))}
                </div>
              </div>
            )}

            {/* Mesa jugador (Truco) */}
            {gameType === 'Truco' && (
              <>
                <div className="mb-6">
                  <h3 className="text-lg font-semibold text-white mb-2">Mesa jugador (0-3)</h3>
                  <div className="flex flex-wrap gap-2 mb-3 min-h-[50px] bg-black/30 p-3 rounded-lg">
                    {mesaJugador.map((card, i) => (
                      <span key={i} className="bg-green-500 text-white px-3 py-1 rounded-full text-sm flex items-center gap-2">
                        {card}
                        <button onClick={() => removeCard(mesaJugador, setMesaJugador, i)} className="hover:text-red-300">×</button>
                      </span>
                    ))}
                  </div>
                  <div className="grid grid-cols-4 gap-2">
                    {SUITS.map(suit => (
                      <select
                        key={suit}
                        onChange={(e) => e.target.value && addCard(mesaJugador, setMesaJugador, e.target.value, suit)}
                        className="bg-gray-700 text-white p-2 rounded text-sm"
                        value=""
                      >
                        <option value="">🂠 {suit}</option>
                        {VALUES.map(val => (
                          <option key={val} value={val}>{val}</option>
                        ))}
                      </select>
                    ))}
                  </div>
                </div>

                <div className="mb-6">
                  <h3 className="text-lg font-semibold text-white mb-2">Mesa oponente (0-3)</h3>
                  <div className="flex flex-wrap gap-2 mb-3 min-h-[50px] bg-black/30 p-3 rounded-lg">
                    {mesaOponente.map((card, i) => (
                      <span key={i} className="bg-purple-500 text-white px-3 py-1 rounded-full text-sm flex items-center gap-2">
                        {card}
                        <button onClick={() => removeCard(mesaOponente, setMesaOponente, i)} className="hover:text-red-300">×</button>
                      </span>
                    ))}
                  </div>
                  <div className="grid grid-cols-4 gap-2">
                    {SUITS.map(suit => (
                      <select
                        key={suit}
                        onChange={(e) => e.target.value && addCard(mesaOponente, setMesaOponente, e.target.value, suit)}
                        className="bg-gray-700 text-white p-2 rounded text-sm"
                        value=""
                      >
                        <option value="">🂠 {suit}</option>
                        {VALUES.map(val => (
                          <option key={val} value={val}>{val}</option>
                        ))}
                      </select>
                    ))}
                  </div>
                </div>
              </>
            )}

            {/* Botón de compilar */}
            <button
              onClick={handleCompile}
              disabled={!canCompile() || loading}
              className={`w-full py-4 rounded-lg font-bold text-lg transition-all ${
                canCompile() && !loading
                  ? 'bg-gradient-to-r from-green-500 to-emerald-600 hover:from-green-600 hover:to-emerald-700 text-white shadow-lg transform hover:scale-105'
                  : 'bg-gray-600 text-gray-400 cursor-not-allowed'
              }`}
            >
              {loading ? (
                <span className="flex items-center justify-center gap-2">
                  <Loader className="animate-spin" /> Compilando...
                </span>
              ) : (
                <span className="flex items-center justify-center gap-2">
                  <Play /> Compilar y Analizar
                </span>
              )}
            </button>
          </div>

          {/* Panel de resultados */}
          <div className="bg-white/10 backdrop-blur-md rounded-xl p-6 border border-white/20">
            <h2 className="text-2xl font-bold text-white mb-4">Resultado</h2>

            {error && (
              <div className="bg-red-500/20 border border-red-500 rounded-lg p-4 mb-4">
                <div className="flex items-start gap-2">
                  <AlertCircle className="text-red-400 flex-shrink-0" />
                  <div>
                    <p className="font-semibold text-red-200">Error</p>
                    <p className="text-red-300 text-sm">{error}</p>
                  </div>
                </div>
              </div>
            )}

            {result && (
              <div className="space-y-4">
                <div className="bg-green-500/20 border border-green-500 rounded-lg p-4">
                  <div className="flex items-center gap-2 mb-3">
                    <CheckCircle className="text-green-400" />
                    <span className="font-semibold text-green-200">Compilación exitosa</span>
                  </div>

                  {gameType === 'Blackjack' && result.result && (
                    <div className="space-y-2 text-white">
                      <p className="text-xl font-bold text-yellow-300">
                        {result.result.recommendation}
                      </p>
                      <p>Suma: <span className="font-mono">{result.result.playerSum}</span></p>
                      <p>Carta dealer: <span className="font-mono">{result.result.dealerCard}</span></p>
                    </div>
                  )}

                  {gameType === 'Truco' && result.result && (
                    <div className="space-y-2 text-white">
                      <p><strong>Movimiento:</strong> {result.result.movement}</p>
                      <p><strong>Truco:</strong> {result.result.truco}</p>
                      <p><strong>Envido:</strong> {result.result.envido}</p>
                    </div>
                  )}
                </div>

                <button
                  onClick={() => setShowCode(!showCode)}
                  className="flex items-center gap-2 text-blue-300 hover:text-blue-200"
                >
                  <Code size={20} />
                  {showCode ? 'Ocultar' : 'Ver'} código generado
                </button>

                {showCode && result.files && (
                  <div className="space-y-3">
                    <div>
                      <h3 className="text-sm font-semibold text-gray-300 mb-1">input.card</h3>
                      <pre className="bg-black/50 p-3 rounded text-xs text-gray-300 overflow-x-auto">
                        {result.files.card}
                      </pre>
                    </div>
                    <div>
                      <h3 className="text-sm font-semibold text-gray-300 mb-1">output.cpp</h3>
                      <pre className="bg-black/50 p-3 rounded text-xs text-gray-300 overflow-x-auto max-h-60">
                        {result.files.cpp}
                      </pre>
                    </div>
                  </div>
                )}
              </div>
            )}

            {!result && !error && !loading && (
              <div className="text-center text-gray-400 py-12">
                <Code size={48} className="mx-auto mb-3 opacity-50" />
                <p>Configura las cartas y presiona compilar</p>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}