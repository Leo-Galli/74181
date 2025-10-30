MANUALE UTENTE – SIMULATORE ALU 74181
====================================

INTRODUZIONE
------------

Il Simulatore ALU 74181 è un programma scritto in C (standard C99) che riproduce il funzionamento del chip storico Texas Instruments 74181, una delle prime Unità Aritmetiche e Logiche integrate.  
Questo software consente di osservare e sperimentare come un’ALU elabora operazioni logiche e matematiche, offrendo una visione diretta sui meccanismi alla base dei microprocessori.

Il simulatore amplia le capacità del chip originale fino a 32 bit, utilizzando otto unità 74181 collegate in cascata in modalità PIPO (Parallel-In Parallel-Out).  
È pensato per utenti di ogni livello, anche senza conoscenze di elettronica, ed è utile per studenti, curiosi e appassionati di retrocomputing.

------------------------------------------------------------

1. AVVIO DEL PROGRAMMA
----------------------

REQUISITI:
- Sistema operativo: Windows 7 o successivo, Linux o macOS.
- Compilatore C compatibile con lo standard C99 (GCC, Clang, MinGW).
- Nessuna libreria esterna richiesta.

INSTALLAZIONE:
1. Clona il repository:
   git clone https://github.com/Leo-Galli/74181.git
   cd 74181
2. Compila:
   gcc main.c -o simulatore
3. Esegui:
   ./simulatore    (Linux/macOS)
   simulatore.exe  (Windows)

All’avvio apparirà il menu principale.

------------------------------------------------------------

2. MENU PRINCIPALE
------------------

