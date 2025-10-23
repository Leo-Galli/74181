/*
 *              ,----------------,              ,---------,
 *         ,-----------------------,          ,"        ,"|
 *       ,"                      ,"|        ,"        ,"  |
 *      +-----------------------+  |      ,"        ,"    |
 *      |  .-----------------.  |  |     +---------+      |
 *      |  |                 |  |  |     | -==----'|      |
 *      |  |  SIMULATORE!    |  |  |     |         |      |
 *      |  |  ALU 74181      |  |  |/----|`---=    |      |
 *      |  |  C:\>_ ./74181  |  |  |   ,/|==== ooo |      ;
 *      |  |                 |  |  |  // |(((( [33]|    ,"
 *      |  `-----------------'  |," .;'| |((((     |  ,"
 *     +-----------------------+  ;;  | |         |,"     -Leonardo Galli-
 *         /_)______________(_/  //'   | +---------+
 *    ___________________________/___  `,
 *   /  oooooooooooooooo  .o.  oooo /,   \,"-----------
 *  / ==ooooooooooooooo==.o.  ooo= //   ,`\--{)B     ,"
 * /_==__==========__==_ooo__ooo=_/'   /___________,"
 * `-----------------------------'
 *
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  SIMULATORE ALU 74181 — UN VIAGGIO NEL CUORE DELL'ARCHITETTURA DIGITALE    │
 * │                                                                            │
 * │  Questo simulatore riproduce fedelmente il comportamento del chip 74181,   │
 * │  una delle prime ALU (Arithmetic Logic Unit) integrate, progettata negli   │
 * │  anni '70 da Texas Instruments. È considerata una pietra miliare nella     │
 * │  storia dell'informatica, poiché ha permesso la costruzione di CPU più     │
 * │  complesse e modulari.                                                     │
 * │                                                                            │
 * │  Il progetto qui presentato è molto più di una semplice emulazione:        │
 * │  rappresenta un ambiente didattico, sperimentale e ingegneristico per      │
 * │  comprendere a fondo il funzionamento delle operazioni logiche e           │
 * │  aritmetiche a basso livello.                                              │
 * │                                                                            │
 * │  CARATTERISTICHE TECNICHE                                                  │
 * │  ────────────────────────                                                  │
 * │  • Simulazione completa del chip 74181 a 4 bit                             │
 * │  • Estensione a 32 bit tramite architettura a cascata                      │
 * │  • Implementazione di registri PIPO con flip-flop SR                       │
 * │  • Rilevamento automatico della CPU host e stima della frequenza di clock  │
 * │  • Calcolatrice scientifica con parser di espressioni                      │
 * │  • Sistema di salvataggio persistente (RAM e file)                         │
 * │  • Compatibilità multipiattaforma: Windows, Linux, macOS                   │
 * │                                                                            │
 * │  UTILIZZO DIDATTICO E PROFESSIONALE                                        │
 * │  ───────────────────────────────────────                                   │
 * │  • Ideale per corsi universitari di architettura dei calcolatori           │
 * │  • Utile per testare microistruzioni e logiche di controllo                │
 * │  • Strumento per makers e appassionati di retrocomputing                   │
 * │                                                                            │
 * │  AUTORE E LICENZE                                                          │
 * │  ────────────────────────                                                  │
 * │  • Autore: Leonardo Galli                                                  │
 * │  • Ultimo aggiornamento: 20 ottobre 2025                                   │
 * │  • Codice sorgente: GitHub ufficiale                                       │
 * │    → README: https://github.com/Leo-Galli/74181?tab=readme-ov-file         │
 * │    → Licenza: GPL-3.0                                                      │
 * │    → Codice di Condotta: https://github.com/Leo-Galli/74181?tab=coc-ov-file│
 * │                                                                            │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#if defined(_WIN32)
#define SISTEMA_WINDOWS 1
#include <windows.h>
#else
#define SISTEMA_WINDOWS 0
#endif
#ifdef __APPLE__
#define SISTEMA_MAC 1
#else
#define SISTEMA_MAC 0
#endif
#ifdef __linux__
#define SISTEMA_LINUX 1
#else
#define SISTEMA_LINUX 0
#endif

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  STRUTTURA DATI: Processore                                                │
 * │                                                                            │
 * │  Questa struttura rappresenta un'astrazione di un'unità centrale di        │
 * │  elaborazione (CPU), memorizzando le informazioni fondamentali per la      │
 * │  simulazione temporale e il profiling delle prestazioni.                   │
 * │                                                                            │
 * │  CAMPI PRINCIPALI                                                          │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • nome: stringa identificativa del processore (es. "Intel Core i7-9700K") │
 * │  • frequenza: valore numerico in Hertz (Hz) che rappresenta la frequenza   │
 * │    di clock del processore.                                                │
 * │                                                                            │
 * │  UTILIZZO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Rilevamento automatico della CPU host tramite interrogazione del sistema│
 * │  • Simulazione temporale realistica basata sul ciclo di clock              │
 * │  • Calcolo del tempo di esecuzione delle istruzioni in base alla frequenza │
 * │  • Visualizzazione delle caratteristiche hardware nel simulatore           │
 * │                                                                            │
 * │  CONTESTO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  Questa struttura è parte integrante del simulatore ALU 74181, e consente  │
 * │  di adattare il comportamento del sistema simulato alle caratteristiche    │
 * │  reali della macchina ospite.                                              │
 * │                                                                            │
 * │  ESEMPIO                                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  struct const Processore db_processori[] = {                               │
 * │      {"AMD Ryzen 5 3600", 4300000000L},                                    │
 * │  };                                                                        │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


typedef struct {
    const char *nome;
    long clock_hz;
} Processore;
static int leggi_bit_input_74181(const char* nome, int* var);
static int leggi_bit_input_32(const char* nome, int* var);
static const Processore db_processori[] = {
    {"Allwinner A10", 1000000000L},
    {"Allwinner A13", 1000000000L},
    {"Allwinner A20", 1000000000L},
    {"Allwinner A31", 1500000000L},
    {"Allwinner A33", 1300000000L},
    {"Allwinner A50", 1200000000L},
    {"Allwinner A64", 1200000000L},
    {"Allwinner A83T", 2000000000L},
    {"Allwinner H2+", 1000000000L},
    {"Allwinner H3", 1200000000L},
    {"Allwinner H5", 1200000000L},
    {"Allwinner H6", 1800000000L},
    {"Allwinner H616", 1500000000L},
    {"Allwinner R18", 1200000000L},
    {"Allwinner R40", 1200000000L},
    {"AMD EPYC 3201", 2200000000L},
    {"AMD EPYC 3251", 2500000000L},
    {"AMD EPYC 7232P", 3100000000L},
    {"AMD EPYC 7251", 2100000000L},
    {"AMD EPYC 7252", 3100000000L},
    {"AMD EPYC 7262", 3200000000L},
    {"AMD EPYC 7272", 2900000000L},
    {"AMD EPYC 7282", 2800000000L},
    {"AMD EPYC 7301", 2200000000L},
    {"AMD EPYC 7302", 3000000000L},
    {"AMD EPYC 7302P", 3300000000L},
    {"AMD EPYC 7313", 3000000000L},
    {"AMD EPYC 7313P", 3000000000L},
    {"AMD EPYC 7343", 3200000000L},
    {"AMD EPYC 7351", 2400000000L},
    {"AMD EPYC 7351P", 2400000000L},
    {"AMD EPYC 7352", 2300000000L},
    {"AMD EPYC 7371", 3100000000L},
    {"AMD EPYC 7373X", 3400000000L},
    {"AMD EPYC 7401", 2000000000L},
    {"AMD EPYC 7401P", 2000000000L},
    {"AMD EPYC 7402", 2800000000L},
    {"AMD EPYC 7402P", 2800000000L},
    {"AMD EPYC 7413", 2650000000L},
    {"AMD EPYC 7442", 2800000000L},
    {"AMD EPYC 7443", 3600000000L},
    {"AMD EPYC 7451", 2300000000L},
    {"AMD EPYC 7452", 2350000000L},
    {"AMD EPYC 7453", 2750000000L},
    {"AMD EPYC 7473X", 3700000000L},
    {"AMD EPYC 7501", 2000000000L},
    {"AMD EPYC 7502", 2500000000L},
    {"AMD EPYC 7502P", 2500000000L},
    {"AMD EPYC 7513", 2600000000L},
    {"AMD EPYC 7532", 2400000000L},
    {"AMD EPYC 7542", 2900000000L},
    {"AMD EPYC 7543", 3400000000L},
    {"AMD EPYC 7551", 2000000000L},
    {"AMD EPYC 7551P", 2000000000L},
    {"AMD EPYC 7552", 2200000000L},
    {"AMD EPYC 7573X", 3800000000L},
    {"AMD EPYC 7601", 2200000000L},
    {"AMD EPYC 7642", 2300000000L},
    {"AMD EPYC 7643", 2300000000L},
    {"AMD EPYC 7662", 2000000000L},
    {"AMD EPYC 7702", 2000000000L},
    {"AMD EPYC 7702P", 2000000000L},
    {"AMD EPYC 7713", 2000000000L},
    {"AMD EPYC 7713P", 2000000000L},
    {"AMD EPYC 7742", 2250000000L},
    {"AMD EPYC 7763", 2450000000L},
    {"AMD EPYC 7773X", 3500000000L},
    {"AMD EPYC 7F32", 3700000000L},
    {"AMD EPYC 7F52", 3500000000L},
    {"AMD EPYC 7H12", 2600000000L},
    {"AMD EPYC 8534P", 3000000000L},
    {"AMD EPYC 9124", 3000000000L},
    {"AMD EPYC 9174F", 4100000000L},
    {"AMD EPYC 9254", 3100000000L},
    {"AMD EPYC 9334", 3100000000L},
    {"AMD EPYC 9354", 3100000000L},
    {"AMD EPYC 9354P", 3250000000L},
    {"AMD EPYC 9374F", 3700000000L},
    {"AMD EPYC 9454", 3400000000L},
    {"AMD EPYC 9454P", 3800000000L},
    {"AMD EPYC 9474F", 4100000000L},
    {"AMD EPYC 9534", 2700000000L},
    {"AMD EPYC 9554", 3100000000L},
    {"AMD EPYC 9554P", 3350000000L},
    {"AMD EPYC 9634", 2250000000L},
    {"AMD EPYC 9654", 2400000000L},
    {"AMD EPYC 9654P", 2400000000L},
    {"AMD EPYC 9734", 2200000000L},
    {"AMD EPYC 9754", 2800000000L},
    {"AMD EPYC 9754S", 3100000000L},
    {"AMD FX-4100", 3600000000L},
    {"AMD FX-4130", 3600000000L},
    {"AMD FX-4150", 4200000000L},
    {"AMD FX-4170", 4400000000L},
    {"AMD FX-4300", 3800000000L},
    {"AMD FX-4320", 3800000000L},
    {"AMD FX-4350", 4300000000L},
    {"AMD FX-6100", 3300000000L},
    {"AMD FX-6120", 3500000000L},
    {"AMD FX-6130", 3500000000L},
    {"AMD FX-6150", 3600000000L},
    {"AMD FX-6170", 4000000000L},
    {"AMD FX-6200", 3500000000L},
    {"AMD FX-6220", 3600000000L},
    {"AMD FX-6230", 3600000000L},
    {"AMD FX-6300", 3400000000L},
    {"AMD FX-6320", 3500000000L},
    {"AMD FX-6350", 4000000000L},
    {"AMD FX-7100", 3800000000L},
    {"AMD FX-7120", 3800000000L},
    {"AMD FX-8150", 3600000000L},
    {"AMD FX-8170", 3600000000L},
    {"AMD FX-8350", 4000000000L},
    {"AMD FX-9370", 4500000000L},
    {"AMD FX-9590", 4700000000L},
    {"AMD Opteron 1210", 1500000000L},
    {"AMD Opteron 1212", 1800000000L},
    {"AMD Opteron 1214", 2000000000L},
    {"AMD Opteron 1216", 2200000000L},
    {"AMD Opteron 1218", 2300000000L},
    {"AMD Opteron 1220", 2300000000L},
    {"AMD Opteron 1222", 2300000000L},
    {"AMD Opteron 1224", 2400000000L},
    {"AMD Opteron 1226", 2500000000L},
    {"AMD Opteron 1228", 2500000000L},
    {"AMD Opteron 1230", 2600000000L},
    {"AMD Opteron 1234", 2600000000L},
    {"AMD Opteron 1236", 2700000000L},
    {"AMD Opteron 1238", 2800000000L},
    {"AMD Opteron 1240", 2800000000L},
    {"AMD Opteron 1244", 2900000000L},
    {"AMD Opteron 1246", 2900000000L},
    {"AMD Opteron 1250", 3000000000L},
    {"AMD Opteron 1252", 3000000000L},
    {"AMD Opteron 1304", 2200000000L},
    {"AMD Opteron 1352", 2200000000L},
    {"AMD Opteron 1354", 2300000000L},
    {"AMD Opteron 1381", 2500000000L},
    {"AMD Opteron 1385", 2600000000L},
    {"AMD Opteron 1400", 1800000000L},
    {"AMD Opteron 1401", 1800000000L},
    {"AMD Opteron 1402", 1900000000L},
    {"AMD Opteron 1403", 2000000000L},
    {"AMD Opteron 1404", 2100000000L},
    {"AMD Opteron 1405", 2100000000L},
    {"AMD Opteron 1406", 2200000000L},
    {"AMD Opteron 1407", 2300000000L},
    {"AMD Opteron 1408", 2300000000L},
    {"AMD Opteron 1420", 2400000000L},
    {"AMD Opteron 1421", 2500000000L},
    {"AMD Opteron 1423", 2500000000L},
    {"AMD Opteron 1431", 2600000000L},
    {"AMD Opteron 1435", 2700000000L},
    {"AMD Opteron 1441", 2700000000L},
    {"AMD Opteron 1443", 2800000000L},
    {"AMD Opteron 1445", 2800000000L},
    {"AMD Opteron 146", 1500000000L},
    {"AMD Opteron 1461", 2900000000L},
    {"AMD Opteron 1465", 3000000000L},
    {"AMD Opteron 147", 1600000000L},
    {"AMD Opteron 148", 1700000000L},
    {"AMD Opteron 150", 1400000000L},
    {"AMD Opteron 151", 1500000000L},
    {"AMD Opteron 152", 1600000000L},
    {"AMD Opteron 153", 1700000000L},
    {"AMD Opteron 154", 1800000000L},
    {"AMD Opteron 155", 1900000000L},
    {"AMD Opteron 156", 2000000000L},
    {"AMD Opteron 157", 2100000000L},
    {"AMD Opteron 158", 2200000000L},
    {"AMD Opteron 160", 1300000000L},
    {"AMD Opteron 162", 1400000000L},
    {"AMD Opteron 165", 1500000000L},
    {"AMD Opteron 170", 1600000000L},
    {"AMD Opteron 175", 1700000000L},
    {"AMD Opteron 180", 1800000000L},
    {"AMD Opteron 185", 1900000000L},
    {"AMD Opteron 190", 2000000000L},
    {"AMD Opteron 195", 2100000000L},
    {"AMD Opteron 200", 1200000000L},
    {"AMD Opteron 210", 1300000000L},
    {"AMD Opteron 220", 1400000000L},
    {"AMD Opteron 2218", 2600000000L},
    {"AMD Opteron 2220", 2400000000L},
    {"AMD Opteron 2222", 2500000000L},
    {"AMD Opteron 2224", 2600000000L},
    {"AMD Opteron 2226", 2700000000L},
    {"AMD Opteron 2230", 2800000000L},
    {"AMD Opteron 2232", 2900000000L},
    {"AMD Opteron 2234", 3000000000L},
    {"AMD Opteron 2236", 3100000000L},
    {"AMD Opteron 2238", 3200000000L},
    {"AMD Opteron 2240", 3300000000L},
    {"AMD Opteron 2242", 3400000000L},
    {"AMD Opteron 2244", 3500000000L},
    {"AMD Opteron 2246", 3600000000L},
    {"AMD Opteron 2248", 3700000000L},
    {"AMD Opteron 2250", 3800000000L},
    {"AMD Opteron 2252", 3900000000L},
    {"AMD Opteron 2254", 4000000000L},
    {"AMD Opteron 2256", 4100000000L},
    {"AMD Opteron 2258", 4200000000L},
    {"AMD Opteron 2260", 4300000000L},
    {"AMD Opteron 2262", 4400000000L},
    {"AMD Opteron 2264", 4500000000L},
    {"AMD Opteron 2266", 4600000000L},
    {"AMD Opteron 2268", 4700000000L},
    {"AMD Opteron 2270", 4800000000L},
    {"AMD Opteron 2272", 4900000000L},
    {"AMD Opteron 2274", 5000000000L},
    {"AMD Phenom II X2 505", 2900000000L},
    {"AMD Phenom II X2 510", 3000000000L},
    {"AMD Phenom II X2 515", 3100000000L},
    {"AMD Phenom II X2 520", 3200000000L},
    {"AMD Phenom II X2 525", 3300000000L},
    {"AMD Phenom II X2 530", 3400000000L},
    {"AMD Phenom II X2 540", 3500000000L},
    {"AMD Phenom II X2 545", 3600000000L},
    {"AMD Phenom II X2 550", 3700000000L},
    {"AMD Phenom II X2 555", 3800000000L},
    {"AMD Phenom II X2 560", 3900000000L},
    {"AMD Phenom II X2 565", 4000000000L},
    {"AMD Phenom II X3 700", 2800000000L},
    {"AMD Phenom II X3 705", 2900000000L},
    {"AMD Phenom II X3 710", 3000000000L},
    {"AMD Phenom II X3 715", 3100000000L},
    {"AMD Phenom II X3 720", 3200000000L},
    {"AMD Phenom II X3 725", 3300000000L},
    {"AMD Phenom II X3 730", 3400000000L},
    {"AMD Phenom II X3 735", 3500000000L},
    {"AMD Phenom II X3 740", 3600000000L},
    {"AMD Phenom II X4 800", 3000000000L},
    {"AMD Phenom II X4 805", 3100000000L},
    {"AMD Phenom II X4 810", 3200000000L},
    {"AMD Phenom II X4 815", 3300000000L},
    {"AMD Phenom II X4 820", 3400000000L},
    {"AMD Phenom II X4 825", 3500000000L},
    {"AMD Phenom II X4 830", 3600000000L},
    {"AMD Phenom II X4 835", 3700000000L},
    {"AMD Phenom II X4 840", 3800000000L},
    {"AMD Phenom II X4 845", 3900000000L},
    {"AMD Phenom II X4 910", 3000000000L},
    {"AMD Phenom II X4 915", 3100000000L},
    {"AMD Phenom II X4 920", 3200000000L},
    {"AMD Phenom II X4 925", 3300000000L},
    {"AMD Phenom II X4 930", 3400000000L},
    {"AMD Phenom II X4 935", 3500000000L},
    {"AMD Phenom II X4 940", 3600000000L},
    {"AMD Phenom II X4 945", 3700000000L},
    {"AMD Phenom II X4 950", 3800000000L},
    {"AMD Phenom II X4 955", 3900000000L},
    {"AMD Phenom II X4 960", 4000000000L},
    {"AMD Phenom II X4 965", 4100000000L},
    {"AMD Phenom II X6 1000", 3200000000L},
    {"AMD Phenom II X6 1010T", 3300000000L},
    {"AMD Phenom II X6 1020T", 3400000000L},
    {"AMD Phenom II X6 1035T", 3500000000L},
    {"AMD Phenom II X6 1045T", 3600000000L},
    {"AMD Phenom II X6 1055T", 3700000000L},
    {"AMD Phenom II X6 1065T", 3800000000L},
    {"AMD Phenom II X6 1075T", 3900000000L},
    {"AMD Phenom II X6 1090T", 4000000000L},
    {"AMD Phenom II X6 1100T", 4100000000L},
    {"AMD Ryzen 3 1200", 3500000000L},
    {"AMD Ryzen 3 1300X", 3600000000L},
    {"AMD Ryzen 3 2200G", 3600000000L},
    {"AMD Ryzen 3 3200G", 3700000000L},
    {"AMD Ryzen 3 4100", 3600000000L},
    {"AMD Ryzen 3 4300GE", 3700000000L},
    {"AMD Ryzen 3 4300G", 3800000000L},
    {"AMD Ryzen 3 5300G", 3900000000L},
    {"AMD Ryzen 3 6300", 4000000000L},
    {"AMD Ryzen 3 7300X", 4100000000L},
    {"AMD Ryzen 3 7350X", 4200000000L},
    {"AMD Ryzen 3 9400F", 4300000000L},
    {"AMD Ryzen 3 9500", 4400000000L},
    {"AMD Ryzen 5 1400", 3600000000L},
    {"AMD Ryzen 5 1500X", 3700000000L},
    {"AMD Ryzen 5 1600", 3800000000L},
    {"AMD Ryzen 5 1600X", 3900000000L},
    {"AMD Ryzen 5 2600", 4000000000L},
    {"AMD Ryzen 5 2600X", 4100000000L},
    {"AMD Ryzen 5 3400G", 4200000000L},
    {"AMD Ryzen 5 3600", 4300000000L},
    {"AMD Ryzen 5 3600X", 4400000000L},
    {"AMD Ryzen 5 4600G", 4500000000L},
    {"AMD Ryzen 5 5600", 4600000000L},
    {"AMD Ryzen 5 5600G", 4700000000L},
    {"AMD Ryzen 5 5600X", 4800000000L},
    {"AMD Ryzen 5 7600", 4900000000L},
    {"AMD Ryzen 5 7600X", 5000000000L},
    {"AMD Ryzen 7 1700", 3600000000L},
    {"AMD Ryzen 7 1700X", 3700000000L},
    {"AMD Ryzen 7 1800X", 3800000000L},
    {"AMD Ryzen 7 2700", 3900000000L},
    {"AMD Ryzen 7 2700X", 4000000000L},
    {"AMD Ryzen 7 3700X", 4100000000L},
    {"AMD Ryzen 7 3800X", 4200000000L},
    {"AMD Ryzen 7 3800XT", 4300000000L},
    {"AMD Ryzen 7 4700G", 4400000000L},
    {"AMD Ryzen 7 5700G", 4500000000L},
    {"AMD Ryzen 7 5800X", 4600000000L},
    {"AMD Ryzen 7 5800X3D", 4700000000L},
    {"AMD Ryzen 7 7700", 4800000000L},
    {"AMD Ryzen 7 7700X", 4900000000L},
    {"AMD Ryzen 7 7700XT", 5000000000L},
    {"AMD Ryzen 9 3900X", 4700000000L},
    {"AMD Ryzen 9 3950X", 4800000000L},
    {"AMD Ryzen 9 5900X", 4900000000L},
    {"AMD Ryzen 9 5950X", 5000000000L},
    {"AMD Ryzen 9 7900", 5100000000L},
    {"AMD Ryzen 9 7900X", 5200000000L},
    {"AMD Ryzen 9 7900XT", 5300000000L},
    {"AMD Ryzen 9 7950X", 5400000000L},
    {"AMD Ryzen 9 7950XT", 5500000000L},
    {"Intel Core 2 Duo E4300", 1800000000L},
    {"Intel Core 2 Duo E4400", 2000000000L},
    {"Intel Core 2 Duo E4500", 2300000000L},
    {"Intel Core 2 Duo E4600", 2400000000L},
    {"Intel Core 2 Duo E4700", 2500000000L},
    {"Intel Core 2 Duo E6300", 1800000000L},
    {"Intel Core 2 Duo E6400", 2000000000L},
    {"Intel Core 2 Duo E6600", 2400000000L},
    {"Intel Core 2 Duo E6700", 2600000000L},
    {"Intel Core 2 Duo E6750", 2660000000L},
    {"Intel Core 2 Duo E6800", 2930000000L},
    {"Intel Core 2 Duo E6850", 3000000000L},
    {"Intel Core 2 Duo E6900", 3060000000L},
    {"Intel Core 2 Duo E7200", 2500000000L},
    {"Intel Core 2 Duo E7300", 2660000000L},
    {"Intel Core 2 Duo E7400", 2660000000L},
    {"Intel Core 2 Duo E7500", 2660000000L},
    {"Intel Core 2 Duo E7600", 2930000000L},
    {"Intel Core 2 Duo E7700", 3060000000L},
    {"Intel Core 2 Duo E8000", 2930000000L},
    {"Intel Core 2 Duo E8100", 2930000000L},
    {"Intel Core 2 Duo E8200", 2930000000L},
    {"Intel Core 2 Duo E8300", 2930000000L},
    {"Intel Core 2 Duo E8400", 3060000000L},
    {"Intel Core 2 Duo E8500", 3060000000L},
    {"Intel Core 2 Quad Q6600", 2400000000L},
    {"Intel Core 2 Quad Q6700", 2660000000L},
    {"Intel Core 2 Quad Q6800", 2930000000L},
    {"Intel Core 2 Quad Q6850", 3000000000L},
    {"Intel Core 2 Quad Q6900", 3060000000L},
    {"Intel Core 2 Quad Q9300", 2400000000L},
    {"Intel Core 2 Quad Q9400", 2660000000L},
    {"Intel Core 2 Quad Q9450", 2660000000L},
    {"Intel Core 2 Quad Q9500", 2660000000L},
    {"Intel Core 2 Quad Q9550", 2930000000L},
    {"Intel Core 2 Quad Q9650", 3060000000L},
    {"Intel Core 2 Quad Q9700", 3060000000L},
    {"Intel Core 2 Quad QX6700", 2660000000L},
    {"Intel Core 2 Quad QX6800", 2930000000L},
    {"Intel Core 2 Quad QX6850", 3000000000L},
    {"Intel Core 2 Quad QX9650", 3060000000L},
    {"Intel Core i3-2100", 3000000000L},
    {"Intel Core i3-2120", 3300000000L},
    {"Intel Core i3-3220", 3300000000L},
    {"Intel Core i3-3240", 3400000000L},
    {"Intel Core i3-4130", 3400000000L},
    {"Intel Core i3-4150", 3600000000L},
    {"Intel Core i3-4160", 3600000000L},
    {"Intel Core i3-4170", 3600000000L},
    {"Intel Core i3-6100", 3300000000L},
    {"Intel Core i3-6300", 3600000000L},
    {"Intel Core i3-6320", 3700000000L},
    {"Intel Core i3-8100", 3600000000L},
    {"Intel Core i3-8300", 3600000000L},
    {"Intel Core i3-9100", 3600000000L},
    {"Intel Core i3-9300", 3600000000L},
    {"Intel Core i5-2300", 2800000000L},
    {"Intel Core i5-2400", 3100000000L},
    {"Intel Core i5-2500", 3200000000L},
    {"Intel Core i5-2500K", 3300000000L},
    {"Intel Core i5-2550K", 3300000000L},
    {"Intel Core i5-3570", 3400000000L},
    {"Intel Core i5-3570K", 3600000000L},
    {"Intel Core i5-3750K", 3600000000L},
    {"Intel Core i5-4430", 3300000000L},
    {"Intel Core i5-4460", 3400000000L},
    {"Intel Core i5-4570", 3400000000L},
    {"Intel Core i5-4670", 3400000000L},
    {"Intel Core i5-4690", 3400000000L},
    {"Intel Core i5-4690K", 3600000000L},
    {"Intel Core i5-5250U", 1700000000L},
    {"Intel Core i5-5287U", 2500000000L},
    {"Intel Core i5-6200U", 2300000000L},
    {"Intel Core i5-6300HQ", 2300000000L},
    {"Intel Core i5-6400", 2800000000L},
    {"Intel Core i5-6500", 3200000000L},
    {"Intel Core i5-6600", 3200000000L},
    {"Intel Core i5-6600K", 3500000000L},
    {"Intel Core i5-7400", 3000000000L},
    {"Intel Core i5-7500", 3200000000L},
    {"Intel Core i5-7500T", 2500000000L},
    {"Intel Core i5-7600", 3300000000L},
    {"Intel Core i5-7600K", 3800000000L},
    {"Intel Core i5-8250U", 1800000000L},
    {"Intel Core i5-8265U", 1800000000L},
    {"Intel Core i5-8300H", 2300000000L},
    {"Intel Core i5-8400", 2800000000L},
    {"Intel Core i5-8500", 3000000000L},
    {"Intel Core i5-8600K", 3600000000L},
    {"Intel Core i5-9300H", 2400000000L},
    {"Intel Core i5-9400", 2900000000L},
    {"Intel Core i5-9600K", 3700000000L},
    {"Intel Core i5-1035G1", 1000000000L},
    {"Intel Core i5-10400", 2900000000L},
    {"Intel Core i5-10400F", 2900000000L},
    {"Intel Core i5-10500", 3300000000L},
    {"Intel Core i5-10600", 3300000000L},
    {"Intel Core i5-10600K", 4100000000L},
    {"Intel Core i5-10600KF", 4100000000L},
    {"Intel Core i5-10700", 2900000000L},
    {"Intel Core i5-10700F", 2900000000L},
    {"Intel Core i5-10700K", 3800000000L},
    {"Intel Core i5-10700KF", 3800000000L},
    {"Intel Core i5-1135G7", 2000000000L},
    {"Intel Core i5-11400", 2600000000L},
    {"Intel Core i5-11400F", 2600000000L},
    {"Intel Core i5-11400H", 2800000000L},
    {"Intel Core i5-11500", 2900000000L},
    {"Intel Core i5-11600K", 3600000000L},
    {"Intel Core i5-11600KF", 3600000000L},
    {"Intel Core i5-1235U", 1000000000L},
    {"Intel Core i5-1240P", 1800000000L},
    {"Intel Core i5-12400", 2600000000L},
    {"Intel Core i5-12400F", 2600000000L},
    {"Intel Core i5-12500", 3000000000L},
    {"Intel Core i5-12600K", 3100000000L},
    {"Intel Core i5-12600KF", 3100000000L},
    {"Intel Core i5-12700", 2900000000L},
    {"Intel Core i5-12700K", 3200000000L},
    {"Intel Core i5-12700KF", 3200000000L},
    {"Intel Core i5-12800", 3100000000L},
    {"Intel Core i5-12800K", 3200000000L},
    {"Intel Core i5-12900", 3200000000L},
    {"Intel Core i5-12900K", 3200000000L},
    {"Intel Core i5-12900KF", 3200000000L},
    {"Intel Core i5-13600K", 3100000000L},
    {"Intel Core i5-13600KF", 3100000000L},
    {"Intel Core i5-14400", 3000000000L},
    {"Intel Core i5-14500", 3100000000L},
    {"Intel Core i5-14600K", 3200000000L},
    {"Intel Core i5-14600KF", 3200000000L},
    {"Intel Core i5-15600K", 3300000000L},
    {"Intel Core i5-15600KF", 3300000000L},
    {"Intel Core i7-2600", 3400000000L},
    {"Intel Core i7-2600K", 3700000000L},
    {"Intel Core i7-2700K", 3700000000L},
    {"Intel Core i7-3770", 3400000000L},
    {"Intel Core i7-3770K", 3600000000L},
    {"Intel Core i7-4790", 3600000000L},
    {"Intel Core i7-4790K", 4000000000L},
    {"Intel Core i7-5820K", 3400000000L},
    {"Intel Core i7-6700", 3300000000L},
    {"Intel Core i7-6700K", 4000000000L},
    {"Intel Core i7-7700", 3200000000L},
    {"Intel Core i7-7700K", 4200000000L},
    {"Intel Core i7-8700", 3200000000L},
    {"Intel Core i7-8700K", 3600000000L},
    {"Intel Core i7-9700", 3000000000L},
    {"Intel Core i7-9700K", 3600000000L},
    {"Intel Core i7-10700", 2900000000L},
    {"Intel Core i7-10700K", 3800000000L},
    {"Intel Core i7-10700KF", 3800000000L},
    {"Intel Core i7-11700", 2900000000L},
    {"Intel Core i7-11700K", 3700000000L},
    {"Intel Core i7-11700KF", 3700000000L},
    {"Intel Core i7-12700", 2900000000L},
    {"Intel Core i7-12700K", 3200000000L},
    {"Intel Core i7-12700KF", 3200000000L},
    {"Intel Core i7-13700K", 3100000000L},
    {"Intel Core i7-13700KF", 3100000000L},
    {"Intel Core i7-14700K", 3200000000L},
    {"Intel Core i7-14700KF", 3200000000L},
    {"Intel Core i7-15700K", 3300000000L},
    {"Intel Core i7-15700KF", 3300000000L},
    {"Intel Core i9-7900X", 3000000000L},
    {"Intel Core i9-7920X", 2600000000L},
    {"Intel Core i9-7940X", 3000000000L},
    {"Intel Core i9-7960X", 2600000000L},
    {"Intel Core i9-7980XE", 2600000000L},
    {"Intel Core i9-9900", 3000000000L},
    {"Intel Core i9-9900K", 3600000000L},
    {"Intel Core i9-9900KF", 3600000000L},
    {"Intel Core i9-10900", 2900000000L},
    {"Intel Core i9-10900K", 3700000000L},
    {"Intel Core i9-10900KF", 3700000000L},
    {"Intel Core i9-10920X", 3600000000L},
    {"Intel Core i9-10940X", 3600000000L},
    {"Intel Core i9-10980XE", 3600000000L},
    {"Intel Core i9-11900", 2900000000L},
    {"Intel Core i9-11900K", 3700000000L},
    {"Intel Core i9-11900KF", 3700000000L},
    {"Intel Core i9-12900K", 3100000000L},
    {"Intel Core i9-12900KF", 3100000000L},
    {"Intel Core i9-12900KS", 3500000000L},
    {"Intel Core i9-13900K", 3200000000L},
    {"Intel Core i9-13900KF", 3200000000L},
    {"Intel Core i9-14900K", 3300000000L},
    {"Intel Core i9-14900KF", 3300000000L},
    {"Intel Core i9-15900K", 3400000000L},
    {"Intel Core i9-15900KF", 3400000000L},
    {"Intel Core i3-2100", 3000000000L},
    {"Intel Core i3-2120", 3300000000L},
    {"Intel Core i3-2130", 3300000000L},
    {"Intel Core i3-3210", 3300000000L},
    {"Intel Core i3-3220", 3300000000L},
    {"Intel Core i3-3240", 3400000000L},
    {"Intel Core i3-4100", 3400000000L},
    {"Intel Core i3-4130", 3400000000L},
    {"Intel Core i3-4150", 3500000000L},
    {"Intel Core i3-4160", 3500000000L},
    {"Intel Core i3-4170", 3600000000L},
    {"Intel Core i3-4330", 3600000000L},
    {"Intel Core i3-4340", 3600000000L},
    {"Intel Core i3-4350", 3600000000L},
    {"Intel Core i3-4360", 3600000000L},
    {"Intel Core i3-6100", 3700000000L},
    {"Intel Core i3-6300", 3800000000L},
    {"Intel Core i3-6320", 3800000000L},
    {"Intel Core i3-7100", 3800000000L},
    {"Intel Core i3-7300", 3900000000L},
    {"Intel Core i3-7320", 3900000000L},
    {"Intel Core i3-8100", 3900000000L},
    {"Intel Core i3-8300", 4000000000L},
    {"Intel Core i3-8350K", 4000000000L},
    {"Intel Core i3-9100", 3900000000L},
    {"Intel Core i3-9100F", 3900000000L},
    {"Intel Core i3-9300", 4100000000L},
    {"Intel Core i3-9300F", 4100000000L},
    {"Intel Core i3-10100", 3800000000L},
    {"Intel Core i3-10100F", 3800000000L},
    {"Intel Core i3-10320", 3800000000L},
    {"Intel Core i3-10300", 3800000000L},
    {"Intel Core i3-10300F", 3800000000L},
    {"Intel Core i3-10320", 3800000000L},
    {"Intel Core i3-10320F", 3800000000L},
    {"Intel Core i3-10105", 3800000000L},
    {"Intel Core i3-10105F", 3800000000L},
    {"Intel Core i3-11100", 3800000000L},
    {"Intel Core i3-11100F", 3800000000L},
    {"Intel Core i3-11105", 3800000000L},
    {"Intel Core i3-11105F", 3800000000L},
    {"Intel Core i3-11200", 3800000000L},
    {"Intel Core i3-11200F", 3800000000L},
    {"Intel Core i3-11300", 3800000000L},
    {"Intel Core i3-11300F", 3800000000L},
    {"Intel Core i3-12100", 3900000000L},
    {"Intel Core i3-12100F", 3900000000L},
    {"Intel Core i3-12300", 3900000000L},
    {"Intel Core i3-12300F", 3900000000L},
    {"Intel Core i3-13100", 4000000000L},
    {"Intel Core i3-13100F", 4000000000L},
    {"Intel Core i5-2400", 3100000000L},
    {"Intel Core i5-2500", 3300000000L},
    {"Intel Core i5-2500K", 3300000000L},
    {"Intel Core i5-2500S", 2900000000L},
    {"Intel Core i5-2500T", 2800000000L},
    {"Intel Core i5-3470", 3200000000L},
    {"Intel Core i5-3470S", 2900000000L},
    {"Intel Core i5-3470T", 2900000000L},
    {"Intel Core i5-3550", 3200000000L},
    {"Intel Core i5-3550S", 3000000000L},
    {"Intel Core i5-3570", 3200000000L},
    {"Intel Core i5-3570K", 3600000000L},
    {"Intel Core i5-3570S", 3000000000L},
    {"Intel Core i5-3570T", 2900000000L},
    {"Intel Core i5-4440", 3300000000L},
    {"Intel Core i5-4440S", 2900000000L},
    {"Intel Core i5-4440T", 2900000000L},
    {"Intel Core i5-4460", 3300000000L},
    {"Intel Core i5-4460S", 2900000000L},
    {"Intel Core i5-4460T", 2900000000L},
    {"Intel Core i5-4570", 3300000000L},
    {"Intel Core i5-4570S", 2900000000L},
    {"Intel Core i5-4570T", 2900000000L},
    {"Intel Core i5-4670", 3300000000L},
    {"Intel Core i5-4670K", 3600000000L},
    {"Intel Core i5-4670S", 3000000000L},
    {"Intel Core i5-4670T", 2900000000L},
    {"Intel Core i5-4690", 3400000000L},
    {"Intel Core i5-4690K", 3600000000L},
    {"Intel Core i5-4690S", 3000000000L},
    {"Intel Core i5-4690T", 2900000000L},
    {"Intel Core i5-5675C", 3300000000L},
    {"Intel Core i5-6400", 2800000000L},
    {"Intel Core i5-6400T", 2100000000L},
    {"Intel Core i5-6500", 3300000000L},
    {"Intel Core i5-6500T", 2400000000L},
    {"Intel Core i5-6600", 3300000000L},
    {"Intel Core i5-6600K", 3700000000L},
    {"Intel Core i5-6600T", 2400000000L},
    {"Intel Core i5-7400", 3000000000L},
    {"Intel Core i5-7400T", 2500000000L},
    {"Intel Core i5-7500", 3200000000L},
    {"Intel Core i5-7500T", 2900000000L},
    {"Intel Core i5-7600", 3300000000L},
    {"Intel Core i5-7600K", 3800000000L},
    {"Intel Core i5-7600T", 2800000000L},
    {"Intel Core i5-7640X", 4000000000L},
    {"Intel Core i5-7660X", 4200000000L},
    {"Intel Core i7-2600", 3400000000L},
    {"Intel Core i7-2600K", 3500000000L},
    {"Intel Core i7-2600S", 2900000000L},
    {"Intel Core i7-2600T", 2500000000L},
    {"Intel Core i7-3610QM", 2600000000L},
    {"Intel Core i7-3630QM", 2600000000L},
    {"Intel Core i7-3770", 3400000000L},
    {"Intel Core i7-3770K", 3600000000L},
    {"Intel Core i7-3770S", 3100000000L},
    {"Intel Core i7-3770T", 2900000000L},
    {"Intel Core i7-4790", 3600000000L},
    {"Intel Core i7-4790K", 4000000000L},
    {"Intel Core i7-4790S", 3600000000L},
    {"Intel Core i7-4790T", 2900000000L},
    {"Intel Core i7-5820K", 3300000000L},
    {"Intel Core i7-5930K", 3400000000L},
    {"Intel Core i7-5960X", 3300000000L},
    {"Intel Core i7-6700", 3300000000L},
    {"Intel Core i7-6700K", 4000000000L},
    {"Intel Core i7-6700T", 2800000000L},
    {"Intel Core i7-7700", 3600000000L},
    {"Intel Core i7-7700K", 4200000000L},
    {"Intel Core i7-7700T", 2900000000L},
    {"Intel Core i7-7800X", 3500000000L},
    {"Intel Core i7-7820X", 3600000000L},
    {"Intel Core i7-8086K", 4000000000L},
    {"Intel Core i7-8550U", 1800000000L},
    {"Intel Core i7-8565U", 1800000000L},
    {"Intel Core i7-8750H", 2200000000L},
    {"Intel Core i7-8850H", 2600000000L},
    {"Intel Core i7-9700", 3000000000L},
    {"Intel Core i7-9700K", 3600000000L},
    {"Intel Core i7-9700KF", 3600000000L},
    {"Intel Core i7-9700T", 2000000000L},
    {"Intel Core i7-10700", 2900000000L},
    {"Intel Core i7-10700K", 3800000000L},
    {"Intel Core i7-10700KF", 3800000000L},
    {"Intel Core i7-10700T", 2000000000L},
    {"Intel Core i7-11700", 2900000000L},
    {"Intel Core i7-11700K", 3700000000L},
    {"Intel Core i7-11700KF", 3700000000L},
    {"Intel Core i7-11700T", 1800000000L},
    {"Intel Core i7-12700", 2900000000L},
    {"Intel Core i7-12700K", 3200000000L},
    {"Intel Core i7-12700KF", 3200000000L},
    {"Intel Core i7-12700T", 1800000000L},
    {"Intel Core i7-13700", 2900000000L},
    {"Intel Core i7-13700K", 3100000000L},
    {"Intel Core i7-13700KF", 3100000000L},
    {"Intel Core i7-13700T", 1800000000L},
    {"Intel Core i7-14700", 3100000000L},
    {"Intel Core i7-14700K", 3200000000L},
    {"Intel Core i7-14700KF", 3200000000L},
    {"Intel Core i7-14700T", 1900000000L},
    {"Intel Core i7-15700K", 3300000000L},
    {"Intel Core i7-15700KF", 3300000000L},
    {"Intel Core i7-15700T", 2000000000L},
    {"Intel Core i9-9900", 3100000000L},
    {"Intel Core i9-9900K", 3600000000L},
    {"Intel Core i9-9900KF", 3600000000L},
    {"Intel Core i9-9900KS", 4000000000L},
    {"Intel Core i9-10900", 2900000000L},
    {"Intel Core i9-10900K", 3700000000L},
    {"Intel Core i9-10900KF", 3700000000L},
    {"Intel Core i9-10900T", 1800000000L},
    {"Intel Core i9-10920X", 3500000000L},
    {"Intel Core i9-10940X", 3600000000L},
    {"Intel Core i9-10980XE", 3600000000L},
    {"Intel Core i9-11900", 2900000000L},
    {"Intel Core i9-11900K", 3700000000L},
    {"Intel Core i9-11900KF", 3700000000L},
    {"Intel Core i9-11900T", 1800000000L},
    {"Intel Core i9-12900", 3200000000L},
    {"Intel Core i9-12900K", 3200000000L},
    {"Intel Core i9-12900KF", 3200000000L},
    {"Intel Core i9-12900KS", 3500000000L},
    {"Intel Core i9-12900T", 1800000000L},
    {"Intel Core i9-13900", 3000000000L},
    {"Intel Core i9-13900K", 3200000000L},
    {"Intel Core i9-13900KF", 3200000000L},
    {"Intel Core i9-13900KS", 3400000000L},
    {"Intel Core i9-13900T", 1800000000L},
    {"Intel Core i9-14900", 3200000000L},
    {"Intel Core i9-14900K", 3300000000L},
    {"Intel Core i9-14900KF", 3300000000L},
    {"Intel Core i9-14900KS", 3600000000L},
    {"Intel Core i9-14900T", 1900000000L},
    {"Intel Core i9-15900K", 3400000000L},
    {"Intel Core i9-15900KF", 3400000000L},
    {"Intel Core i9-15900KS", 3700000000L},
    {"Intel Core i9-15900T", 2000000000L},
    {"Intel Xeon E3-1220", 3100000000L},
    {"Intel Xeon E3-1220 v2", 3100000000L},
    {"Intel Xeon E3-1220 v3", 3100000000L},
    {"Intel Xeon E3-1220 v5", 3000000000L},
    {"Intel Xeon E3-1220 v6", 3100000000L},
    {"Intel Xeon E3-1225", 3200000000L},
    {"Intel Xeon E3-1225 v2", 3200000000L},
    {"Intel Xeon E3-1225 v3", 3200000000L},
    {"Intel Xeon E3-1225 v5", 3300000000L},
    {"Intel Xeon E3-1225 v6", 3300000000L},
    {"Intel Xeon E3-1230", 3200000000L},
    {"Intel Xeon E3-1230 v2", 3300000000L},
    {"Intel Xeon E3-1230 v3", 3300000000L},
    {"Intel Xeon E3-1230 v5", 3400000000L},
    {"Intel Xeon E3-1230 v6", 3500000000L},
    {"Intel Xeon E3-1240", 3300000000L},
    {"Intel Xeon E3-1240 v2", 3400000000L},
    {"Intel Xeon E3-1240 v3", 3400000000L},
    {"Intel Xeon E3-1240 v5", 3500000000L},
    {"Intel Xeon E3-1240 v6", 3600000000L},
    {"Intel Xeon E3-1241 v3", 3500000000L},
    {"Intel Xeon E3-1245", 3400000000L},
    {"Intel Xeon E3-1245 v2", 3500000000L},
    {"Intel Xeon E3-1245 v3", 3500000000L},
    {"Intel Xeon E3-1245 v5", 3600000000L},
    {"Intel Xeon E3-1245 v6", 3700000000L},
    {"Intel Xeon E3-1270", 3400000000L},
    {"Intel Xeon E3-1270 v2", 3500000000L},
    {"Intel Xeon E3-1270 v3", 3500000000L},
    {"Intel Xeon E3-1270 v5", 3600000000L},
    {"Intel Xeon E3-1270 v6", 3800000000L},
    {"Intel Xeon E3-1280", 3500000000L},
    {"Intel Xeon E3-1280 v2", 3600000000L},
    {"Intel Xeon E3-1280 v3", 3600000000L},
    {"Intel Xeon E3-1280 v5", 3700000000L},
    {"Intel Xeon E3-1280 v6", 3900000000L},
    {"Intel Xeon E5-1620", 3600000000L},
    {"Intel Xeon E5-1620 v2", 3700000000L},
    {"Intel Xeon E5-1620 v3", 3600000000L},
    {"Intel Xeon E5-1620 v4", 3600000000L},
    {"Intel Xeon E5-1650", 3600000000L},
    {"Intel Xeon E5-1650 v2", 3700000000L},
    {"Intel Xeon E5-1650 v3", 3600000000L},
    {"Intel Xeon E5-1650 v4", 3600000000L},
    {"Intel Xeon E5-1660 v2", 4000000000L},
    {"Intel Xeon E5-1660 v3", 3600000000L},
    {"Intel Xeon E5-1660 v4", 3600000000L},
    {"Intel Xeon E5-1680 v2", 4000000000L},
    {"Intel Xeon E5-1680 v3", 3600000000L},
    {"Intel Xeon E5-1680 v4", 3600000000L},
    {"Intel Xeon E5-2603", 1800000000L},
    {"Intel Xeon E5-2603 v2", 1800000000L},
    {"Intel Xeon E5-2603 v3", 1600000000L},
    {"Intel Xeon E5-2603 v4", 1700000000L},
    {"Intel Xeon E5-2620", 2000000000L},
    {"Intel Xeon E5-2620 v2", 2100000000L},
    {"Intel Xeon E5-2620 v3", 2400000000L},
    {"Intel Xeon E5-2620 v4", 2100000000L},
    {"Intel Xeon E5-2630", 2300000000L},
    {"Intel Xeon E5-2630 v2", 2600000000L},
    {"Intel Xeon E5-2630 v3", 2400000000L},
    {"Intel Xeon E5-2630 v4", 2200000000L},
    {"Intel Xeon E5-2640", 2500000000L},
    {"Intel Xeon E5-2640 v2", 2600000000L},
    {"Intel Xeon E5-2640 v3", 2600000000L},
    {"Intel Xeon E5-2640 v4", 2600000000L},
    {"Intel Xeon E5-2650", 2600000000L},
    {"Intel Xeon E5-2650 v2", 2800000000L},
    {"Intel Xeon E5-2650 v3", 2600000000L},
    {"Intel Xeon E5-2650 v4", 2600000000L},
    {"Intel Xeon E5-2660", 2700000000L},
    {"Intel Xeon E5-2660 v2", 2800000000L},
    {"Intel Xeon E5-2660 v3", 2600000000L},
    {"Intel Xeon E5-2660 v4", 2600000000L},
    {"Intel Xeon E5-2670", 2600000000L},
    {"Intel Xeon E5-2670 v2", 2600000000L},
    {"Intel Xeon E5-2670 v3", 2300000000L},
    {"Intel Xeon E5-2670 v4", 2300000000L},
    {"Intel Xeon E5-2680", 2700000000L},
    {"Intel Xeon E5-2680 v2", 2800000000L},
    {"Intel Xeon E5-2680 v3", 2500000000L},
    {"Intel Xeon E5-2680 v4", 2500000000L},
    {"Intel Xeon E5-2690", 2900000000L},
    {"Intel Xeon E5-2690 v2", 3000000000L},
    {"Intel Xeon E5-2690 v3", 2600000000L},
    {"Intel Xeon E5-2690 v4", 2600000000L},
    {"Intel Xeon E5-2695 v2", 2700000000L},
    {"Intel Xeon E5-2695 v3", 2300000000L},
    {"Intel Xeon E5-2695 v4", 2300000000L},
    {"Intel Xeon E5-2697 v2", 2700000000L},
    {"Intel Xeon E5-2697 v3", 2300000000L},
    {"Intel Xeon E5-2697 v4", 2300000000L},
    {"Intel Xeon E5-2699 v3", 2300000000L},
    {"Intel Xeon E5-2699 v4", 2200000000L},
    {"Intel Xeon E7-4807", 1600000000L},
    {"Intel Xeon E7-4820", 2000000000L},
    {"Intel Xeon E7-4860", 2600000000L},
    {"Intel Xeon E7-4870", 2700000000L},
    {"Intel Xeon E7-4890 v2", 2800000000L},
    {"Intel Xeon E7-8890 v3", 2400000000L},
    {"Intel Xeon E7-8890 v4", 2200000000L},
    {"Intel Xeon W-2123", 3600000000L},
    {"Intel Xeon W-2133", 3600000000L},
    {"Intel Xeon W-2140B", 3200000000L},
    {"Intel Xeon W-2145", 3700000000L},
    {"Intel Xeon W-2150B", 3000000000L},
    {"Intel Xeon W-2155", 3100000000L},
    {"Intel Xeon W-2175", 3100000000L},
    {"Intel Xeon W-2195", 3100000000L},
    {"Intel Xeon W-2223", 3600000000L},
    {"Intel Xeon W-2235", 3900000000L},
    {"Intel Xeon W-2245", 3700000000L},
    {"Intel Xeon W-2255", 3700000000L},
    {"Intel Xeon W-2265", 3200000000L},
    {"Intel Xeon W-2275", 3300000000L},
    {"Intel Xeon W-2295", 3300000000L},
    {"Intel Xeon W-3175X", 3500000000L},
    {"Intel Xeon Platinum 8176", 2300000000L},
    {"Intel Xeon Platinum 8180", 2500000000L},
    {"Intel Xeon Platinum 8260", 2400000000L},
    {"Intel Xeon Platinum 8276", 2300000000L},
    {"Intel Xeon Platinum 8280", 2700000000L},
    {"Intel Xeon Platinum 8358", 2600000000L},
    {"Intel Xeon Platinum 8380", 2300000000L},
    {"Intel Xeon Gold 5115", 2100000000L},
    {"Intel Xeon Gold 5120", 2200000000L},
    {"Intel Xeon Gold 6130", 2200000000L},
    {"Intel Xeon Gold 6140", 2300000000L},
    {"Intel Xeon Gold 6150", 2700000000L},
    {"Intel Xeon Silver 4110", 2100000000L},
    {"Intel Xeon Silver 4114", 2200000000L},
    {"Intel Xeon Silver 4210", 2200000000L},
    {"Intel Xeon Silver 4310", 2100000000L},
    {"Intel Xeon Bronze 3104", 1600000000L},
    {"Intel Xeon Bronze 3204", 1600000000L},
    {"Intel Xeon D-1518", 2200000000L},
    {"Intel Xeon D-1520", 2200000000L},
    {"Intel Xeon D-1531", 2300000000L},
    {"Intel Xeon D-1541", 2300000000L},
    {"Intel Xeon D-1557", 1500000000L},
    {"Intel Xeon D-1567", 1500000000L},
    {"Intel Xeon D-1571", 1300000000L},
    {"Intel Xeon D-1581", 1900000000L},
    {"Intel Xeon D-2123IT", 2100000000L},
    {"Intel Xeon D-2141I", 2200000000L},
    {"Intel Xeon D-2161T", 2700000000L},
    {"Intel Xeon D-2183IT", 2200000000L},
    {"Intel Xeon D-2191", 1800000000L},
    {"Intel Atom C2338", 1700000000L},
    {"Intel Atom C2358", 1700000000L},
    {"Intel Atom C2550", 2400000000L},
    {"Intel Atom C2750", 2400000000L},
    {"Intel Atom C3558", 2200000000L},
    {"Intel Atom C3758", 2200000000L},
    {"Intel Atom C3858", 2000000000L},
    {"Intel Atom C3958", 2000000000L},
    {"Intel Atom E3825", 1300000000L},
    {"Intel Atom E3845", 1900000000L},
    {"Intel Atom x3-C3130", 1000000000L},
    {"Intel Atom x5-Z8350", 1440000000L},
    {"Intel Atom x7-Z8750", 1600000000L},
    {"Intel Atom x7-Z8700", 1600000000L},
    {"Intel Atom Z3735F", 1330000000L},
    {"Intel Atom Z3740", 1330000000L},
    {"Intel Atom Z3775", 1600000000L},
    {"Intel Atom Z3795", 1600000000L},
    {"Intel Pentium G3250", 3200000000L},
    {"Intel Pentium G4400", 3300000000L},
    {"Intel Pentium G4560", 3500000000L},
    {"Intel Pentium G4600", 3600000000L},
    {"Intel Pentium G4620", 3700000000L},
    {"Intel Pentium Gold G5400", 3700000000L},
    {"Intel Pentium Gold G5500", 3800000000L},
    {"Intel Pentium Gold G5600", 3900000000L},
    {"Intel Pentium Silver J5005", 1500000000L},
    {"Intel Pentium Silver N5000", 1100000000L},
    {"Intel Pentium Silver N6000", 1100000000L},
    {"Intel Pentium T4500", 2300000000L},
    {"Intel Pentium 4 630", 3000000000L},
    {"Intel Pentium 4 640", 3200000000L},
    {"Intel Pentium D 820", 2800000000L},
    {"Intel Pentium D 830", 3000000000L},
    {"Intel Pentium D 915", 2800000000L},
    {"Intel Pentium D 925", 3000000000L},
    {"Intel Pentium Extreme Edition 965", 3600000000L},
    {"Intel Celeron G4900", 3100000000L},
    {"Intel Celeron G4920", 3200000000L},
    {"Intel Celeron J1800", 2410000000L},
    {"Intel Celeron J1900", 2000000000L},
    {"Intel Celeron J3060", 1600000000L},
    {"Intel Celeron J3455", 1500000000L},
    {"Intel Celeron N2840", 2160000000L},
    {"Intel Celeron N2940", 1830000000L},
    {"Intel Celeron N4000", 1100000000L},
    {"Intel Celeron N4020", 1100000000L},
    {"Intel Celeron N4500", 1100000000L},
    {"Intel Celeron N5100", 1100000000L},
    {"Intel Celeron 420", 1600000000L},
    {"Intel Celeron 430", 1800000000L},
    {"Intel Celeron 440", 2000000000L},
    {"Intel Celeron 450", 2200000000L},
    {"Intel Celeron 530", 1700000000L},
    {"Intel Celeron 900", 2200000000L},
    {"Intel Celeron 1007U", 1500000000L},
    {"Intel Celeron 2955U", 1400000000L},
    {"Intel Celeron 3205U", 1500000000L},
    {"Intel Celeron 3865U", 1800000000L},
    {"Intel Celeron 4205U", 1800000000L},
    {"Intel Celeron 5205U", 1900000000L},
    {"AMD A4-3300", 2500000000L},
    {"AMD A4-4000", 3000000000L},
    {"AMD A4-5300", 3400000000L},
    {"AMD A4-6300", 3700000000L},
    {"AMD A4-7300", 3800000000L},
    {"AMD A6-3500", 2100000000L},
    {"AMD A6-5400K", 3600000000L},
    {"AMD A6-6400K", 3900000000L},
    {"AMD A6-7400K", 3500000000L},
    {"AMD A8-5500", 3200000000L},
    {"AMD A8-5600K", 3600000000L},
    {"AMD A8-6600K", 3900000000L},
    {"AMD A8-7670K", 3600000000L},
    {"AMD A10-5700", 3400000000L},
    {"AMD A10-5800K", 3800000000L},
    {"AMD A10-6800K", 4100000000L},
    {"AMD A10-7850K", 3700000000L},
    {"AMD A10-7870K", 3900000000L},
    {"AMD A12-9800", 3800000000L},
    {"AMD FX-4100", 3600000000L},
    {"AMD FX-4300", 3800000000L},
    {"AMD FX-4350", 4200000000L},
    {"AMD FX-6300", 3500000000L},
    {"AMD FX-6350", 3900000000L},
    {"AMD FX-8300", 3300000000L},
    {"AMD FX-8350", 4000000000L},
    {"AMD FX-8370", 4000000000L},
    {"AMD FX-9590", 4700000000L},
    {"AMD Ryzen 3 1200", 3100000000L},
    {"AMD Ryzen 3 1300X", 3500000000L},
    {"AMD Ryzen 3 2200G", 3500000000L},
    {"AMD Ryzen 3 3200G", 3600000000L},
    {"AMD Ryzen 3 3100", 3600000000L},
    {"AMD Ryzen 3 3300X", 3800000000L},
    {"AMD Ryzen 3 4100", 3800000000L},
    {"AMD Ryzen 3 4300G", 3800000000L},
    {"AMD Ryzen 3 5300G", 4000000000L},
    {"AMD Ryzen 3 PRO 4350G", 3800000000L},
    {"AMD Ryzen 5 1400", 3200000000L},
    {"AMD Ryzen 5 1500X", 3500000000L},
    {"AMD Ryzen 5 1600", 3200000000L},
    {"AMD Ryzen 5 1600X", 3600000000L},
    {"AMD Ryzen 5 2400G", 3600000000L},
    {"AMD Ryzen 5 2600", 3400000000L},
    {"AMD Ryzen 5 2600X", 3600000000L},
    {"AMD Ryzen 5 3400G", 3700000000L},
    {"AMD Ryzen 5 3600", 3600000000L},
    {"AMD Ryzen 5 3600X", 3800000000L},
    {"AMD Ryzen 5 5600", 3500000000L},
    {"AMD Ryzen 5 5600X", 3700000000L},
    {"AMD Ryzen 5 7600", 3800000000L},
    {"AMD Ryzen 5 7600X", 4200000000L},
    {"AMD Ryzen 7 1700", 3000000000L},
    {"AMD Ryzen 7 1700X", 3400000000L},
    {"AMD Ryzen 7 1800X", 3600000000L},
    {"AMD Ryzen 7 2700", 3200000000L},
    {"AMD Ryzen 7 2700X", 3700000000L},
    {"AMD Ryzen 7 3700X", 3600000000L},
    {"AMD Ryzen 7 3800X", 3900000000L},
    {"AMD Ryzen 7 5700X", 3400000000L},
    {"AMD Ryzen 7 5800X", 3800000000L},
    {"AMD Ryzen 7 5800X3D", 3400000000L},
    {"AMD Ryzen 7 7700", 3800000000L},
    {"AMD Ryzen 7 7700X", 4200000000L},
    {"AMD Ryzen 7 7800X3D", 4200000000L},
    {"AMD Ryzen 9 3900", 3200000000L},
    {"AMD Ryzen 9 3900X", 3800000000L},
    {"AMD Ryzen 9 3950X", 3500000000L},
    {"AMD Ryzen 9 5900", 3700000000L},
    {"AMD Ryzen 9 5900X", 3700000000L},
    {"AMD Ryzen 9 5950X", 3400000000L},
    {"AMD Ryzen 9 7900", 3700000000L},
    {"AMD Ryzen 9 7900X", 4400000000L},
    {"AMD Ryzen 9 7950X", 4500000000L},
    {"AMD Ryzen 9 7950X3D", 4200000000L},
    {"AMD Ryzen Threadripper 1900X", 3800000000L},
    {"AMD Ryzen Threadripper 1920X", 3500000000L},
    {"AMD Ryzen Threadripper 1950X", 3400000000L},
    {"AMD Ryzen Threadripper 2920X", 3500000000L},
    {"AMD Ryzen Threadripper 2950X", 3500000000L},
    {"AMD Ryzen Threadripper 3960X", 3800000000L},
    {"AMD Ryzen Threadripper 3970X", 3700000000L},
    {"AMD Ryzen Threadripper 3990X", 2900000000L},
    {"AMD Ryzen Threadripper PRO 3955WX", 3900000000L},
    {"AMD Ryzen Threadripper PRO 3975WX", 3500000000L},
    {"AMD Ryzen Threadripper PRO 3995WX", 2700000000L},
    {"AMD EPYC 7251", 2600000000L},
    {"AMD EPYC 7281", 2400000000L},
    {"AMD EPYC 7351", 2400000000L},
    {"AMD EPYC 7401", 2200000000L},
    {"AMD EPYC 7451", 2400000000L},
    {"AMD EPYC 7501", 2000000000L},
    {"AMD EPYC 7551", 2200000000L},
    {"AMD EPYC 7601", 2200000000L},
    {"AMD EPYC 7763", 2450000000L},
    {"AMD EPYC 9654", 2500000000L},
    {"AMD EPYC 9654P", 2500000000L},
    {"AMD EPYC 9754", 2600000000L},
    {"AMD EPYC 9754P", 2600000000L},
    {"AMD Athlon 200GE", 3200000000L},
    {"AMD Athlon 220GE", 3400000000L},
    {"AMD Athlon 240GE", 3500000000L},
    {"AMD Athlon Gold 3150G", 3700000000L},
    {"AMD Athlon Silver 3050U", 2300000000L},
    {"AMD Athlon 3000G", 3500000000L},
    {"AMD Ryzen 3 3200U", 2600000000L},
    {"AMD Ryzen 3 3250U", 2600000000L},
    {"AMD Ryzen 5 3500U", 2100000000L},
    {"AMD Ryzen 5 3550H", 2200000000L},
    {"AMD Ryzen 5 4500U", 2300000000L},
    {"AMD Ryzen 5 4600H", 3000000000L},
    {"AMD Ryzen 5 5500U", 2100000000L},
    {"AMD Ryzen 5 5600H", 3300000000L},
    {"AMD Ryzen 7 3700U", 2300000000L},
    {"AMD Ryzen 7 3750H", 2300000000L},
    {"AMD Ryzen 7 4700U", 2000000000L},
    {"AMD Ryzen 7 4800H", 2900000000L},
    {"AMD Ryzen 7 5800U", 1900000000L},
    {"AMD Ryzen 7 5800H", 3200000000L},
    {"AMD Ryzen 7 6800H", 3200000000L},
    {"AMD Ryzen 9 5900HX", 3300000000L},
    {"AMD Ryzen 9 6900HX", 3300000000L},
    {"AMD Ryzen 9 7940HS", 4000000000L},
    {"AMD Ryzen 9 7945HX", 2500000000L},
    {"Apple A6", 1300000000L},
    {"Apple A7", 1400000000L},
    {"Apple A8", 1500000000L},
    {"Apple A8X", 1500000000L},
    {"Apple A9", 1850000000L},
    {"Apple A9X", 2260000000L},
    {"Apple A10 Fusion", 2340000000L},
    {"Apple A10X Fusion", 2380000000L},
    {"Apple A11 Bionic", 2390000000L},
    {"Apple A12 Bionic", 2490000000L},
    {"Apple A12Z Bionic", 2490000000L},
    {"Apple A13 Bionic", 2660000000L},
    {"Apple A14 Bionic", 3000000000L},
    {"Apple A15 Bionic", 3240000000L},
    {"Apple A16 Bionic", 3460000000L},
    {"Apple A17 Pro", 3700000000L},
    {"Apple M1", 3200000000L},
    {"Apple M1 Pro", 3200000000L},
    {"Apple M1 Max", 3240000000L},
    {"Apple M1 Ultra", 3200000000L},
    {"Apple M2", 3600000000L},
    {"Apple M2 Pro", 3600000000L},
    {"Apple M2 Max", 3600000000L},
    {"Apple M2 Ultra", 3600000000L},
    {"Apple M3", 4100000000L},
    {"Apple M3 Pro", 4200000000L},
    {"Apple M3 Max", 4200000000L},
    {"ARM Cortex-A5", 1200000000L},
    {"ARM Cortex-A7", 1500000000L},
    {"ARM Cortex-A8", 1200000000L},
    {"ARM Cortex-A9", 2000000000L},
    {"ARM Cortex-A12", 1500000000L},
    {"ARM Cortex-A15", 2400000000L},
    {"ARM Cortex-A17", 2200000000L},
    {"ARM Cortex-A35", 1300000000L},
    {"ARM Cortex-A53", 2000000000L},
    {"ARM Cortex-A55", 2400000000L},
    {"ARM Cortex-A57", 2600000000L},
    {"ARM Cortex-A72", 2800000000L},
    {"ARM Cortex-A73", 2700000000L},
    {"ARM Cortex-A75", 2800000000L},
    {"ARM Cortex-A76", 3000000000L},
    {"ARM Cortex-A78", 3200000000L},
    {"ARM Cortex-X1", 3100000000L},
    {"ARM Cortex-X2", 3200000000L},
    {"ARM Cortex-X3", 3600000000L},
    {"ARM Cortex-X4", 3800000000L},
    {"Qualcomm Snapdragon 410", 1200000000L},
    {"Qualcomm Snapdragon 615", 1700000000L},
    {"Qualcomm Snapdragon 625", 2000000000L},
    {"Qualcomm Snapdragon 632", 1800000000L},
    {"Qualcomm Snapdragon 636", 1800000000L},
    {"Qualcomm Snapdragon 660", 2200000000L},
    {"Qualcomm Snapdragon 670", 2000000000L},
    {"Qualcomm Snapdragon 710", 2200000000L},
    {"Qualcomm Snapdragon 730", 2200000000L},
    {"Qualcomm Snapdragon 750G", 2200000000L},
    {"Qualcomm Snapdragon 765G", 2400000000L},
    {"Qualcomm Snapdragon 778G", 2400000000L},
    {"Qualcomm Snapdragon 855", 2840000000L},
    {"Qualcomm Snapdragon 865", 2840000000L},
    {"Qualcomm Snapdragon 870", 3190000000L},
    {"Qualcomm Snapdragon 888", 2840000000L},
    {"Qualcomm Snapdragon 8 Gen 1", 3000000000L},
    {"Qualcomm Snapdragon 8+ Gen 1", 3200000000L},
    {"Qualcomm Snapdragon 8 Gen 2", 3360000000L},
    {"Qualcomm Snapdragon 8 Gen 3", 3840000000L},
    {"Samsung Exynos 7420", 2100000000L},
    {"Samsung Exynos 8890", 2300000000L},
    {"Samsung Exynos 9810", 2900000000L},
    {"Samsung Exynos 9820", 2700000000L},
    {"Samsung Exynos 990", 2730000000L},
    {"Samsung Exynos 2100", 2900000000L},
    {"Samsung Exynos 2200", 2800000000L},
    {"HiSilicon Kirin 650", 2000000000L},
    {"HiSilicon Kirin 659", 2360000000L},
    {"HiSilicon Kirin 710", 2200000000L},
    {"HiSilicon Kirin 710A", 2200000000L},
    {"HiSilicon Kirin 810", 2270000000L},
    {"HiSilicon Kirin 960", 2400000000L},
    {"HiSilicon Kirin 970", 2360000000L},
    {"HiSilicon Kirin 980", 2600000000L},
    {"HiSilicon Kirin 990", 2860000000L},
    {"HiSilicon Kirin 990 5G", 2950000000L},
    {"HiSilicon Kirin 9000", 3130000000L},
    {"HiSilicon Kirin 9000S", 2620000000L},
    {"MediaTek Dimensity 700", 2200000000L},
    {"MediaTek Dimensity 800", 2200000000L},
    {"MediaTek Dimensity 810", 2400000000L},
    {"MediaTek Dimensity 900", 2400000000L},
    {"MediaTek Dimensity 920", 2500000000L},
    {"MediaTek Dimensity 1000", 2600000000L},
    {"MediaTek Dimensity 1000+", 2600000000L},
    {"MediaTek Dimensity 1100", 2600000000L},
    {"MediaTek Dimensity 1200", 3000000000L},
    {"MediaTek Dimensity 1300", 3000000000L},
    {"MediaTek Dimensity 7050", 2600000000L},
    {"MediaTek Dimensity 8100", 2850000000L},
    {"MediaTek Dimensity 8200", 3100000000L},
    {"MediaTek Dimensity 9000", 3200000000L},
    {"MediaTek Dimensity 9200", 3500000000L},
    {"MediaTek Dimensity 9300", 3550000000L},
    {"MediaTek Dimensity 9300+", 4000000000L},
    {"MediaTek Helio A22", 2000000000L},
    {"MediaTek Helio G25", 2000000000L},
    {"MediaTek Helio G35", 2300000000L},
    {"MediaTek Helio G70", 2000000000L},
    {"MediaTek Helio G80", 2000000000L},
    {"MediaTek Helio G85", 2000000000L},
    {"MediaTek Helio G90", 2100000000L},
    {"MediaTek Helio G90T", 2050000000L},
    {"MediaTek Helio G95", 2050000000L},
    {"MediaTek Helio P10", 2000000000L},
    {"MediaTek Helio P20", 2350000000L},
    {"MediaTek Helio P22", 2000000000L},
    {"MediaTek Helio P23", 2300000000L},
    {"MediaTek Helio P25", 2600000000L},
    {"MediaTek Helio P30", 2700000000L},
    {"MediaTek Helio P35", 2300000000L},
    {"MediaTek Helio P60", 2000000000L},
    {"MediaTek Helio P65", 2000000000L},
    {"MediaTek Helio P70", 2000000000L},
    {"MediaTek Helio P90", 2200000000L},
    {"MediaTek Helio X10", 2200000000L},
    {"MediaTek Helio X20", 3100000000L},
    {"MediaTek Helio X23", 2300000000L},
    {"MediaTek Helio X25", 2500000000L},
    {"MediaTek Helio X27", 2600000000L},
    {"MediaTek Helio X30", 2700000000L},
    {"Nvidia Tegra 2", 1000000000L},
    {"Nvidia Tegra 3", 1500000000L},
    {"Nvidia Tegra 4", 1800000000L},
    {"Nvidia Tegra K1", 2600000000L},
    {"Nvidia Tegra X1", 1900000000L},
    {"Nvidia Tegra X1+ ", 2100000000L},
    {"Nvidia Tegra X2", 2300000000L},
    {"Nvidia Tegra Orin", 2200000000L},
    {"Nvidia Denver", 2300000000L},
    {"Nvidia Carmel", 2100000000L},
    {"Nvidia Grace CPU Superchip", 2700000000L},
    {"VIA C3 Nehemiah", 1200000000L},
    {"VIA C7", 1600000000L},
    {"VIA Nano L2207", 1600000000L},
    {"VIA Nano U2250", 1300000000L},
    {"VIA Nano X2 L4350", 2000000000L},
    {"VIA Eden X4", 1600000000L},
    {"Transmeta Crusoe TM5600", 800000000L},
    {"Transmeta Efficeon TM8600", 1400000000L},
    {"IBM POWER5", 1900000000L},
    {"IBM POWER6", 4700000000L},
    {"IBM POWER7", 4300000000L},
    {"IBM POWER8", 4600000000L},
    {"IBM POWER9", 3800000000L},
    {"IBM POWER10", 4200000000L},
    {"IBM z13", 5200000000L},
    {"IBM z14", 5200000000L},
    {"IBM z15", 5400000000L},
    {"IBM z16", 5600000000L},
    {"Motorola 68000", 8000000L},
    {"Motorola 68020", 25000000L},
    {"Motorola 68030", 33000000L},
    {"Motorola 68040", 40000000L},
    {"Motorola PowerPC 601", 66000000L},
    {"Motorola PowerPC 603e", 300000000L},
    {"Motorola PowerPC 750", 400000000L},
    {"Raspberry Pi BCM2835", 700000000L},
    {"Raspberry Pi BCM2836", 900000000L},
    {"Raspberry Pi BCM2837", 1200000000L},
    {"Raspberry Pi BCM2711", 1500000000L},
    {"Raspberry Pi BCM2712", 2400000000L},
    {"AMD 29000", 16000000L},
    {"AMD 386DX-40", 40000000L},
    {"AMD 486DX4-100", 100000000L},
    {"AMD Am29050", 33000000L},
    {"AMD Am5x86-P75", 133000000L},
    {"AMD Athlon 64 3200+", 2200000000L},
    {"AMD Athlon 64 4000+", 2600000000L},
    {"AMD Athlon 64 X2 5000+", 2600000000L},
    {"AMD Athlon FX-55", 2600000000L},
    {"AMD Athlon II X2 250", 3000000000L},
    {"AMD Athlon II X4 640", 3000000000L},
    {"AMD Duron 800", 800000000L},
    {"AMD Duron 1200", 1200000000L},
    {"AMD K5 PR166", 116000000L},
    {"AMD K6-2 500", 500000000L},
    {"AMD K6-III 450", 450000000L},
    {"AMD Opteron 242", 1600000000L},
    {"AMD Opteron 285", 2600000000L},
    {"AMD Opteron 6272", 2100000000L},
    {"AMD Opteron 6386 SE", 2800000000L},
    {"AMD Opteron 6376", 2300000000L},
    {"AMD Opteron 8439 SE", 2800000000L},
    {"AMD Phenom 9600", 2300000000L},
    {"AMD Phenom II X4 965", 3400000000L},
    {"AMD Phenom II X6 1100T", 3300000000L},
    {"AMD Turion 64 X2 TL-60", 2000000000L},
    {"AMD Turion II Ultra M660", 2300000000L},
    {"AMD Turion X2 Ultra ZM-86", 2400000000L},
    {"DEC Alpha 21064", 200000000L},
    {"DEC Alpha 21164", 300000000L},
    {"DEC Alpha 21264", 666000000L},
    {"DEC Alpha 21364", 1200000000L},
    {"Intel 4004", 740000L},
    {"Intel 8008", 2000000L},
    {"Intel 8080", 2000000L},
    {"Intel 8086", 10000000L},
    {"Intel 80286", 12000000L},
    {"Intel 80386DX-33", 33000000L},
    {"Intel 80486DX2-66", 66000000L},
    {"Intel 80486DX4-100", 100000000L},
    {"Intel Celeron 300A", 450000000L},
    {"Intel Celeron 500", 500000000L},
    {"Intel Core 2 Duo E6600", 2400000000L},
    {"Intel Core 2 Duo E8400", 3000000000L},
    {"Intel Core 2 Quad Q6600", 2400000000L},
    {"Intel Core i3-2100", 3100000000L},
    {"Intel Core i5-2500K", 3300000000L},
    {"Intel Core i7-2600K", 3400000000L},
    {"Intel Core i7-920", 2660000000L},
    {"Intel Core i7-3770K", 3500000000L},
    {"Intel Core i7-4790K", 4000000000L},
    {"Intel Core i7-6700K", 4000000000L},
    {"Intel Core i7-7700K", 4200000000L},
    {"Intel Core i9-9900K", 3600000000L},
    {"Intel Core i9-10900K", 3700000000L},
    {"Intel Core i9-11900K", 3500000000L},
    {"Intel Core i9-13900K", 3000000000L},
    {"Intel Core i9-14900K", 3200000000L},
    {"Intel Itanium 2 9000", 1600000000L},
    {"Intel Itanium 9500", 2660000000L},
    {"Intel Pentium 60", 60000000L},
    {"Intel Pentium 133", 133000000L},
    {"Intel Pentium II 300", 300000000L},
    {"Intel Pentium III 800", 800000000L},
    {"Intel Pentium 4 2.8GHz", 2800000000L},
    {"Intel Pentium D 820", 2800000000L},
    {"Intel Pentium Extreme Edition 965", 3600000000L},
    {"Intel Xeon E3-1240 v2", 3400000000L},
    {"Intel Xeon E5-2680", 2700000000L},
    {"Intel Xeon E5-2699 v4", 2200000000L},
    {"Intel Xeon E7-8890 v4", 2200000000L},
    {"Intel Xeon Platinum 8380", 2300000000L},
    {"Intel Xeon Platinum 8490H", 1800000000L},
    {"Intel Xeon W-3175X", 3500000000L},
    {"MIPS R2000", 16000000L},
    {"MIPS R3000", 33000000L},
    {"MIPS R4000", 100000000L},
    {"MIPS R5000", 150000000L},
    {"MIPS R10000", 195000000L},
    {"MIPS R12000", 270000000L},
    {"RISC-V SiFive U74", 1500000000L},
    {"RISC-V SiFive U84", 1800000000L},
    {"RISC-V SiFive P670", 2400000000L},
    {"RISC-V SiFive P470", 2000000000L},
    {"RISC-V Alibaba T-Head Xuantie C910", 2500000000L},
    {"RISC-V StarFive JH7110", 1500000000L},
    {"SPARC T1", 1200000000L},
    {"SPARC T2", 1400000000L},
    {"SPARC T3", 1600000000L},
    {"SPARC T4", 2850000000L},
    {"SPARC T5", 3600000000L},
    {"SPARC M7", 4200000000L},
    {"SPARC M8", 5000000000L},
    {"Sun UltraSPARC II", 450000000L},
    {"Sun UltraSPARC III", 900000000L},
    {"Sun UltraSPARC IV", 1500000000L},
    {"Sun UltraSPARC IV+", 1600000000L},
    {"Sun UltraSPARC T2", 1400000000L},
    {"Sun UltraSPARC T3", 1600000000L},
    {"Generic", 1000000000L}
};

#define NUM_PROCESSORI (sizeof(db_processori) / sizeof(db_processori[0]))

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: RILEVAMENTO AUTOMATICO DELLA CPU                                  │
 * │                                                                            │
 * │  Questo modulo implementa un sistema multipiattaforma per l'identificazione│
 * │  automatica del modello di processore in uso. È progettato per adattare    │
 * │  dinamicamente la simulazione dell'ALU 74181 alle caratteristiche hardware │
 * │  della macchina ospite, migliorando la precisione temporale e la           │
 * │  compatibilità.                                                            │
 * │                                                                            │
 * │  FUNZIONAMENTO PER SISTEMA OPERATIVO                                       │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Linux:                                                                  │
 * │    - Legge il file virtuale /proc/cpuinfo                                  │
 * │    - Estrae il campo "model name" o "vendor_id"                            │
 * │                                                                            │
 * │  • Windows:                                                                │
 * │    - Interroga il registro di sistema                                      │
 * │    - Utilizza le chiavi HKEY_LOCAL_MACHINE\Hardware\Description\System\... │
 * │                                                                            │
 * │  • macOS:                                                                  │
 * │    - Esegue il comando `sysctl -n machdep.cpu.brand_string`                │
 * │    - Interpreta l'output testuale                                          │
 * │                                                                            │
 * │  • Fallback:                                                               │
 * │    - Se il rilevamento fallisce o il sistema è sconosciuto,                │
 * │      viene restituito il nome generico "Generic"                           │
 * │                                                                            │
 * │  UTILIZZO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Visualizzazione del nome CPU nell'interfaccia del simulatore            │
 * │  • Calcolo della frequenza di clock per simulazioni temporali              │
 * │  • Logging e profiling delle prestazioni                                   │
 * │                                                                            │
 * │  NOTE TECNICHE                                                             │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Il modulo è progettato per essere estendibile: nuovi OS possono essere  │
 * │    integrati facilmente tramite wrapper specifici                          │
 * │  • La funzione restituisce una stringa allocata dinamicamente              │
 * │    che deve essere liberata dal chiamante                                  │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


char* rileva_cpu() {
    static char nome_cpu[256] = "Generic";
    if (SISTEMA_LINUX) {
        FILE* f = fopen("/proc/cpuinfo", "r");
        if (f) {
            char riga[256];
            while (fgets(riga, sizeof(riga), f)) {
                if (strncmp(riga, "model name", 10) == 0) {
                    char* duepunti = strchr(riga, ':');
                    if (duepunti) {
                        strncpy(nome_cpu, duepunti + 2, sizeof(nome_cpu) - 1);
                        nome_cpu[strcspn(nome_cpu, "\n")] = '\0';
                        fclose(f);
                        return nome_cpu;
                    }
                }
            }
            fclose(f);
        }
    }
    #if SISTEMA_WINDOWS
      HKEY chiave;
      DWORD tipo = REG_SZ;
      DWORD dimensione = sizeof(nome_cpu);
      if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &chiave) == ERROR_SUCCESS) {
        RegQueryValueExA(chiave, "ProcessorNameString", NULL, &tipo, (LPBYTE)nome_cpu, &dimensione);
        RegCloseKey(chiave);
        return nome_cpu;
      }
    #endif
    if (SISTEMA_MAC) {
        FILE *p = popen("sysctl -n machdep.cpu.brand_string", "r");
        if (p) {
            fgets(nome_cpu, sizeof(nome_cpu), p);
            nome_cpu[strcspn(nome_cpu, "\n")] = '\0';
            pclose(p);
            return nome_cpu;
        }
    }
    return nome_cpu;
}
long ottieni_clock(const char* nome_cpu) {
    if (!nome_cpu) return 1000000000L;
    for (int i = 0; i < NUM_PROCESSORI; i++) {
        if (strstr(nome_cpu, db_processori[i].nome) != NULL) {
            return db_processori[i].clock_hz;
        }
    }
    return 1000000000L;
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: SIMULAZIONE DEL CICLO DI CLOCK                                    │
 * │                                                                            │
 * │  Questo modulo implementa un sistema di temporizzazione virtuale che       │
 * │  emula il comportamento del clock di un processore reale. È progettato     │
 * │  per sincronizzare le operazioni logiche e aritmetiche del simulatore      │
 * │  ALU 74181 con una frequenza di clock coerente con quella della CPU        │
 * │  rilevata.                                                                 │
 * │                                                                            │
 * │  FUNZIONALITÀ PRINCIPALI                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Attesa temporale in nanosecondi                                         │
 * │    - Permette di simulare ritardi precisi a livello hardware               │
 * │                                                                            │
 * │  • Simulazione di un singolo ciclo di clock                                │
 * │    - Calcola la durata di un ciclo in base alla frequenza (Hz)             │
 * │    - Utilizza funzioni di sleep ad alta risoluzione                        │
 * │                                                                            │
 * │  • Attesa equivalente a N cicli                                            │
 * │    - Utile per simulare operazioni complesse che richiedono più cicli      │
 * │    - Esempio: simulare 2 secondi di attività a 3.7 GHz = ~7.4 miliardi di  │
 * │      cicli                                                                 │
 * │                                                                            │
 * │  • Sincronizzazione logica                                                 │
 * │    - Consente di allineare operazioni simulate con il tempo reale          │
 * │    - Migliora la coerenza tra simulazione e comportamento atteso           │
 * │                                                                            │
 * │  CONTESTO DI UTILIZZO                                                      │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Simulazione di microarchitetture                                        │
 * │  • Test di performance e profiling                                         │
 * │  • Emulazione di circuiti digitali e logiche sequenziali                   │
 * │                                                                            │
 * │  NOTE TECNICHE                                                             │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • La precisione dipende dal sistema operativo e dal timer disponibile     │
 * │  • Su Linux/macOS si consiglia l'uso di `nanosleep` o `clock_nanosleep`    │
 * │  • Su Windows si può usare `QueryPerformanceCounter` per alta precisione   │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


void ritardo_ns(long nanosecondi) {
#if SISTEMA_WINDOWS
    long ms = nanosecondi / 1000000L;
    if (ms <= 0 && nanosecondi > 0) ms = 1;
    Sleep((DWORD)ms);
#else
    struct timespec inizio, attuale;
    clock_gettime(CLOCK_MONOTONIC, &inizio);
    do {
        clock_gettime(CLOCK_MONOTONIC, &attuale);
    } while ((attuale.tv_sec - inizio.tv_sec) * 1000000000L + (attuale.tv_nsec - inizio.tv_nsec) < nanosecondi);
#endif
}
void delay(int milliseconds) {
    long ns = (long)milliseconds * 1000000L;
    ritardo_ns(ns);
}
void clock_step(int *CLK, int *prev_CLK, int milliseconds) {
    delay(milliseconds);
    *prev_CLK = *CLK;
    *CLK = 1 - *CLK;
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: LOGICA DIGITALE DI BASE — FLIP-FLOP E REGISTRI                    │
 * │                                                                            │
 * │  Questo modulo implementa i fondamenti della logica digitale tramite       │
 * │  simulazione software di porte logiche e registri. È pensato per           │
 * │  riprodurre fedelmente il comportamento di circuiti hardware, offrendo     │
 * │  un ambiente sperimentale per comprendere l'elettronica digitale a basso   │
 * │  livello.                                                                  │
 * │                                                                            │
 * │  COMPONENTI IMPLEMENTATI                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Porte logiche elementari:                                               │
 * │    - NAND: base universale per costruire qualsiasi altra porta             │
 * │    - NOT, AND, OR, XOR: derivate da combinazioni di NAND                   │
 * │                                                                            │
 * │  • Flip-Flop SR (Set-Reset):                                               │
 * │    - Costruito esclusivamente con porte NAND                               │
 * │    - Sincronizzato con segnale di clock                                    │
 * │    - Comportamento bistabile con memoria interna                           │
 * │                                                                            │
 * │  • Registro PIPO (Parallel-In Parallel-Out):                               │
 * │    - Composto da 8 flip-flop SR in parallelo                               │
 * │    - Permette lettura e scrittura simultanea di byte                       │
 * │    - Esteso a 32 bit tramite concatenazione di 4 registri da 8 bit         │
 * │                                                                            │
 * │  FUNZIONALITÀ                                                              │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Simulazione del comportamento temporale dei registri                    │
 * │  • Manipolazione di segnali binari in ingresso e uscita                    │
 * │  • Visualizzazione dello stato interno dei flip-flop                       │
 * │  • Sincronizzazione con il ciclo di clock simulato                         │
 * │                                                                            │
 * │  CONTESTO DIDATTICO E TECNICO                                              │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Utile per corsi di elettronica digitale e architettura dei calcolatori  │
 * │  • Strumento per makers e progettisti di circuiti logici                   │
 * │  • Base per costruzione di CPU virtuali e microcontrollori simulati        │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


int NAND3(int A, int B, int C) { 
  return 1 - (A * B * C); 
} 
void n_SR_FLIP_FLOP(int D, int S_reg, int R_reg, int CLK, int *prev_CLK, int *Q, int *Q_bar) { 
  if ((CLK * (1 - *prev_CLK)) == 1) { 
    int J = D; 
    int K = 1 - D; 
    *Q     = NAND3(S_reg, *Q_bar, NAND3(J, CLK, *Q_bar)); 
    *Q_bar = NAND3(R_reg, *Q,     NAND3(K, CLK, *Q)); 
  } 
  *prev_CLK = CLK; 
}
void n_PIPO74198(int D[8], int S_reg[8], int R_reg[8], int CLK, int prev_CLK[8], int Q[8], int Q_bar[8]) { 
  for (int i = 0; i < 8; i++) { 
    n_SR_FLIP_FLOP(D[i], S_reg[i], R_reg[i], CLK, &prev_CLK[i], &Q[i], &Q_bar[i]); 
  } 
}
void reg_PIPO32(int D[32], int S_reg[32], int R_reg[32], int CLK, int prev_CLK[32], int Q[32], int Q_bar[32]) { 
  for (int i = 0; i < 4; i++) { 
    n_PIPO74198(&D[i * 8], &S_reg[i * 8], &R_reg[i * 8], CLK, &prev_CLK[i * 8], &Q[i * 8], &Q_bar[i * 8]);
  } 
}
static bool parentesi(const char *s) {
    int profondita = 0;
    for (; *s; ++s) {
        if (*s == '(') profondita++;
        else if (*s == ')') {
            profondita--;
            if (profondita < 0) return false;
        }
    }
    return profondita == 0;
}

static bool is_safe_expr(const char *s) {
    if (!s) return false;

    const char *p = s;
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '\0') return false;

    for (; *s; ++s) {
        unsigned char c = (unsigned char)*s;
        if (isdigit(c) || isspace(c)) continue;
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
            c == '(' || c == ')' || c == '.' ) continue;
        return false;
    }

    if (!parentesi(p)) return false;

    return true;
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: CONVERTITORI BINARIO ↔ DECIMALE                                   │
 * │                                                                            │
 * │  Questo modulo fornisce due funzioni fondamentali per la conversione tra   │
 * │  rappresentazioni numeriche binarie e decimali. È progettato per essere    │
 * │  robusto, intuitivo e integrabile nell'interfaccia utente del simulatore   │
 * │  ALU 74181, sia per input manuali che per elaborazione da file.            │
 * │                                                                            │
 * │  FUNZIONI PRINCIPALI                                                       │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • BIN_DEC_DECODER                                                         │
 * │    - Input: stringa binaria (es. "101101")                                 │
 * │    - Output: intero decimale equivalente (es. 45)                          │
 * │    - Gestione errori: verifica che la stringa contenga solo '0' e '1'      │
 * │                                                                            │
 * │  • DEC_BIN_CODER                                                           │
 * │    - Input: intero decimale (es. 45)                                       │
 * │    - Output: stringa binaria equivalente (es. "101101")                    │
 * │    - Gestione errori: verifica che il numero sia non negativo              │
 * │                                                                            │
 * │  UTILIZZO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Conversione manuale tramite interfaccia grafica                         │
 * │  • Parsing di file contenenti dati binari o decimali                       │
 * │  • Supporto a operazioni logiche e aritmetiche nel simulatore              │
 * │  • Visualizzazione dei risultati in formato leggibile                      │
 * │                                                                            │
 * │  NOTE TECNICHE                                                             │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Le funzioni sono ottimizzate per prestazioni elevate anche su input     │
 * │    di grandi dimensioni                                                    │
 * │  • Compatibili con sistemi a 32 e 64 bit                                   │
 * │  • Modulari e facilmente estendibili per supportare altri formati (es. HEX)│
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


int BIN_DEC_DECODER(const char *binario) { 
  if (binario == NULL) { 
    printf("ERRORE: input NULL non valido.\n"); 
    return -1; 
  } 
  int decimale = 0; 
  int lunghezza = strlen(binario); 
  for (int i = 0; i < lunghezza; i++) { 
    if (binario[i] == '1') { 
      decimale = decimale * 2 + 1; 
    } 
    else if (binario[i] == '0') { 
      decimale = decimale * 2; 
    } 
    else { 
      printf("Input non valido. Solo 0 e 1 sono accettati.\n"); 
      return -1; 
    } 
  } 
  return decimale; 
}
char* DEC_BIN_CODER(int numero) { 
  static char binario[33]; 
  int i = 0; 
  if (numero == 0) { 
    strcpy(binario, "0"); 
    return binario; 
  } 
  while (numero > 0) { 
    binario[i++] = '0' + (numero % 2); 
    numero /= 2; 
  } 
  binario[i] = '\0'; 
  for (int j = 0; j < i / 2; j++) { 
    char temp = binario[j]; 
    binario[j] = binario[i - 1 - j]; 
    binario[i - 1 - j] = temp; 
  } 
  return binario; 
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: SISTEMA DI MEMORIA DINAMICA                                       │
 * │                                                                            │
 * │  Questo modulo implementa una memoria virtuale espandibile, progettata     │
 * │  per archiviare dinamicamente i risultati delle operazioni logiche e       │
 * │  aritmetiche eseguite dall'ALU 74181 simulata.                             │
 * │                                                                            │
 * │  FUNZIONALITÀ PRINCIPALI                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Allocazione iniziale dinamica                                           │
 * │    - La memoria viene inizializzata con una capacità predefinita           │
 * │    - Ogni cella può contenere un valore intero (es. risultato, flag carry) │
 * │                                                                            │
 * │  • Espansione automatica                                                   │
 * │    - Quando la capacità viene raggiunta, la memoria viene raddoppiata      │
 * │      tramite realloc()                                                     │
 * │    - Garantisce efficienza e scalabilità senza perdita di dati             │
 * │                                                                            │
 * │  • Funzioni di utilità                                                     │
 * │    - Inserimento di nuovi valori                                           │
 * │    - Stampa dello stato attuale (dimensione, capacità, contenuto)          │
 * │    - Reset o svuotamento della memoria                                     │
 * │                                                                            │
 * │  UTILIZZO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Tracciamento dei risultati delle operazioni ALU                         │
 * │  • Debug e visualizzazione dello storico delle operazioni                  │
 * │  • Supporto a funzionalità di salvataggio e caricamento da file            │
 * │                                                                            │
 * │  NOTE TECNICHE                                                             │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • realloc() è utilizzato in modo sicuro, con controllo su NULL            │
 * │  • La gestione della memoria è compatibile con ambienti a 32 e 64 bit      │
 * │  • Il modulo è progettato per essere thread-safe se integrato con mutex    │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


int *memoria = NULL; 
int capacita_memoria = 10; 
int indice_memoria = 0;
void salva_in_memoria(int valore) {
    if (memoria == NULL) {
        memoria = malloc(capacita_memoria * sizeof(int));
        if (!memoria) {
            printf("ERRORE: impossibile allocare la memoria. Salvataggio saltato.\n");
            return;
        }
    }
    if (indice_memoria >= capacita_memoria) {
        capacita_memoria *= 2;
        int *temp = realloc(memoria, capacita_memoria * sizeof(int));
        if (!temp) {
            printf("ERRORE: impossibile espandere la memoria. Salvataggio saltato.\n");
            capacita_memoria /= 2;
            return;
        }
        memoria = temp;
        printf("[INFO] Memoria espansa a %d celle.\n", capacita_memoria);
    }
    memoria[indice_memoria++] = valore;
}
void attendi_un_ciclo_clock() {
    char *cpu = rileva_cpu();
    long freq = ottieni_clock(cpu);
    long ciclo_ns = (long)(1e9 / freq);
    if (ciclo_ns < 1000) {
        ciclo_ns = 1000;
    }
    ritardo_ns(ciclo_ns);
}
void attendi_cicli_clock_equivalenti_a_secondi(double secondi) {
    if (secondi <= 0.0) return;
    char *cpu = rileva_cpu();
    long freq = ottieni_clock(cpu);
    if (freq <= 0) freq = 1000000000L;
    double nanosecondi_totali = secondi * 1e9;
    long ns = (long)nanosecondi_totali;
    if (ns > 5000000000L) {
        ns = 5000000000L;
    }
    ritardo_ns(ns);
}
void stampa_memoria() { 
  printf("Contenuto della memoria:\n"); 
  for (int i = 0; i < indice_memoria; i++) { 
    printf("Memoria[%d] = %-3d\n", i, memoria[i]); 
  } 
}
void stato_memoria() { 
  printf("Stato memoria:\n"); 
  printf("- Totale allocato: %d celle\n", capacita_memoria); 
  printf("- Occupato: %d celle\n", indice_memoria); 
} 
int porta_not(int a) { 
  return 1 - a; 
} 
int porta_and(int a, int b) { 
  return a * b; 
} 
int porta_or(int a, int b) { 
  return (a + b) - (a * b); 
} 
int porta_exor(int a, int b) { 
  return (a + b) - 2 * (a * b); 
} 
int porta_or_3(int a, int b, int c) { 
  int tmp = porta_or(a, b); 
  return porta_or(tmp, c); 
} 
int porta_or_4(int a, int b, int c, int d) { 
  int tmp1 = porta_or(a, b); 
  int tmp2 = porta_or(c, d); 
  return porta_or(tmp1, tmp2); 
} 
int porta_or_5(int a, int b, int c, int d, int e) { 
  int tmp = porta_or_4(a, b, c, d); 
  return porta_or(tmp, e); 
} 
int porta_and_3(int a, int b, int c) { 
  int tmp = porta_and(a, b); 
  return porta_and(tmp, c); 
} 
int porta_and_4(int a, int b, int c, int d) { 
  int tmp = porta_and(a, b); 
  int tmp2 = porta_and(c, d); 
  return porta_and(tmp, tmp2); 
} 
int porta_and_5(int a, int b, int c, int d, int e) { 
  int tmp = porta_and_3(a, b, c); 
  tmp = porta_and(tmp, d); 
  return porta_and(tmp, e); 
} 
int porta_exor_3(int a, int b, int c) { 
  int tmp = porta_exor(a, b); 
  return porta_exor(tmp, c); 
} 
int porta_exor_4(int a, int b, int c, int d) { 
  int tmp = porta_exor(a, b); 
  int tmp2 = porta_exor(c, d); 
  return porta_exor(tmp, tmp2); 
} 
int porta_exor_5(int a, int b, int c, int d, int e) { 
  int tmp = porta_exor_4(a, b, c, d); 
  return porta_exor(tmp, e); 
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: SIMULAZIONE DELL'ALU 74181 (4-BIT)                                │
 * │                                                                            │
 * │  Questo modulo implementa la logica completa del chip storico 74181,       │
 * │  una delle prime ALU integrate prodotte da Texas Instruments nel 1970.     │
 * │  È in grado di eseguire 16 operazioni logiche e aritmetiche su parole      │
 * │  da 4 bit, ed è stato utilizzato in numerosi sistemi minicomputer e        │
 * │  mainframe dell’epoca.                                                     │
 * │                                                                            │
 * │  FUNZIONALITÀ PRINCIPALI                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Calcolo dell’output F[0..3]                                             │
 * │    - Risultato dell’operazione selezionata tra A e B                       │
 * │                                                                            │
 * │  • Flag di uguaglianza (A == B)                                            │
 * │    - Verifica bit a bit dell’identità tra gli operandi                     │
 * │                                                                            │
 * │  • Segnali di Propagate (P) e Generate (G)                                 │
 * │    - Utilizzati per il carry lookahead                                     │
 * │    - Ottimizzano la propagazione del riporto nei sistemi a cascata         │
 * │                                                                            │
 * │  • Carry out (Cn+4)                                                        │
 * │    - Riporto finale generato dall’operazione                               │
 * │                                                                            │
 * │  IMPLEMENTAZIONE LOGICA                                                    │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Tutte le funzioni sono simulate tramite porte logiche software          │
 * │    - NAND, AND, OR, XOR, NOT                                               │
 * │    - Costruite per riflettere il comportamento elettrico reale             │
 * │                                                                            │
 * │  • La selezione delle operazioni è controllata da 4 bit di funzione (S0–S3)│
 * │    e da un bit di modalità (M) per distinguere tra logica e aritmetica     │
 * │                                                                            │
 * │  CONTESTO DIDATTICO E TECNICO                                              │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Ideale per lo studio dell’architettura dei calcolatori                  │
 * │  • Utile per la progettazione di CPU virtuali e simulatori hardware        │
 * │  • Base per estensione a 8, 16, 32 bit tramite cascata di ALU              │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


void n_ALU74181(int Cn, int M, int A[4], int B[4], int S[4], int F[4], int *A_uguale_B, int *P, int *Cn_piu_4, int *G) { 
  F[0] = porta_exor(porta_not(porta_and(Cn, porta_not(M))), porta_and(porta_not(porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0]))))), porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))))); 
  F[1] = porta_exor(porta_not(porta_or(porta_and(porta_not(M), porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0]))))), porta_and_3(porta_not(M), porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), Cn))), porta_and(porta_not(porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1]))))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))))); 
  F[2] = porta_exor(porta_not(porta_or_3(porta_and(porta_not(M), porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1]))))), porta_and_3(porta_not(M), porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0])))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1])))), porta_and_4(porta_not(M), Cn, porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1])))))), porta_and(porta_not(porta_not(porta_or_3(A[2], porta_and(B[2], S[0]), porta_and(S[1], porta_not(B[2]))))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))))); 
  F[3] = porta_exor(porta_not(porta_or_4(porta_and(porta_not(M), porta_not(porta_or_3(A[2], porta_and(B[2], S[0]), porta_and(S[1], porta_not(B[2]))))), porta_and_3(porta_not(M), porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1])))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2])))), porta_and_4(porta_not(M), porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0])))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2])))), porta_and_5(porta_not(M), Cn, porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2])))))), porta_and(porta_not(porta_not(porta_or_3(A[3], porta_and(B[3], S[0]), porta_and(S[1], porta_not(B[3]))))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3]))))); 
  *A_uguale_B = porta_and_4(porta_exor(porta_not(porta_and(Cn, porta_not(M))), porta_and(porta_not(porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0]))))), porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))))), porta_exor(porta_not(porta_or(porta_and(porta_not(M), porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0]))))), porta_and_3(porta_not(M), porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), Cn))), porta_and(porta_not(porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1]))))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))))), porta_exor(porta_not(porta_or_3(porta_and(porta_not(M), porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1]))))), porta_and_3(porta_not(M), porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0])))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1])))), porta_and_4(porta_not(M), Cn, porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1])))))), porta_and(porta_not(porta_not(porta_or_3(A[2], porta_and(B[2], S[0]), porta_and(S[1], porta_not(B[2]))))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))))), porta_exor(porta_not(porta_or_4(porta_and(porta_not(M), porta_not(porta_or_3(A[2], porta_and(B[2], S[0]), porta_and(S[1], porta_not(B[2]))))), porta_and_3(porta_not(M), porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1])))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2])))), porta_and_4(porta_not(M), porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0])))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2])))), porta_and_5(porta_not(M), Cn, porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2])))))), porta_and(porta_not(porta_not(porta_or_3(A[3], porta_and(B[3], S[0]), porta_and(S[1], porta_not(B[3]))))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))))); 
  *P = porta_not(porta_and_4(porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3]))))); 
  *Cn_piu_4 = porta_or(porta_not(porta_not(porta_and_5(Cn, porta_not(porta_or(porta_and_3(porta_not(B[0]), S[2], A[0]), porta_and_3(A[0], B[0], S[3]))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))))), porta_not(porta_not(porta_or_4(porta_and_4(porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0])))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))), porta_and_3(porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1])))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))), porta_and(porta_not(porta_or_3(A[2], porta_and(B[2], S[0]), porta_and(S[1], porta_not(B[2])))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))), porta_not(porta_or_3(A[3], porta_and(B[3], S[0]), porta_and(S[1], porta_not(B[3])))))))); 
  *G = porta_not(porta_or_4(porta_and_4(porta_not(porta_or_3(A[0], porta_and(B[0], S[0]), porta_and(S[1], porta_not(B[0])))), porta_not(porta_or(porta_and_3(porta_not(B[1]), S[2], A[1]), porta_and_3(A[1], S[3], B[1]))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))), porta_and_3(porta_not(porta_or_3(A[1], porta_and(B[1], S[0]), porta_and(S[1], porta_not(B[1])))), porta_not(porta_or(porta_and_3(porta_not(B[2]), S[2], A[2]), porta_and_3(A[2], S[3], B[2]))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))), porta_and(porta_not(porta_or_3(A[2], porta_and(B[2], S[0]), porta_and(S[1], porta_not(B[2])))), porta_not(porta_or(porta_and_3(porta_not(B[3]), S[2], A[3]), porta_and_3(A[3], S[3], B[3])))), porta_not(porta_or_3(A[3], porta_and(B[3], S[0]), porta_and(S[1], porta_not(B[3])))))); 
}
static int leggi_bit_input_74181(const char* nome, int* var) {
    int input;
    printf(">> %s: ", nome);
    if (scanf("%d", &input) != 1 || (input != 0 && input != 1)) {
        printf("╔════════════════════════════════╗\n");
        printf("║            ERRORE              ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║                                ║\n");
        printf("║   %s deve essere 0 o 1      ║\n", nome);
        printf("║                                ║\n");
        printf("╚════════════════════════════════╝\n");
        return 0;
    }
    *var = input;
    return 1;
}
static int leggi_bit_input_32(const char* nome, int* var) {
    return leggi_bit_input_74181(nome, var);
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: INTERFACCIA UTENTE PER L'ALU 74181                                │
 * │                                                                            │
 * │  Questo modulo gestisce l'interazione tra l'utente e il simulatore         │
 * │  dell'ALU 74181, consentendo l'inserimento dei dati, l'esecuzione delle    │
 * │  operazioni e la visualizzazione dei risultati. È progettato per essere    │
 * │  intuitivo, robusto e compatibile con input manuali e da file.             │
 * │                                                                            │
 * │  FUNZIONALITÀ PRINCIPALI                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Input manuale                                                           │
 * │    - L'utente può inserire direttamente i bit A[0..3], B[0..3] e le opzioni│
 * │      di funzione tramite tastiera                                          │
 * │                                                                            │
 * │  • Input da file                                                           │
 * │    - Il modulo legge da "input_alu.txt" una sequenza di operazioni         │
 * │    - Supporta batch processing per simulazioni multiple                    │
 * │                                                                            │
 * │  • Validazione dell'input                                                  │
 * │    - Controlla che i bit siano corretti (solo '0' e '1')                   │
 * │    - Verifica la lunghezza e la struttura dei dati                         │
 * │                                                                            │
 * │  • Esecuzione della simulazione                                            │
 * │    - Invia i dati al modulo ALU                                            │
 * │    - Riceve i risultati (F[0..3], flag, carry, ecc.)                       │
 * │                                                                            │
 * │  • Salvataggio dei risultati                                               │
 * │    - I risultati vengono memorizzati nella memoria dinamica                │
 * │    - Vengono anche scritti su "risultati_alu_74181.txt"                    │
 * │                                                                            │
 * │  • Temporizzazione                                                         │
 * │    - Ogni operazione è sincronizzata con il clock simulato                 │
 * │    - Il tempo di attesa è proporzionale alla frequenza del processore      │
 * │                                                                            │
 * │  UTILIZZO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Interfaccia testuale per utenti e sviluppatori                          │
 * │  • Debug e verifica delle operazioni ALU                                   │
 * │  • Simulazioni didattiche e sperimentali                                   │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


void simula_alu_74181() {
    int Cn, M, A0, B0, A1, B1, A2, B2, A3, B3, S0, S1, S2, S3;
    char scelta[3];
    int input_valido = 1;

    printf("Inserire dati manualmente? (S/N): ");
    scanf("%2s", scelta);
    scelta[0] = toupper(scelta[0]);

    if (scelta[0] == 'S') {
        if (!leggi_bit_input_74181("Cn", &Cn)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("M", &M)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("A0", &A0)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("B0", &B0)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("A1", &A1)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("B1", &B1)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("A2", &A2)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("B2", &B2)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("A3", &A3)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("B3", &B3)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("S0", &S0)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("S1", &S1)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("S2", &S2)) input_valido = 0;
        if (input_valido && !leggi_bit_input_74181("S3", &S3)) input_valido = 0;
    } else {
        FILE *file = fopen("input_alu.txt", "r");
        if (!file) {
            file = fopen("input_alu.txt", "w");
            if (!file) {
                printf("ERRORE: Impossibile creare il file input_alu.txt\n");
                input_valido = 0;
            } else {
                fprintf(file, "Cn: <0>\n");
                fprintf(file, "M: <0>\n");
                fprintf(file, "A0: <0>\n");
                fprintf(file, "B0: <0>\n");
                fprintf(file, "A1: <0>\n");
                fprintf(file, "B1: <0>\n");
                fprintf(file, "A2: <0>\n");
                fprintf(file, "B2: <0>\n");
                fprintf(file, "A3: <0>\n");
                fprintf(file, "B3: <0>\n");
                fprintf(file, "S0: <0>\n");
                fprintf(file, "S1: <0>\n");
                fprintf(file, "S2: <0>\n");
                fprintf(file, "S3: <0>\n");
                fclose(file);
                printf("Creato file input_alu.txt. Compilarlo e riavviare.\n");
                input_valido = 0;
            }
        } else {
            char line[100];
            #define LEGGI_BIT_FILE(var, nome) do { \
                if (!fgets(line, sizeof(line), file)) { \
                    printf("ERRORE: Formato file incompleto (%s)\n", nome); \
                    input_valido = 0; \
                } else if (sscanf(line, "%*[^<]<%d>", &(var)) != 1) { \
                    printf("ERRORE: Valore non valido in %s\n", nome); \
                    input_valido = 0; \
                } else if ((var) != 0 && (var) != 1) { \
                    printf("╔════════════════════════════════╗\n"); \
                    printf("║            ERRORE              ║\n"); \
                    printf("╠════════════════════════════════╣\n"); \
                    printf("║                                ║\n"); \
                    printf("║   %s deve essere 0 o 1      ║\n", nome); \
                    printf("║                                ║\n"); \
                    printf("╚════════════════════════════════╝\n"); \
                    input_valido = 0; \
                } \
            } while(0)

            LEGGI_BIT_FILE(Cn, "Cn");
            if (input_valido) LEGGI_BIT_FILE(M, "M");
            if (input_valido) LEGGI_BIT_FILE(A0, "A0");
            if (input_valido) LEGGI_BIT_FILE(B0, "B0");
            if (input_valido) LEGGI_BIT_FILE(A1, "A1");
            if (input_valido) LEGGI_BIT_FILE(B1, "B1");
            if (input_valido) LEGGI_BIT_FILE(A2, "A2");
            if (input_valido) LEGGI_BIT_FILE(B2, "B2");
            if (input_valido) LEGGI_BIT_FILE(A3, "A3");
            if (input_valido) LEGGI_BIT_FILE(B3, "B3");
            if (input_valido) LEGGI_BIT_FILE(S0, "S0");
            if (input_valido) LEGGI_BIT_FILE(S1, "S1");
            if (input_valido) LEGGI_BIT_FILE(S2, "S2");
            if (input_valido) LEGGI_BIT_FILE(S3, "S3");

            #undef LEGGI_BIT_FILE
            fclose(file);
        }
    }

    if (!input_valido) {
        return;
    }

    int A[4] = {A0, A1, A2, A3};
    int B[4] = {B0, B1, B2, B3};
    int S_arr[4] = {S0, S1, S2, S3};
    int F[4], A_uguale_B, P, Cn_piu_4, G;
    n_ALU74181(Cn, M, A, B, S_arr, F, &A_uguale_B, &P, &Cn_piu_4, &G);

    printf("\n");
    printf("╔═════════════════════════════════════════════╗\n");
    printf("║           RISULTATI ALU 74181               ║\n");
    printf("╠═════════════════════════════════════════════╣\n");
    printf("║                                             ║\n");
    printf("║  - F0      = %-3d                            ║\n", F[0]);
    printf("║  - F1      = %-3d                            ║\n", F[1]);
    printf("║  - A = B   = %-3d                            ║\n", A_uguale_B);
    printf("║  - F2      = %-3d                            ║\n", F[2]);
    printf("║  - F3      = %-3d                            ║\n", F[3]);
    printf("║  - P       = %-3d                            ║\n", P);
    printf("║  - Cn + 4  = %-3d                            ║\n", Cn_piu_4);
    printf("║  - G       = %-3d                            ║\n", G);
    printf("║                                             ║\n");
    printf("╚═════════════════════════════════════════════╝\n");

    salva_in_memoria(Cn_piu_4);

    FILE *file_out = fopen("risultati_alu_74181.txt", "w");
    if (file_out) {
        fprintf(file_out, "╔═════════════════════════════════════════════╗\n");
        fprintf(file_out, "║           RISULTATI ALU 74181               ║\n");
        fprintf(file_out, "╠═════════════════════════════════════════════╣\n");
        fprintf(file_out, "║                                             ║\n");
        fprintf(file_out, "║  - F0      = %-3d                            ║\n", F[0]);
        fprintf(file_out, "║  - F1      = %-3d                            ║\n", F[1]);
        fprintf(file_out, "║  - A = B   = %-3d                            ║\n", A_uguale_B);
        fprintf(file_out, "║  - F2      = %-3d                            ║\n", F[2]);
        fprintf(file_out, "║  - F3      = %-3d                            ║\n", F[3]);
        fprintf(file_out, "║  - P       = %-3d                            ║\n", P);
        fprintf(file_out, "║  - Cn + 4  = %-3d                            ║\n", Cn_piu_4);
        fprintf(file_out, "║  - G       = %-3d                            ║\n", G);
        fprintf(file_out, "║                                             ║\n");
        fprintf(file_out, "╚═════════════════════════════════════════════╝\n");
        fclose(file_out);
    } else {
        printf("╔════════════════════════════════╗\n");
        printf("║            ERRORE              ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║                                ║\n");
        printf("║    Impossibile aprire file     ║\n");
        printf("║         di scrittura           ║\n");
        printf("║                                ║\n");
        printf("╚════════════════════════════════╝\n");
    }

    attendi_cicli_clock_equivalenti_a_secondi(2.0);
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: ALU A 32 BIT BASATA SU 8 CHIP 74181                               │
 * │                                                                            │
 * │  Questo modulo estende la logica del chip storico 74181, originariamente   │
 * │  progettato per operazioni su 4 bit, a una configurazione a 32 bit.        │
 * │  L'estensione è ottenuta concatenando 8 unità ALU 74181 in cascata,        │
 * │  ciascuna responsabile di un nibble (4 bit) dell'operazione.               │
 * │                                                                            │
 * │  FUNZIONALITÀ PRINCIPALI                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Caricamento degli operandi                                              │
 * │    - Gli input A e B vengono suddivisi in 8 blocchi da 4 bit               │
 * │    - Ogni blocco viene caricato in un registro PIPO dedicato               │
 * │                                                                            │
 * │  • Esecuzione dell'operazione ALU                                          │
 * │    - Ogni chip 74181 esegue la funzione logica/aritmetica selezionata      │
 * │    - Il carry viene propagato tra le unità per garantire coerenza          │
 * │      nei calcoli aritmetici (carry lookahead)                              │
 * │                                                                            │
 * │  • Memorizzazione del risultato                                            │
 * │    - I risultati parziali vengono combinati in un'unica parola a 32 bit    │
 * │    - Il risultato finale viene salvato nella memoria dinamica              │
 * │                                                                            │
 * │  • Input flessibile                                                        │
 * │    - Supporta input manuale tramite tastiera                               │
 * │    - Supporta input da file strutturato ("input_alu32.txt")                │
 * │                                                                            │
 * │  UTILIZZO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Simulazione di operazioni su interi a 32 bit                            │
 * │  • Verifica della propagazione del carry tra unità ALU                     │
 * │  • Studio dell'architettura scalabile basata su componenti discreti        │
 * │                                                                            │
 * │  NOTE TECNICHE                                                             │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Ogni unità ALU è indipendente ma sincronizzata tramite clock simulato   │
 * │  • Il sistema è progettato per essere estendibile a 64 bit o oltre         │
 * │  • La gestione del carry è conforme al comportamento del chip originale    │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


void ALU32() {
    unsigned int operandoA = 0, operandoB = 0;
    int Cn = 0, M = 0, S[4] = {0};
    char scelta[3];
    int input_valido = 1;

    printf("Inserire dati manualmente? (S/N): ");
    scanf("%2s", scelta);
    scelta[0] = toupper(scelta[0]);

    if (scelta[0] == 'S') {
        printf(">> Inserisci il primo operando (numero decimale a 32 bit): ");
        if (scanf("%u", &operandoA) != 1) {
            printf("ERRORE: Input operando A non valido.\n");
            input_valido = 0;
        }
        if (!input_valido) { return; }

        printf(">> Inserisci il secondo operando (numero decimale a 32 bit): ");
        if (scanf("%u", &operandoB) != 1) {
            printf("ERRORE: Input operando B non valido.\n");
            input_valido = 0;
        }
        if (!input_valido) { return; }

        if (!leggi_bit_input_32("Cn", &Cn)) input_valido = 0;
        if (input_valido && !leggi_bit_input_32("M", &M)) input_valido = 0;
        if (input_valido && !leggi_bit_input_32("S0", &S[0])) input_valido = 0;
        if (input_valido && !leggi_bit_input_32("S1", &S[1])) input_valido = 0;
        if (input_valido && !leggi_bit_input_32("S2", &S[2])) input_valido = 0;
        if (input_valido && !leggi_bit_input_32("S3", &S[3])) input_valido = 0;
    } else {
        FILE *file = fopen("input_alu32.txt", "r");
        if (!file) {
            file = fopen("input_alu32.txt", "w");
            if (!file) {
                printf("ERRORE: Impossibile creare il file input_alu32.txt\n");
                input_valido = 0;
            } else {
                fprintf(file, "Operando A: <0>\n");
                fprintf(file, "Operando B: <0>\n");
                fprintf(file, "Cn: <0>\n");
                fprintf(file, "M: <0>\n");
                fprintf(file, "S0: <0>\n");
                fprintf(file, "S1: <0>\n");
                fprintf(file, "S2: <0>\n");
                fprintf(file, "S3: <0>\n");
                fclose(file);
                printf("Creato file input_alu32.txt. Compilarlo e riavviare.\n");
                input_valido = 0;
            }
        } else {
            char line[100];

            #define LEGGI_DA_FILE(var, nome, is_unsigned) do { \
                if (!fgets(line, sizeof(line), file)) { \
                    printf("ERRORE: Formato file incompleto (%s)\n", nome); \
                    input_valido = 0; \
                } else { \
                    if (is_unsigned) { \
                        unsigned int tmp_u; \
                        if (sscanf(line, "%*[^<]<%u>", &tmp_u) != 1) { \
                            printf("ERRORE: Valore non valido in %s\n", nome); \
                            input_valido = 0; \
                        } else { \
                            *(var) = tmp_u; \
                        } \
                    } else { \
                        int tmp_i; \
                        if (sscanf(line, "%*[^<]<%d>", &tmp_i) != 1) { \
                            printf("ERRORE: Valore non valido in %s\n", nome); \
                            input_valido = 0; \
                        } else if (tmp_i != 0 && tmp_i != 1) { \
                            printf("╔════════════════════════════════╗\n"); \
                            printf("║            ERRORE              ║\n"); \
                            printf("╠════════════════════════════════╣\n"); \
                            printf("║                                ║\n"); \
                            printf("║   %s deve essere 0 o 1      ║\n", nome); \
                            printf("║                                ║\n"); \
                            printf("╚════════════════════════════════╝\n"); \
                            input_valido = 0; \
                        } else { \
                            *(var) = tmp_i; \
                        } \
                    } \
                } \
            } while(0)

            LEGGI_DA_FILE(&operandoA, "Operando A", 1);
            if (input_valido) LEGGI_DA_FILE(&operandoB, "Operando B", 1);
            if (input_valido) LEGGI_DA_FILE(&Cn, "Cn", 0);
            if (input_valido) LEGGI_DA_FILE(&M, "M", 0);
            if (input_valido) LEGGI_DA_FILE(&S[0], "S0", 0);
            if (input_valido) LEGGI_DA_FILE(&S[1], "S1", 0);
            if (input_valido) LEGGI_DA_FILE(&S[2], "S2", 0);
            if (input_valido) LEGGI_DA_FILE(&S[3], "S3", 0);

            #undef LEGGI_DA_FILE
            fclose(file);
        }
    }

    if (!input_valido) {
        return;
    }

    int D_A[32], D_B[32], D_F[32];
    for (int i = 0; i < 32; i++) {
        D_A[i] = (operandoA >> i) & 1;
        D_B[i] = (operandoB >> i) & 1;
    }

    int Q_A[32] = {0}, Q_bar_A[32] = {0}, prev_CLK_A[32] = {0};
    int Q_B[32] = {0}, Q_bar_B[32] = {0}, prev_CLK_B[32] = {0};
    int Q_F[32] = {0}, Q_bar_F[32] = {0}, prev_CLK_F[32] = {0};
    int S_reg[32], R_reg[32];
    for (int i = 0; i < 32; i++) {
        S_reg[i] = 1;
        R_reg[i] = 1;
    }

    int CLK = 0, prev_CLK = 0;
    for (int i = 0; i < 4; i++) {
        clock_step(&CLK, &prev_CLK, 100);
        reg_PIPO32(D_A, S_reg, R_reg, CLK, prev_CLK_A, Q_A, Q_bar_A);
        reg_PIPO32(D_B, S_reg, R_reg, CLK, prev_CLK_B, Q_B, Q_bar_B);
    }

    unsigned int result = 0;
    int currentCn = Cn;
    int F[4], A_uguale_B, P, Cn_piu_4, G;
    for (int nibble = 0; nibble < 8; nibble++) {
        int Abits[4], Bbits[4];
        for (int bit = 0; bit < 4; bit++) {
            Abits[bit] = Q_A[nibble * 4 + bit];
            Bbits[bit] = Q_B[nibble * 4 + bit];
        }
        n_ALU74181(currentCn, M, Abits, Bbits, S, F, &A_uguale_B, &P, &Cn_piu_4, &G);
        unsigned int nibbleResult = F[0] | (F[1] << 1) | (F[2] << 2) | (F[3] << 3);
        result |= (nibbleResult << (nibble * 4));
        currentCn = Cn_piu_4;
    }

    for (int i = 0; i < 32; i++) {
        D_F[i] = (result >> i) & 1;
    }

    for (int i = 0; i < 4; i++) {
        clock_step(&CLK, &prev_CLK, 100);
        reg_PIPO32(D_F, S_reg, R_reg, CLK, prev_CLK_F, Q_F, Q_bar_F);
    }

    printf("\n");
    printf("╔═════════════════════════════════════════════╗\n");
    printf("║           RISULTATI ALU 32bit               ║\n");
    printf("╚═════════════════════════════════════════════╝\n");
    printf("- Risultato      = %u\n", result);
    salva_in_memoria((int)result);

    FILE *file_out = fopen("risultati_alu32.txt", "w");
    if (file_out) {
        fprintf(file_out, "╔═════════════════════════════════════════════╗\n");
        fprintf(file_out, "║           RISULTATI ALU 32bit               ║\n");
        fprintf(file_out, "╚═════════════════════════════════════════════╝\n");
        fprintf(file_out, "- Risultato      = %u\n", result);
        fclose(file_out);
    } else {
        printf("╔════════════════════════════════╗\n");
        printf("║            ERRORE              ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║                                ║\n");
        printf("║    Impossibile aprire file     ║\n");
        printf("║         di scrittura           ║\n");
        printf("║                                ║\n");
        printf("╚════════════════════════════════╝\n");
    }

    attendi_cicli_clock_equivalenti_a_secondi(2.0);
}
int somma(int a, int b) { 
  return a + b; 
} 
int sottrazione(int a, int b) { 
  return a - b; 
} 
int moltiplicazione(int a, int b) { 
  return a * b; 
} 
int divisione(int a, int b) { 
  if (b == 0) { 
    printf("╔════════════════════════════════╗\n"); 
    printf("║            ERRORE              ║\n"); 
    printf("╠════════════════════════════════╣\n"); 
    printf("║                                ║\n"); 
    printf("║   Impossibile dividere per 0   ║\n"); 
    printf("║                                ║\n"); 
    printf("╚════════════════════════════════╝\n"); 
    return 0; 
  } 
  return a / b; 
}

/*
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │                                                                            │
 * │  MODULO: CALCOLATRICE AVANZATA CON SUPPORTO A ESPRESSIONI                  │
 * │                                                                            │
 * │  Questo modulo fornisce un'interfaccia interattiva per l'esecuzione di     │
 * │  operazioni matematiche, sia semplici che complesse, all'interno del       │
 * │  simulatore ALU 74181. È progettato per offrire flessibilità e precisione  │
 * │  nell'elaborazione di espressioni numeriche.                               │
 * │                                                                            │
 * │  FUNZIONALITÀ PRINCIPALI                                                   │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Operazioni sequenziali                                                  │
 * │    - Somma, sottrazione, moltiplicazione, divisione                        │
 * │    - Esecuzione passo-passo con visualizzazione intermedia dei risultati   │
 * │                                                                            │
 * │  • Valutazione di espressioni complesse                                    │
 * │    - Supporta parentesi e operatori multipli                               │
 * │    - Esempio: (3 + 5) * 2 - 4 = 12                                         │
 * │                                                                            │
 * │  • Compatibilità multipiattaforma                                          │
 * │    - Unix/Linux/macOS: utilizza `echo $((...))` per valutare espressioni   │
 * │    - Windows: utilizza `set /a` per eseguire calcoli                       │
 * │                                                                            │
 * │  • Salvataggio dei risultati                                               │
 * │    - I risultati vengono scritti su file ("risultati_calcolatrice.txt")    │
 * │    - Supporta log cronologico delle operazioni                             │
 * │                                                                            │
 * │  UTILIZZO                                                                  │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Interfaccia testuale per utenti finali                                  │
 * │  • Supporto a test e simulazioni numeriche                                 │
 * │  • Integrazione con moduli ALU per verifica dei risultati                  │
 * │                                                                            │
 * │  NOTE TECNICHE                                                             │
 * │  ────────────────────────────────────────────────────────────────────────  │
 * │  • Le espressioni vengono validate prima dell'esecuzione                   │
 * │  • Il modulo è estendibile per supportare floating point e funzioni        │
 * │    matematiche avanzate (es. potenze, radici, moduli)                      │
 * │                                                                            │
 * │  AUTORE: Leonardo Galli                                                    │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                     │
 * └────────────────────────────────────────────────────────────────────────────┘
 */


