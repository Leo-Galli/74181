#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
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

typedef struct {
    const char *nome;
    long clock_hz;
} Processore;
static int leggi_bit_input_74181(const char* nome, int* var);
static int leggi_bit_input_32(const char* nome, int* var);
static const Processore db_processori[] = {
    {"Intel(R) Core(TM) i3-2100", 3100000000L},
    {"Intel(R) Core(TM) i3-3220", 3300000000L},
    {"Intel(R) Core(TM) i3-4130", 3400000000L},
    {"Intel(R) Core(TM) i3-6100", 3700000000L},
    {"Intel(R) Core(TM) i3-7100", 3900000000L},
    {"Intel(R) Core(TM) i3-8100", 3600000000L},
    {"Intel(R) Core(TM) i3-9100", 3600000000L},
    {"Intel(R) Core(TM) i3-10100", 3600000000L},
    {"Intel(R) Core(TM) i3-11100", 3700000000L},
    {"Intel(R) Core(TM) i3-12100", 3300000000L},
    {"Intel(R) Core(TM) i3-13100", 3400000000L},
    {"Intel(R) Core(TM) i5-2400", 3100000000L},
    {"Intel(R) Core(TM) i5-2500K", 3300000000L},
    {"Intel(R) Core(TM) i5-3570K", 3400000000L},
    {"Intel(R) Core(TM) i5-4460", 3200000000L},
    {"Intel(R) Core(TM) i5-6500", 3200000000L},
    {"Intel(R) Core(TM) i5-7400", 3000000000L},
    {"Intel(R) Core(TM) i5-8400", 2800000000L},
    {"Intel(R) Core(TM) i5-9400F", 2900000000L},
    {"Intel(R) Core(TM) i5-9600K", 3700000000L},
    {"Intel(R) Core(TM) i5-10400", 2900000000L},
    {"Intel(R) Core(TM) i5-10600K", 4100000000L},
    {"Intel(R) Core(TM) i5-11400", 2600000000L},
    {"Intel(R) Core(TM) i5-12400", 2500000000L},
    {"Intel(R) Core(TM) i5-12600K", 3700000000L},
    {"Intel(R) Core(TM) i5-13400", 2500000000L},
    {"Intel(R) Core(TM) i5-13600K", 3500000000L},
    {"Intel(R) Core(TM) i7-2600K", 3400000000L},
    {"Intel(R) Core(TM) i7-3770K", 3500000000L},
    {"Intel(R) Core(TM) i7-4770K", 3500000000L},
    {"Intel(R) Core(TM) i7-6700K", 4000000000L},
    {"Intel(R) Core(TM) i7-7700K", 4200000000L},
    {"Intel(R) Core(TM) i7-8700K", 3700000000L},
    {"Intel(R) Core(TM) i7-9700K", 3600000000L},
    {"Intel(R) Core(TM) i7-10700K", 3800000000L},
    {"Intel(R) Core(TM) i7-11700K", 3600000000L},
    {"Intel(R) Core(TM) i7-12700K", 3600000000L},
    {"Intel(R) Core(TM) i7-13700K", 3900000000L},
    {"Intel(R) Core(TM) i7-14700K", 4200000000L},
    {"Intel(R) Core(TM) i9-7900X", 3300000000L},
    {"Intel(R) Core(TM) i9-9900K", 3600000000L},
    {"Intel(R) Core(TM) i9-10900K", 3700000000L},
    {"Intel(R) Core(TM) i9-11900K", 3500000000L},
    {"Intel(R) Core(TM) i9-12900K", 3700000000L},
    {"Intel(R) Core(TM) i9-13900K", 5500000000L},
    {"Intel(R) Core(TM) i9-14900K", 6000000000L},
    {"Intel Xeon E5-2620", 2000000000L},
    {"Intel Xeon E5-2680", 2700000000L},
    {"Intel Xeon E5-2690", 2900000000L},
    {"Intel Xeon W-2145", 3700000000L},
    {"Intel Xeon W-3175X", 3500000000L},
    {"Intel Xeon Gold 6248R", 3000000000L},
    {"Intel Xeon Platinum 8380", 2300000000L},
    {"AMD FX-6300", 3500000000L},
    {"AMD FX-8350", 4000000000L},
    {"AMD Ryzen 3 1200", 3100000000L},
    {"AMD Ryzen 3 2200G", 3500000000L},
    {"AMD Ryzen 3 3200G", 3600000000L},
    {"AMD Ryzen 3 5300G", 4000000000L},
    {"AMD Ryzen 5 1600", 3200000000L},
    {"AMD Ryzen 5 2600", 3400000000L},
    {"AMD Ryzen 5 3600", 3600000000L},
    {"AMD Ryzen 5 5600", 3500000000L},
    {"AMD Ryzen 5 5600X", 3700000000L},
    {"AMD Ryzen 5 7600", 3800000000L},
    {"AMD Ryzen 7 1700", 3000000000L},
    {"AMD Ryzen 7 2700X", 3700000000L},
    {"AMD Ryzen 7 3700X", 3600000000L},
    {"AMD Ryzen 7 3800X", 3900000000L},
    {"AMD Ryzen 7 5800X", 3800000000L},
    {"AMD Ryzen 7 7700X", 4200000000L},
    {"AMD Ryzen 9 3900X", 3800000000L},
    {"AMD Ryzen 9 3950X", 3500000000L},
    {"AMD Ryzen 9 5900X", 3700000000L},
    {"AMD Ryzen 9 5950X", 3400000000L},
    {"AMD Ryzen 9 7900X", 4700000000L},
    {"AMD Ryzen 9 7950X", 4500000000L},
    {"AMD Ryzen Threadripper 1950X", 3400000000L},
    {"AMD Ryzen Threadripper 2970WX", 3000000000L},
    {"AMD Ryzen Threadripper 2990WX", 3100000000L},
    {"AMD Ryzen Threadripper 3990X", 2900000000L},
    {"AMD Ryzen Threadripper PRO 5995WX", 2700000000L},
    {"AMD EPYC 7601", 2200000000L},
    {"AMD EPYC 7742", 2250000000L},
    {"AMD EPYC 9654", 2400000000L},
    {"AMD EPYC 7763", 2450000000L},
    {"Apple M1", 3200000000L},
    {"Apple M1 Pro", 3220000000L},
    {"Apple M1 Max", 3300000000L},
    {"Apple M2", 3500000000L},
    {"Apple M2 Pro", 3700000000L},
    {"Apple M2 Max", 3850000000L},
    {"Apple M3", 4000000000L},
    {"Apple M3 Pro", 4100000000L},
    {"Apple M3 Max", 4200000000L},
    {"ARM Cortex-A72", 1500000000L},
    {"ARM Cortex-A76", 2400000000L},
    {"ARM Cortex-X1", 3000000000L},
    {"ARM Neoverse N1", 3000000000L},
    {"Qualcomm Snapdragon 8 Gen 2", 3200000000L},
    {"Apple A17 Pro", 3780000000L},
    {"Generic", 1000000000L}
};
#define NUM_PROCESSORI (sizeof(db_processori) / sizeof(db_processori[0]))

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
    char *cpu = rileva_cpu();
    long freq = ottieni_clock(cpu);
    long num_cicli = (long)(freq * secondi);
    if (num_cicli > 1000000000L) {
        num_cicli = 1000000000L;
    }
    for (long i = 0; i < num_cicli; i++) {
        attendi_un_ciclo_clock();
    }
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
        return; // torna al menu
    }

    // === SIMULAZIONE ===
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

    // === SIMULAZIONE ===
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
void operazioni_algebriche() { 
  int num_elementi; 
  printf(">> Quanti elementi vuoi utilizzare (2 o 3)? "); 
  scanf("%d", &num_elementi); 
  if (num_elementi == 2) { 
    int a, b; 
    printf(">> Inserisci i due numeri: "); 
    scanf("%d %d", &a, &b); 
    char operazione[20]; 
    printf(">> Scegli l'operazione (1 - Somma, 2 - Sottrazione, 3 - Moltiplicazione, 4 - Divisione): "); 
    scanf("%s", operazione); 
    int scelta = 0; 
    if (strlen(operazione) == 1 && isdigit(operazione[0])) { 
      scelta = operazione[0] - '0'; 
    } 
    else { 
      for (int i = 0; operazione[i]; i++) { 
        operazione[i] = tolower(operazione[i]); 
      } 
      if (strcmp(operazione, "somma") == 0){ 
        scelta = 1;
      } 
      else if (strcmp(operazione, "sottrazione") == 0) {
        scelta = 2;
      } 
      else if (strcmp(operazione, "moltiplicazione") == 0) {
        scelta = 3;
      } 
      else if (strcmp(operazione, "divisione") == 0) {
        scelta = 4;
      } 
    } 
    switch (scelta) { 
      case 1:
          if (strcmp(operazione, "somma") != 0) break;
          {
              int res = somma(a, b);
              printf("╔════════════════════════════════╗\n");
              printf("║         SOMMA ALGEBRICA        ║\n");
              printf("╚════════════════════════════════╝\n");
              printf("Risultato: %d\n", res);
              salva_in_memoria(res);
          }
          return;
      case 2:
          {
              int res = sottrazione(a, b);
              printf("╔════════════════════════════════╗\n");
              printf("║      SOTTRAZIONE ALGEBRICA     ║\n");
              printf("╚════════════════════════════════╝\n");
              printf("Risultato: %d\n", res);
              salva_in_memoria(res);
          }
          return;
      case 3:
          {
              int res = moltiplicazione(a, b);
              printf("╔════════════════════════════════╗\n");
              printf("║    MOLTIPLICAZIONE ALGEBRICA   ║\n");
              printf("╚════════════════════════════════╝\n");
              printf("Risultato: %d\n", res);
              salva_in_memoria(res);
          }
          return;
      case 4:
          {
              if (b == 0) {
                  printf("Errore: divisione per zero.\n");
              } else {
                  int res = divisione(a, b);
                  printf("╔════════════════════════════════╗\n");
                  printf("║        DIVISIONE ALGEBRICA     ║\n");
                  printf("╚════════════════════════════════╝\n");
                  printf("Risultato: %d\n", res);
                  salva_in_memoria(res);
              }
          }
          return;
      default:
          printf("╔════════════════════════════════╗\n");
          printf("║             ERRORE             ║\n");
          printf("╠════════════════════════════════╣\n");
          printf("║                                ║\n");
          printf("║   Operazione non riconosciuta  ║\n");
          printf("║                                ║\n");
          printf("╚════════════════════════════════╝\n");
          return;
  }
  } else if (num_elementi == 3) {
      int a, b, c;
      printf(">> Inserisci i tre numeri: ");
      scanf("%d %d %d", &a, &b, &c);
      char operazione[20];
      printf(">> Scegli l'operazione (somma o moltiplicazione): ");
      scanf("%s", operazione);
      int scelta = 0;
      if (strlen(operazione) == 1 && isdigit(operazione[0])) {
          scelta = operazione[0] - '0';
      } else {
          for (int i = 0; operazione[i]; i++) {
              operazione[i] = tolower(operazione[i]);
          }
          if (strcmp(operazione, "somma") == 0)
              scelta = 1;
          else if (strcmp(operazione, "moltiplicazione") == 0)
              scelta = 3;
      }
      switch (scelta) {
          case 1:
              {
                  int res = somma(somma(a, b), c);
                  printf("╔════════════════════════════════╗\n");
                  printf("║         SOMMA ALGEBRICA        ║\n");
                  printf("╚════════════════════════════════╝\n");
                  printf("Risultato: %d\n", res);
                  salva_in_memoria(res);
              }
              return;
          case 3:
              {
                  int res = moltiplicazione(moltiplicazione(a, b), c);
                  printf("╔════════════════════════════════╗\n");
                  printf("║    MOLTIPLICAZIONE ALGEBRICA   ║\n");
                  printf("╚════════════════════════════════╝\n");
                  printf("Risultato: %d\n", res);
                  salva_in_memoria(res);
              }
              return;
          default:
              printf("╔════════════════════════════════╗\n");
              printf("║             ERRORE             ║\n");
              printf("╠════════════════════════════════╣\n");
              printf("║                                ║\n");
              printf("║   Operazione non riconosciuta  ║\n");
              printf("║                                ║\n");
              printf("╚════════════════════════════════╝\n");
              return;
      }
  } else {
      printf("╔════════════════════════════════╗\n");
      printf("║             ERRORE             ║\n");
      printf("╠════════════════════════════════╣\n");
      printf("║                                ║\n");
      printf("║ Numero di elementi non valido. ║\n");
      printf("║      Scegli tra 2 oppure 3     ║\n");
      printf("║                                ║\n");
      printf("╚════════════════════════════════╝\n");
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
int main() {
    int scelta;
    char input[10];
    while (1) {
        printf("\n╔═════════════════════════════════════════════════════════╗\n");
        printf("║                                                         ║\n");
        printf("║                     ▄▀▀▀▄▄▄▄▄▄▄▀▀▀▄                     ║\n");
        printf("║                     █▒▒░░░░░░░░░▒▒█                     ║\n");
        printf("║                      █░░█░░░░░█░░█                      ║\n");
        printf("║                   ▄▄  █░░░▀█▀░░░█  ▄▄                   ║\n");
        printf("║                  █░░█ ▀▄░░░░░░░▄▀ █░░█                  ║\n");
        printf("║                        ALU 74181                        ║\n");
        printf("║                                                         ║\n");
        printf("║                     MENU PRINCIPALE                     ║\n");
        printf("╠═════════════════════════════════════════════════════════╣\n");
        printf("║   1. Operazioni Logiche (ALU 74181 - Singolo)           ║\n");
        printf("║   2. Operazioni Logiche (ALU 74181 - Singolo con clock) ║\n");
        printf("║   3. Operazioni Algebriche                              ║\n");
        printf("║   4. Convertitore Binario → Decimale                    ║\n");
        printf("║   5. Convertitore Decimale → Binario                    ║\n");
        printf("║   6. ALU in Modalità PIPO (32 bit - 8x74181)            ║\n");
        printf("║   7. ALU in Modalità PIPO (32 bit - 8x74181 con clock)  ║\n");
        printf("║   8. Visualizza Memoria                                 ║\n");
        printf("║   9. Calcolo del Clock                                  ║\n");
        printf("║   0. Esci                                               ║\n");
        printf("╚═════════════════════════════════════════════════════════╝\n");
        printf(">> Inserisci la tua scelta: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Errore di input.\n");
            continue;
        }
        if (sscanf(input, "%d", &scelta) != 1) {
            printf("╔════════════════════════════════╗\n");
            printf("║             ERRORE             ║\n");
            printf("╠════════════════════════════════╣\n");
            printf("║                                ║\n");
            printf("║   Inserisci un numero valido   ║\n");
            printf("║                                ║\n");
            printf("╚════════════════════════════════╝\n");
            attendi_cicli_clock_equivalenti_a_secondi(2.0);
            continue;
        }
        if (scelta == 0) {
            printf("Uscita dal programma...\n");
            break;
        }
        else if (scelta == 1) {
            simula_alu_74181();
            continue;
        }
        else if (scelta == 2) {
            attendi_cicli_clock_equivalenti_a_secondi(2.0);
            simula_alu_74181();
            continue;
        }
        else if (scelta == 3) {
            operazioni_algebriche();
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
                                continue;
                            }
                        }
                    } else {
                        fclose(file);
                        printf("ERRORE: Formato file incompleto\n");
                        continue;
                    }
                }
            }
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
                continue;
            } else {
                FILE *file = fopen("input_dec.txt", "r");
                if (!file) {
                    file = fopen("input_dec.txt", "w");
                    if (!file) {
                        printf("ERRORE: Impossibile creare il file\n");
                        continue;
                    } else {
                        fprintf(file, "Numero Decimale: <0>\n");
                        fclose(file);
                        printf("Creato file input_dec.txt. Compilarlo e riavviare.\n");
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
                                continue;
                            }
                        }
                    } else {
                        fclose(file);
                        printf("ERRORE: Formato file incompleto\n");
                        continue;
                    }
                }
            }
        }
        else if (scelta == 6) {
            ALU32();
            continue;
        }
        else if (scelta == 7) {
            attendi_cicli_clock_equivalenti_a_secondi(2.0);
            ALU32();
            continue;
        }
        else if (scelta == 8) {
            stampa_memoria();
            stato_memoria();
            continue;
        }
        else if (scelta == 9) {
            misura_ciclo_clock();
            continue;
        }
        else {
            printf("Scelta non valida!\n");
        }
        attendi_cicli_clock_equivalenti_a_secondi(2.0);
    }
    if (memoria != NULL) {
        free(memoria);
        memoria = NULL;
        printf("[INFO] Memoria liberata.\n");
    }

    return 0;
}