╔════════════════════════════════════════════════════════════╗
║                ________|          |________                ║
║               |       /   ||||||   \\      |               ║
║               |     ,'              `.     |               ║
║               |   ,'                  `.   |               ║
║               | ,'   ||||||||||||||||   `. |               ║
║               ,'  /____________________\\  `.              ║
║              /______________________________\\             ║
║             |                                |             ║
║             |                                |             ║
║             |                                |             ║
║             |________________________________|             ║
║               |____________________------__|               ║
║                                                            ║
║   ,----------------------------------------------------,   ║
║   | [][][][][]  [][][][][]  [][][][]  [][__]  [][][][] |   ║
║   |                                                    |   ║
║   |  [][][][][][][][][][][][][][_]    [][][]  [][][][] |   ║
║   |  [_][][][][][][][][][][][][][ |   [][][]  [][][][] |   ║
║   | [][_][][][][][][][][][][][][]||     []    [][][][] |   ║
║   | [__][][][][][][][][][][][][__]    [][][]  [][][]|| |   ║
║   |   [__][________________][__]              [__][]|| |   ║
║   `----------------------------------------------------'   ║
║                                                            ║
║                    SIMULATORE ALU 74181                    ║
║                                                            ║
╠════════════════════════════════════════════════════════════╣
║    1. Operazioni Logiche (ALU 74181 - Singolo)             ║
║    2. Operazioni Logiche (ALU 74181 - Singolo con clock)   ║
║    3. Operazioni Algebriche                                ║
║    4. Convertitore Binario → Decimale                      ║
║    5. Convertitore Decimale → Binario                      ║
║    6. ALU in Modalità PIPO (32 bit - 8x74181)              ║
║    7. ALU in Modalità PIPO (32 bit - 8x74181 con clock)    ║
║    8. Visualizza Memoria                                   ║
║    9. Calcolo del Clock                                    ║
║    0. Esci                                                 ║
╚════════════════════════════════════════════════════════════╝

------------------------------------------------------------

3. OPERAZIONI PRINCIPALI
------------------------

3.1 OPERAZIONI LOGICHE (ALU SINGOLO)
Simula un chip 74181 a 4 bit.  
L’utente inserisce i valori binari di A e B, il carry-in (Cn), la modalità (0=aritmetica, 1=logica) e i segnali di controllo (S0–S3).  
L’output mostra il risultato, i flag di stato e la rappresentazione decimale.  
Il risultato è salvato in “risultati_alu_74181.txt”.

3.2 OPERAZIONI LOGICHE CON CLOCK
Come la precedente ma con un ritardo simulato di 2 secondi per riprodurre la propagazione del segnale reale.

3.3 OPERAZIONI ALGEBRICHE
Calcolatrice integrata per eseguire:
- somme e sottrazioni;
- moltiplicazioni e divisioni;
- espressioni con parentesi.  
Esempio: (10 + 5) * 3 - 4  
I risultati vengono salvati in “risultato_operazione.txt”.

3.4 CONVERTITORE BINARIO → DECIMALE
Converte un numero binario (massimo 32 bit) in decimale.
Esempio:
Input: 1101  
Output: 13

3.5 CONVERTITORE DECIMALE → BINARIO
Conversione inversa.  
Input: 25  
Output: 11001  
Risultato salvato in “risultati_bin.txt”.

3.6 ALU IN MODALITÀ PIPO (32 BIT - 8x74181)
Otto ALU a 4 bit collegate in cascata per simulare un sistema a 32 bit.  
Esegue operazioni logiche e aritmetiche complete, salvando i risultati in “risultati_alu32.txt”.

3.7 ALU PIPO CON CLOCK
Simile alla modalità precedente ma con temporizzazione simulata.

3.8 VISUALIZZA MEMORIA
Mostra la cronologia completa delle operazioni eseguite, memorizzate dinamicamente in RAM.

3.9 CALCOLO DEL CLOCK
Analizza la CPU e stima la frequenza del clock.  
Se non riconosciuta, imposta 1 GHz di default.  
Esegue la simulazione di 1000 cicli.

------------------------------------------------------------

4. INPUT DA FILE
----------------

| Funzione        | File di Input        | Formato                      |
|-----------------|---------------------|------------------------------|
| ALU 4-bit       | input_alu.txt        | Cn: <0> M: <1> A0: <1> ...   |
| ALU 32-bit      | input_alu32.txt      | Operando A: <255> ...        |
| Bin → Dec       | input_bin.txt        | Numero Binario: <1101>       |
| Dec → Bin       | input_dec.txt        | Numero Decimale: <13>        |

Se i file non esistono, il programma li genera con esempi predefiniti.

------------------------------------------------------------

5. SALVATAGGIO RISULTATI
------------------------

| Tipo Operazione | File di Output               |
|-----------------|------------------------------|
| ALU 4-bit       | risultati_alu_74181.txt      |
| ALU 32-bit      | risultati_alu32.txt          |
| Bin→Dec         | risultati_dec.txt            |
| Dec→Bin         | risultati_bin.txt            |
| Algebriche      | risultato_operazione.txt     |

I file vengono sovrascritti a ogni sessione.

------------------------------------------------------------

6. COMPATIBILITÀ
----------------

Compatibile con Windows, Linux e macOS.
Usa automaticamente:
- nanosleep (Unix)
- Sleep (Windows)
- popen / _popen (lettura comandi sistema)

Testato su Ubuntu 22.04, Fedora 38, Windows 10/11 e macOS Sonoma.

------------------------------------------------------------

7. RISOLUZIONE PROBLEMI
------------------------

- Il programma non parte: controlla la presenza del compilatore nel PATH.
- I file non si trovano: sono nella stessa cartella dell’eseguibile.
- Chiusura immediata: avvia da terminale per leggere gli errori.
- Input errato: assicurati che i valori siano tra < >.
- Programma lento: disattiva modalità con clock.

------------------------------------------------------------

8. NOTE STORICHE
----------------

Il chip 74181, prodotto da Texas Instruments nel 1970, fu la prima ALU integrata della storia e venne utilizzata in sistemi come il PDP-11.  
Il simulatore riproduce le sue funzioni in modo fedele, offrendo una comprensione pratica del funzionamento interno delle CPU.

------------------------------------------------------------

9. AUTORE E LICENZA
-------------------

Autore: Leonardo Galli  
Ultimo aggiornamento: 30 Ottobre 2025  
Licenza: GNU GPL v3.0  
Repository: https://github.com/Leo-Galli/74181

------------------------------------------------------------

10. CONCLUSIONE
---------------

Il Simulatore ALU 74181 permette di esplorare i principi fondamentali dell’elaborazione digitale, unendo semplicità e accuratezza storica.  
È uno strumento ideale per scoprire, comprendere e visualizzare i meccanismi logici che costituiscono il cuore di ogni processore moderno.