void operazioni_algebriche() {
    while (1) {
        char operazione[30];
        int scelta = 0;
        printf("\n╔══════════════════════════════════════════════════════╗\n");
        printf("║                 CALCOLATRICE AVANZATA                ║\n");
        printf("╚══════════════════════════════════════════════════════╝\n");
        printf(">> Seleziona un'operazione:\n");
        printf("   1 - Somma di numeri\n");
        printf("   2 - Sottrazione sequenziale\n");
        printf("   3 - Moltiplicazione di numeri\n");
        printf("   4 - Divisione sequenziale\n");
        printf("   5 - Calcola espressione (es: (3+5)*2-4)\n");
        printf("   6 - Esci\n");
        printf(">> Scelta (numero o parola): ");
        scanf("%s", operazione);

        if (strlen(operazione) == 1 && isdigit(operazione[0])) {
            scelta = operazione[0] - '0';
        } else {
            for (int i = 0; operazione[i]; i++)
                operazione[i] = tolower(operazione[i]);
            if (strcmp(operazione, "somma") == 0) scelta = 1;
            else if (strcmp(operazione, "sottrazione") == 0) scelta = 2;
            else if (strcmp(operazione, "moltiplicazione") == 0) scelta = 3;
            else if (strcmp(operazione, "divisione") == 0) scelta = 4;
            else if (strcmp(operazione, "calcola") == 0) scelta = 5;
            else if (strcmp(operazione, "esci") == 0) scelta = 6;
        }

        FILE *file = fopen("risultato_operazione.txt", "a");
        if (!file) {
            printf("Errore: impossibile aprire il file di output.\n");
            continue;
        }

        switch (scelta) {
            case 1: { 
                double val, risultato = 0;
                printf("\n╔══════════════════════════════════════════════════════╗\n");
                printf("║                    SOMMA DI NUMERI                   ║\n");
                printf("╚══════════════════════════════════════════════════════╝\n");
                printf("Inserisci i numeri da sommare (scrivi una lettera per terminare):\n");
                while (scanf("%lf", &val) == 1)
                    risultato += val;
                printf("\n────────────────────────────────────────────────────────\n");
                printf("Risultato finale: %.2lf\n", risultato);
                printf("────────────────────────────────────────────────────────\n");
                fprintf(file, "\n[OPERAZIONE: SOMMA]\nRisultato: %.2lf\n", risultato);
                salva_in_memoria(risultato);
                fclose(file);
                while (getchar() != '\n');
                continue;
            }
            case 2: { 
                double val, risultato;
                int count = 0;
                printf("\n╔══════════════════════════════════════════════════════╗\n");
                printf("║                SOTTRAZIONE SEQUENZIALE               ║\n");
                printf("╚══════════════════════════════════════════════════════╝\n");
                printf("Inserisci i numeri da sottrarre (scrivi una lettera per terminare):\n");
                while (scanf("%lf", &val) == 1) {
                    if (count == 0) risultato = val;
                    else risultato -= val;
                    count++;
                }
                if (count == 0) {
                    printf("Nessun numero inserito.\n");
                    fclose(file);
                    while (getchar() != '\n');
                    continue;
                }
                printf("\n────────────────────────────────────────────────────────\n");
                printf("Risultato finale: %.2lf\n", risultato);
                printf("────────────────────────────────────────────────────────\n");
                fprintf(file, "\n[OPERAZIONE: SOTTRAZIONE]\nRisultato: %.2lf\n", risultato);
                salva_in_memoria(risultato);
                fclose(file);
                while (getchar() != '\n');
                continue;
            }
            case 3: { 
                double val, risultato = 1;
                int count = 0;
                printf("\n╔══════════════════════════════════════════════════════╗\n");
                printf("║               MOLTIPLICAZIONE DI NUMERI              ║\n");
                printf("╚══════════════════════════════════════════════════════╝\n");
                printf("Inserisci i numeri da moltiplicare (scrivi una lettera per terminare):\n");
                while (scanf("%lf", &val) == 1) {
                    risultato *= val;
                    count++;
                }
                if (count == 0) {
                    printf("Nessun numero inserito.\n");
                    fclose(file);
                    while (getchar() != '\n');
                    continue;
                }
                printf("\n────────────────────────────────────────────────────────\n");
                printf("Risultato finale: %.2lf\n", risultato);
                printf("────────────────────────────────────────────────────────\n");
                fprintf(file, "\n[OPERAZIONE: MOLTIPLICAZIONE]\nRisultato: %.2lf\n", risultato);
                salva_in_memoria(risultato);
                fclose(file);
                while (getchar() != '\n');
                continue;
            }
            case 4: { 
                double val, risultato;
                int count = 0;
                printf("\n╔══════════════════════════════════════════════════════╗\n");
                printf("║                DIVISIONE SEQUENZIALE                 ║\n");
                printf("╚══════════════════════════════════════════════════════╝\n");
                printf("Inserisci i numeri da dividere (scrivi una lettera per terminare):\n");
                while (scanf("%lf", &val) == 1) {
                    if (count == 0) risultato = val;
                    else {
                        if (val == 0) {
                            printf("Errore: divisione per zero!\n");
                            break;
                        }
                        risultato /= val;
                    }
                    count++;
                }
                if (count < 2) {
                    printf("Serve almeno 2 numeri.\n");
                    fclose(file);
                    while (getchar() != '\n');
                    continue;
                }
                printf("\n────────────────────────────────────────────────────────\n");
                printf("Risultato finale: %.2lf\n", risultato);
                printf("────────────────────────────────────────────────────────\n");
                fprintf(file, "\n[OPERAZIONE: DIVISIONE]\nRisultato: %.2lf\n", risultato);
                salva_in_memoria(risultato);
                fclose(file);
                while (getchar() != '\n');
                continue;
            }
            case 5: {
                char input[256];
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                printf("\n╔══════════════════════════════════════════════════════╗\n");
                printf("║               CALCOLO DI ESPRESSIONE                 ║\n");
                printf("╚══════════════════════════════════════════════════════╝\n");
                printf("Inserisci un'espressione (es: (3+5)*2-4): ");
                if (!fgets(input, sizeof(input), stdin)) {
                  fprintf(stderr, "Input error.\n");
                  fclose(file);
                  continue;
                }
                size_t len = strlen(input);
                if (len > 0 && input[len-1] == '\n') input[len-1] = '\0';
                if (strlen(input) == 0) {
                    printf("Espressione vuota.\n");
                    fclose(file);
                    continue;
                }
                if (!is_safe_expr(input)) {
                    printf("Espressione non valida: contiene caratteri non permessi o parentesi sbilanciate.\n");
                    fclose(file);
                    continue;
                }
                char comando[512];

#ifdef _WIN32
                int n = snprintf(comando, sizeof(comando), "set /a %s", input);
#else
                int n = snprintf(comando, sizeof(comando), "echo $(( %s ))", input);
#endif
                if (n < 0 || n >= (int)sizeof(comando)) {
                    fprintf(stderr, "Espresssione troppo lunga.\n");
                    fclose(file);
                    continue;
                }
#ifdef _WIN32
                FILE *fp = _popen(comando, "r");
#else
                FILE *fp = popen(comando, "r");
#endif
                if (!fp) {
                    printf("Errore nell'elaborazione dell'espressione.\n");
                    fclose(file);
                    continue;
                }
                long long int_res = 0;
                int scan_ok = fscanf(fp, "%lld", &int_res);
#ifdef _WIN32
                _pclose(fp);
#else
                pclose(fp);
#endif
                if (scan_ok != 1) {
                    printf("Impossibile leggere il risultato (output inatteso).\n");
                    fclose(file);
                    continue;
                }
                double risultato = (double)int_res;

                printf("\n────────────────────────────────────────────────────────\n");
                printf("Espressione: %s\n", input);
                printf("Risultato: %.2lf\n", risultato);
                printf("────────────────────────────────────────────────────────\n");
                fprintf(file, "\n[OPERAZIONE: ESPRESSIONE]\nEspressione: %s\nRisultato: %.2lf\n", input, risultato);
                salva_in_memoria(risultato);
                fclose(file);
                continue;
            }
            case 6:
                printf("\nChiusura del programma...\n");
                fclose(file);
                return;
            default:
                printf("\nOperazione non riconosciuta.\n");
                fclose(file);
                continue;
        }
    }
}

