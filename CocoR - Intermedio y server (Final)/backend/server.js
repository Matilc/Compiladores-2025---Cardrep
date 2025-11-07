const express = require('express');
const cors = require('cors');
const { exec } = require('child_process');
const fs = require('fs').promises;
const path = require('path');
const util = require('util');
const execPromise = util.promisify(exec);

const app = express();
const PORT = 3001;

// Middlewares
app.use(cors());
app.use(express.json());

// Directorio donde está el compilador
const COMPILER_DIR = path.join(__dirname, 'compiler');
const CARDREP_EXE = path.join(COMPILER_DIR, 'Cardrep.exe'); // o 'cardrep' en Linux/Mac

// Endpoint principal para compilar y ejecutar
app.post('/api/compile', async (req, res) => {
    const { gameType, playerCards, dealerCards, mesaJugador, mesaOponente } = req.body;
    
    try {
        // 1. Generar archivo .card
        const cardContent = generateCardFile(gameType, playerCards, dealerCards, mesaJugador, mesaOponente);
        const inputFile = path.join(COMPILER_DIR, 'test.card');
        await fs.writeFile(inputFile, cardContent, 'utf8');
        
        console.log('📝 Archivo .card generado');
        
        // 2. Ejecutar cardrep.exe para generar .cpp
        console.log('🔧 Ejecutando compilador Cardrep...');
        const { stdout: compilerOutput, stderr: compilerError } = await execPromise(
            `cd "${COMPILER_DIR}" && ${path.basename(CARDREP_EXE)}`
        );
        
        if (compilerError) {
            console.error('Error del compilador:', compilerError);
            return res.status(400).json({ 
                success: false, 
                error: 'Error de compilación',
                details: compilerError 
            });
        }
        
        console.log('✅ Archivo .cpp generado');
        
        // 3. Compilar el .cpp generado
        console.log('🔨 Compilando C++...');
        const cppFile = path.join(COMPILER_DIR, 'output.cpp');
        const exeFile = path.join(COMPILER_DIR, 'program.exe');
        
        await execPromise(
            `g++ "${cppFile}" -o "${exeFile}"`
        );
        
        console.log('✅ Ejecutable generado');
        
        // 4. Ejecutar el programa final
        console.log('▶️ Ejecutando programa...');
        const { stdout: programOutput } = await execPromise(
            `cd "${COMPILER_DIR}" && ${path.basename(exeFile)}`
        );
        
        console.log('✅ Ejecución completa');
        
        // 5. Parsear resultado
        const result = parseProgramOutput(programOutput, gameType);
        
        // 6. Leer archivos generados (opcional, para debug)
        const generatedCard = await fs.readFile(inputFile, 'utf8');
        const generatedCpp = await fs.readFile(cppFile, 'utf8');
        
        res.json({
            success: true,
            result,
            output: programOutput,
            files: {
                card: generatedCard,
                cpp: generatedCpp
            }
        });
        
    } catch (error) {
        console.error('❌ Error:', error);
        res.status(500).json({
            success: false,
            error: error.message,
            stack: error.stack
        });
    }
});

// Generar contenido del archivo .card
function generateCardFile(gameType, playerCards, dealerCards, mesaJugador, mesaOponente) {
    let content = `JUEGO ${gameType}\n\n`;
    
    if (gameType === 'Blackjack') {
        content += `BLACKJACK_JUGADOR [${playerCards.join(', ')}]\n`;
        content += `BLACKJACK_DEALER [${dealerCards.join(', ')}]\n`;
    } else if (gameType === 'Truco') {
        content += `TRUCO_JUGADOR [${playerCards.join(', ')}]\n`;
        content += `TRUCO_MESA_JUGADOR [${mesaJugador.join(', ')}]\n`;
        content += `TRUCO_MESA_OPONENTE [${mesaOponente.join(', ')}]\n`;
    }
    
    content += `\nPREDECIR jugada`;
    return content;
}

// Parsear salida del programa
function parseProgramOutput(output, gameType) {
    const lines = output.split('\n');
    
    if (gameType === 'Blackjack') {
        const recommendation = lines.find(l => l.includes('RECOMENDACIÓN:'));
        const playerSum = lines.find(l => l.includes('Suma jugador:'));
        const dealerCard = lines.find(l => l.includes('Carta dealer:'));
        
        return {
            recommendation: recommendation?.split(':')[1]?.trim() || 'No disponible',
            playerSum: playerSum?.split(':')[1]?.trim() || '0',
            dealerCard: dealerCard?.split(':')[1]?.trim() || '0',
            fullOutput: output
        };
    } else if (gameType === 'Truco') {
        const movement = lines.find(l => l.includes('MOVIMIENTO:'));
        const truco = lines.find(l => l.includes('TRUCO:'));
        const envido = lines.find(l => l.includes('ENVIDO:'));
        
        return {
            movement: movement?.split(':')[1]?.trim() || 'No disponible',
            truco: truco?.split(':')[1]?.trim() || 'No disponible',
            envido: envido?.split(':')[1]?.trim() || 'No disponible',
            fullOutput: output
        };
    }
    
    return { fullOutput: output };
}

// Endpoint de salud
app.get('/api/health', (req, res) => {
    res.json({ status: 'ok', message: 'Cardrep server running' });
});

// Iniciar servidor
app.listen(PORT, () => {
    console.log(`🚀 Cardrep backend corriendo en http://localhost:${PORT}`);
    console.log(`📂 Directorio del compilador: ${COMPILER_DIR}`);
});