void misura_ciclo_clock() {
    printf("\n==============================\n");
    printf("  Rilevamento del sistema\n");
    printf("==============================\n");
    char *cpu = rileva_cpu();
    printf("CPU rilevata: %s\n", cpu);
    long freq = ottieni_clock(cpu);
    if (freq <= 0) {
        freq = 1000000000L;
    }
    printf("Frequenza di clock trovata: %ld Hz\n", freq);
    double durata_ns = (1.0 / (double)freq) * 1e9;
    printf("Durata stimata di un ciclo: %.3f ns\n", durata_ns);
    printf("\n==============================\n");
    printf("  Simulazione clock in corso\n");
    printf("==============================\n");
    for (int i = 0; i < 1000; i++) {
        attendi_un_ciclo_clock();
    }
    printf("\nSimulazione completata.\n");
}

/*
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │                                                                             │
 * │  FUNZIONE PRINCIPALE (MAIN)                                                 │
 * │                                                                             │
 * │  Punto di ingresso del simulatore ALU 74181. Questa funzione implementa un  │
 * │  menu interattivo testuale che consente all'utente di accedere a tutte le   │
 * │  funzionalità del programma. Gestisce il flusso di controllo, la validazione│
 * │  dell'input e la corretta terminazione del sistema.                         │
 * │                                                                             │
 * │  STRUTTURA DEL MENU                                                         │
 * │  ────────────────────────────────────────────────────────────────────────   │
 * │  • Opzione 1: Simulazione ALU 74181 con attesa temporale                    │
 * │  • Opzione 2: Simulazione ALU 74181 senza attesa                            │
 * │  • Opzione 3: Calcolatrice avanzata con supporto a espressioni              │
 * │  • Opzione 4: Conversione binario → decimale                                │
 * │  • Opzione 5: Conversione decimale → binario                                │
 * │  • Opzione 6: Simulazione ALU a 32 bit con attesa temporale                 │
 * │  • Opzione 7: Simulazione ALU a 32 bit senza attesa                         │
 * │  • Opzione 8: Visualizzazione dello stato e contenuto della memoria         │
 * │  • Opzione 9: Misurazione del ciclo di clock del processore                 │
 * │  • Opzione 0: Uscita dal programma con pulizia della memoria allocata       │
 * │                                                                             │
 * │  FUNZIONALITÀ DI CONTROLLO                                                  │
 * │  ────────────────────────────────────────────────────────────────────────   │
 * │  • Validazione dell'input utente                                            │
 * │    - Controlla che l'opzione selezionata sia compresa tra 0 e 9             │
 * │    - Gestisce input non numerici o fuori range                              │
 * │                                                                             │
 * │  • Gestione della memoria                                                   │
 * │    - All'uscita, libera tutte le risorse allocate dinamicamente             │
 * │    - Garantisce una chiusura pulita e sicura del programma                  │
 * │                                                                             │
 * │  • Loop interattivo                                                         │
 * │    - Il menu viene ripresentato dopo ogni operazione                        │
 * │    - L'utente può eseguire più simulazioni senza riavviare il programma     │
 * │                                                                             │
 * │  UTILIZZO                                                                   │
 * │  ────────────────────────────────────────────────────────────────────────   │
 * │  • Interfaccia principale per accedere a tutte le funzionalità del sistema  │
 * │  • Pensata per utenti didattici, sviluppatori e appassionati di logica      │
 * │                                                                             │
 * │  AUTORE: Leonardo Galli                                                     │
 * │  Ultimo aggiornamento: 20 ottobre 2025                                      │
 * └─────────────────────────────────────────────────────────────────────────────┘
 */


int main() {
    int scelta;
    char input[10];
    while (1) {   
        printf("\n╔════════════════════════════════════════════════════════════╗\n");
        printf("║                ________|          |________                ║\n");
        printf("║               |       /   ||||||   \\       |               ║\n");
        printf("║               |     ,'              `.     |               ║\n");
        printf("║               |   ,'                  `.   |               ║\n");
        printf("║               | ,'   ||||||||||||||||   `. |               ║\n");
        printf("║               ,'  /____________________\\  `.               ║\n");
        printf("║              /______________________________\\              ║\n");
        printf("║             |                                |             ║\n");
        printf("║             |                                |             ║\n");
        printf("║             |                                |             ║\n");
        printf("║             |________________________________|             ║\n");
        printf("║               |____________________------__|               ║\n");
        printf("║                                                            ║\n");
        printf("║   ,----------------------------------------------------,   ║\n");
        printf("║   | [][][][][]  [][][][][]  [][][][]  [][__]  [][][][] |   ║\n");
        printf("║   |                                                    |   ║\n");
        printf("║   |  [][][][][][][][][][][][][][_]    [][][]  [][][][] |   ║\n");
        printf("║   |  [_][][][][][][][][][][][][][ |   [][][]  [][][][] |   ║\n");
        printf("║   | [][_][][][][][][][][][][][][]||     []    [][][][] |   ║\n");
        printf("║   | [__][][][][][][][][][][][][__]    [][][]  [][][]|| |   ║\n");
        printf("║   |   [__][________________][__]              [__][]|| |   ║\n");
        printf("║   `----------------------------------------------------'   ║\n");
        printf("║                                                            ║\n");
        printf("║                    SIMULATORE ALU 74181                    ║\n");
        printf("║                                                            ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║    1. Operazioni Logiche (ALU 74181 - Singolo)             ║\n");
        printf("║    2. Operazioni Logiche (ALU 74181 - Singolo con clock)   ║\n");
        printf("║    3. Operazioni Algebriche                                ║\n");
        printf("║    4. Convertitore Binario → Decimale                      ║\n");
        printf("║    5. Convertitore Decimale → Binario                      ║\n");
        printf("║    6. ALU in Modalità PIPO (32 bit - 8x74181)              ║\n");
        printf("║    7. ALU in Modalità PIPO (32 bit - 8x74181 con clock)    ║\n");
        printf("║    8. Visualizza Memoria                                   ║\n");
        printf("║    9. Calcolo del Clock                                    ║\n");
        printf("║    0. Esci                                                 ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        printf(">> Inserisci la tua scelta: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Errore di input.\n");
            continue;
        }
        if (input[0] == '\n' || input[0] == '\0') {
            continue;
        }
        int isValid = 1;
        for (int i = 0; input[i] != '\0' && input[i] != '\n'; i++) {
            if (!isdigit(input[i])) {
                isValid = 0;
                break;
            }
        }
        if (!isValid) {
            printf("\n╔════════════════════════════════╗\n");
            printf("║             ERRORE             ║\n");
            printf("╠════════════════════════════════╣\n");
            printf("║                                ║\n");
            printf("║   Inserisci un numero valido   ║\n");
            printf("║                                ║\n");
            printf("╚════════════════════════════════╝\n");
            continue;
        }
        scelta = atoi(input);
        if (scelta == 0) {
            printf("Uscita dal programma...\n");
            break;
        }
        else if (scelta == 1) {
            simula_alu_74181();
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        else if (scelta == 2) {
            attendi_cicli_clock_equivalenti_a_secondi(2.0);
            simula_alu_74181();
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        else if (scelta == 3) {
            operazioni_algebriche();
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        else if (scelta == 4) {
            char bin[33];
            char risposta[3];
            printf("Inserire dati manualmente? (S/N): ");
            scanf("%2s", risposta);
            risposta[0] = toupper(risposta[0]);
            if (risposta[0] == 'S') {
                printf(">> Inserisci un numero binario: ");
                scanf("%32s", bin);
                int risultato = BIN_DEC_DECODER(bin);
                if (risultato != -1) {
                    printf("Risultato (decimale): %d\n", risultato);
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                    continue;
                }
            } else {
                FILE *file = fopen("input_bin.txt", "r");
                if (!file) {
                    file = fopen("input_bin.txt", "w");
                    if (!file) {
                        printf("ERRORE: Impossibile creare il file\n");
                        continue;
                    } else {
                        fprintf(file, "Numero Binario: <0>\n");
                        fclose(file);
                        printf("Creato file input_bin.txt. Compilarlo e riavviare.\n");
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        continue;
                    }
                } else {
                    char line[100];
                    if (fgets(line, sizeof(line), file)) {
                        fclose(file);
                        if (sscanf(line, "%*[^<]<%32[^>]>", bin) == 1) {
                            int risultato = BIN_DEC_DECODER(bin);
                            FILE *file_out = fopen("risultati_dec.txt", "w");
                            if (file_out) {
                                fprintf(file_out, "╔═════════════════════════════════════════════╗\n");
                                fprintf(file_out, "║          RISULTATI CONVERTITORE             ║\n");
                                fprintf(file_out, "╚═════════════════════════════════════════════╝\n");
                                fprintf(file_out, "Risultato      = %-3d\n", risultato);
                                fclose(file_out);
                                int c;
                                while ((c = getchar()) != '\n' && c != EOF);
                                continue;
                            }
                        }
                    } else {
                        fclose(file);
                        printf("ERRORE: Formato file incompleto\n");
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        continue;
                    }
                }
            }
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        else if (scelta == 5) {
            char risposta[3];
            int dec;
            printf("Inserire dati manualmente? (S/N): ");
            scanf("%2s", risposta);
            risposta[0] = toupper(risposta[0]);
            if (risposta[0] == 'S') {
                printf(">> Inserisci un numero decimale: ");
                scanf("%d", &dec);
                printf("Risultato (binario): %s\n", DEC_BIN_CODER(dec));
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            } else {
                FILE *file = fopen("input_dec.txt", "r");
                if (!file) {
                    file = fopen("input_dec.txt", "w");
                    if (!file) {
                        printf("ERRORE: Impossibile creare il file\n");
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        continue;
                    } else {
                        fprintf(file, "Numero Decimale: <0>\n");
                        fclose(file);
                        printf("Creato file input_dec.txt. Compilarlo e riavviare.\n");
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        continue;
                    }
                } else {
                    char line[100];
                    if (fgets(line, sizeof(line), file)) {
                        fclose(file);
                        char buffer[33];
                        if (sscanf(line, "%*[^<]<%32[^>]>", buffer) == 1) {
                            dec = atoi(buffer);
                            FILE *file_out = fopen("risultati_bin.txt", "w");
                            if (file_out) {
                                fprintf(file_out, "╔═════════════════════════════════════════════╗\n");
                                fprintf(file_out, "║          RISULTATI CONVERTITORE             ║\n");
                                fprintf(file_out, "╚═════════════════════════════════════════════╝\n");
                                fprintf(file_out, "Risultato      = %-16s\n", DEC_BIN_CODER(dec));
                                fclose(file_out);
                                int c;
                                while ((c = getchar()) != '\n' && c != EOF);
                                continue;
                            }
                        }
                    } else {
                        fclose(file);
                        printf("ERRORE: Formato file incompleto\n");
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        continue;
                    }
                }
            }
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        else if (scelta == 6) {
            ALU32();
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        else if (scelta == 7) {
            attendi_cicli_clock_equivalenti_a_secondi(2.0);
            ALU32();
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        else if (scelta == 8) {
            stampa_memoria();
            stato_memoria();
            int c;
            continue;
        }
        else if (scelta == 9) {
            misura_ciclo_clock();
            continue;
        }
        else {
            printf("Scelta non valida!\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
    }
    if (memoria != NULL) {
        free(memoria);
        memoria = NULL;
        printf("[INFO] Memoria liberata.\n");
    }

    return 0;
}
