#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <curl/curl.h>
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
#define URL_CLOCK "https://raw.githubusercontent.com/Leo-Galli/74181/main/processors.json"
#define FILE_CACHE "clock_cache.json"

struct stringa {
    char *dati;
    size_t lunghezza;
};
void inizializza_stringa(struct stringa *s) {
    s->lunghezza = 0;
    s->dati = malloc(1);
    if (s->dati) s->dati[0] = '\0';
}
size_t scrivi_dati(void *ptr, size_t dim, size_t nmemb, struct stringa *s) {
    size_t nuova_lung = s->lunghezza + dim * nmemb;
    s->dati = realloc(s->dati, nuova_lung + 1);
    memcpy(s->dati + s->lunghezza, ptr, dim * nmemb);
    s->dati[nuova_lung] = '\0';
    s->lunghezza = nuova_lung;
    return dim * nmemb;
}
char* scarica_json() {
    CURL *curl;
    CURLcode res;
    struct stringa s;
    inizializza_stringa(&s);
    curl = curl_easy_init();
    if (!curl) return NULL;
    curl_easy_setopt(curl, CURLOPT_URL, URL_CLOCK);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, scrivi_dati);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        free(s.dati);
        return NULL;
    }
    FILE *cache = fopen(FILE_CACHE, "w");
    if (cache) {
        fwrite(s.dati, 1, s.lunghezza, cache);
        fclose(cache);
    }
    return s.dati;
}
char* leggi_cache_locale() {
    FILE *f = fopen(FILE_CACHE, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char *buffer = malloc(len + 1);
    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);
    return buffer;
}
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
    if (SISTEMA_WINDOWS) {
        HKEY chiave;
        DWORD tipo = REG_SZ;
        DWORD dimensione = sizeof(nome_cpu);
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &chiave) == ERROR_SUCCESS) {
            RegQueryValueExA(chiave, "ProcessorNameString", NULL, &tipo, (LPBYTE)nome_cpu, &dimensione);
            RegCloseKey(chiave);
            return nome_cpu;
        }
    }
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
    char *json_data = scarica_json();
    if (!json_data) json_data = leggi_cache_locale();
    if (!json_data) return 1000000000;
    struct json_object *root, *processori, *cpu, *clock_hz;
    root = json_tokener_parse(json_data);
    if (!root) return 1000000000;
    if (!json_object_object_get_ex(root, "processors", &processori)) return 1000000000;
    if (json_object_object_get_ex(processori, nome_cpu, &cpu)) {
        if (json_object_object_get_ex(cpu, "clock_hz", &clock_hz))
            return json_object_get_int64(clock_hz);
    }
    if (json_object_object_get_ex(processori, "Generic", &cpu)) {
        if (json_object_object_get_ex(cpu, "clock_hz", &clock_hz))
            return json_object_get_int64(clock_hz);
    }
    return 1000000000;
}
void ritardo_ns(long nanosecondi) {
    struct timespec inizio, attuale;
    clock_gettime(CLOCK_MONOTONIC, &inizio);
    do {
        clock_gettime(CLOCK_MONOTONIC, &attuale);
    } while ((attuale.tv_sec - inizio.tv_sec) * 1e9 + (attuale.tv_nsec - inizio.tv_nsec) < nanosecondi);
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
      printf("ERRORE: impossibile allocare la memoria.\n"); 
      exit(EXIT_FAILURE); 
    } 
  } 
  if (indice_memoria >= capacita_memoria) { 
    capacita_memoria *= 2; 
    int *temp = realloc(memoria, capacita_memoria * sizeof(int)); 
    if (!temp) { 
      printf("ERRORE: impossibile espandere la memoria.\n"); 
      free(memoria); 
      memoria = NULL; 
      exit(EXIT_FAILURE); 
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
    ritardo_ns(ciclo_ns);
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
void simula_alu_74181() {
  int Cn, M, A0, B0, A1, B1, A2, B2, A3, B3, S0, S1, S2, S3; 
  char scelta[3];
  printf("Inserire dati manualmente? (S/N): "); 
  scanf("%s", scelta); 
  scelta[0] = toupper(scelta[0]);
  if (scelta[0] == 'S') {
    printf(">> Inserisci i valori degli input (0 o 1):\n");
    printf(">> Cn: "); 
    scanf("%d", &Cn); 
    if (Cn == 0){
      /**/
    }
    else if(Cn == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n");
      printf("║            ERRORE              ║\n");
      printf("╠════════════════════════════════╣\n");
      printf("║                                ║\n");
      printf("║      Cn deve essere 0 o 1      ║\n");
      printf("║                                ║\n");
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> M:  "); 
    scanf("%d", &M); 
    if (M == 0){
      /**/
    }
    else if(M == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n");
      printf("║            ERRORE              ║\n");
      printf("╠════════════════════════════════╣\n");
      printf("║                                ║\n");
      printf("║       M deve essere 0 o 1      ║\n");
      printf("║                                ║\n");
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> A0: "); 
    scanf("%d", &A0); 
    if (A0 == 0){
      /**/
    }
    else if(A0 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n");
      printf("║            ERRORE              ║\n");
      printf("╠════════════════════════════════╣\n");
      printf("║                                ║\n");
      printf("║      A0 deve essere 0 o 1      ║\n");
      printf("║                                ║\n");
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> B0: "); 
    scanf("%d", &B0); 
    if (B0 == 0){
      /**/
    }
    else if(B0 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B0 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> A1: "); 
    scanf("%d", &A1); 
    if (A1 == 0){
      /**/
    }
    else if(A1 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      A1 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> B1: "); 
    scanf("%d", &B1); 
    if (B1 == 0){
      /**/
    }
    else if(B1 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B1 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> A2: "); 
    scanf("%d", &A2); 
    if (A2 == 0){
      /**/
    }
    else if(A2 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      A2 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> B2: "); 
    scanf("%d", &B2); 
    if (B2 == 0){
      /**/
    }
    else if(B2 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B2 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> A3: "); 
    scanf("%d", &A3); 
    if (A3 == 0){
      /**/
    }
    else if(A3 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      A3 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> B3: "); 
    scanf("%d", &B3); 
    if (B3 == 0){
      /**/
    }
    else if(B3 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B3 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> S0: "); 
    scanf("%d", &S0); 
    if (S0 == 0){
      /**/
    }
    else if(S0 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S0 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> S1: "); 
    scanf("%d", &S1); 
    if (S1 == 0){
      /**/
    }
    else if(S1 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S1 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> S2: "); 
    scanf("%d", &S2); 
    if (S2 == 0){
      /**/
    }
    else if(S2 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S2 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    printf(">> S3: "); 
    scanf("%d", &S3); 
    if (S3 == 0){
      /**/
    }
    else if(S3 == 1){
      /**/
    }
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S3 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
  } 
  else {
    FILE *file = fopen("input_alu.txt", "r"); 
    if (file == NULL) { 
      file = fopen("input_alu.txt", "w"); 
      if (file == NULL) { 
        printf("ERRORE: Impossibile creare il file\n"); 
        return; 
      } 
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
      return; 
    } 
    char line[100];
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_Cn = sscanf(line, "%*[^<]<%d>", &Cn); 
    if (sscanf_result_Cn == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in Cn\n"); 
      fclose(file); 
      return; 
    } 
    if (Cn == 0) {
      /**/
    } 
    else if (Cn == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n");
      printf("║            ERRORE              ║\n");
      printf("╠════════════════════════════════╣\n");
      printf("║                                ║\n");
      printf("║      Cn deve essere 0 o 1      ║\n");
      printf("║                                ║\n");
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); return; 
    } 
    int sscanf_result_M = sscanf(line, "%*[^<]<%d>", &M); 
    if (sscanf_result_M == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in M\n"); 
      fclose(file); 
      return; 
    } 
    if (M == 0) {
      /**/
    } 
    else if (M == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║       M deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_A0 = sscanf(line, "%*[^<]<%d>", &A0); 
    if (sscanf_result_A0 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in A0\n"); 
      fclose(file); 
      return; 
    } 
    if (A0 == 0) {
      /**/
    } 
    else if (A0 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      A0 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_B0 = sscanf(line, "%*[^<]<%d>", &B0); 
    if (sscanf_result_B0 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in B0\n"); 
      fclose(file); 
      return; 
    } 
    if (B0 == 0) {
      /**/
    } 
    else if (B0 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B0 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_A1 = sscanf(line, "%*[^<]<%d>", &A1); 
    if (sscanf_result_A1 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in A1\n"); 
      fclose(file); 
      return; 
    } 
    if (A1 == 0) {
      /**/
    } 
    else if (A1 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      A1 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_B1 = sscanf(line, "%*[^<]<%d>", &B1); 
    if (sscanf_result_B1 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in B1\n"); 
      fclose(file); 
      return; 
    } 
    if (B1 == 0) {
      /**/
    } 
    else if (B1 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B1 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_A2 = sscanf(line, "%*[^<]<%d>", &A2); 
    if (sscanf_result_A2 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in A2\n"); 
      fclose(file); 
      return; 
    } 
    if (A2 == 0) {
      /**/
    } 
    else if (A2 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      A2 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_B2 = sscanf(line, "%*[^<]<%d>", &B2); 
    if (sscanf_result_B2 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in B2\n"); 
      fclose(file); 
      return; 
    } 
    if (B2 == 0) {
      /**/
    } 
    else if (B2 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B2 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_A3 = sscanf(line, "%*[^<]<%d>", &A3); 
    if (sscanf_result_A3 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in A3\n"); 
      fclose(file); 
      return; 
    } 
    if (A3 == 0) {
      /**/
    } 
    else if (A3 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      A3 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_B3 = sscanf(line, "%*[^<]<%d>", &B3); 
    if (sscanf_result_B3 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in B3\n"); 
      fclose(file); 
      return; 
    } 
    if (B3 == 0) {
      /**/
    } 
    else if (B3 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      B3 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_S0 = sscanf(line, "%*[^<]<%d>", &S0); 
    if (sscanf_result_S0 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in S0\n"); 
      fclose(file); 
      return; 
    } 
    if (S0 == 0) {
      /**/
    } 
    else if (S0 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S0 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_S1 = sscanf(line, "%*[^<]<%d>", &S1); 
    if (sscanf_result_S1 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in S1\n"); 
      fclose(file); 
      return; 
    } 
    if (S1 == 0) {
      /**/
    } 
    else if (S1 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S1 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_S2 = sscanf(line, "%*[^<]<%d>", &S2); 
    if (sscanf_result_S2 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in S2\n"); 
      fclose(file); 
      return; 
    } 
    if (S2 == 0) {
      /**/
    } 
    else if (S2 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S2 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    if (fgets(line, sizeof(line), file) == NULL) { 
      printf("ERRORE: Formato file incompleto\n"); 
      fclose(file); 
      return; 
    } 
    int sscanf_result_S3 = sscanf(line, "%*[^<]<%d>", &S3); 
    if (sscanf_result_S3 == 1) {
      /**/
    } 
    else { 
      printf("ERRORE: Valore non valido in S3\n"); 
      fclose(file); 
      return; 
    } 
    if (S3 == 0) {
      /**/
    } 
    else if (S3 == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║      S3 deve essere 0 o 1      ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    }
    fclose(file);
  } 
  int A[4] = {A0, A1, A2, A3}; 
  int B[4] = {B0, B1, B2, B3}; 
  int S[4] = {S0, S1, S2, S3}; 
  int F[4], A_uguale_B, P, Cn_piu_4, G; n_ALU74181(Cn, M, A, B, S, F, &A_uguale_B, &P, &Cn_piu_4, &G); 
  printf("\n"); 
  printf("╔═════════════════════════════════════════════╗\n");
  printf("║           RISULTATI ALU 74181               ║\n");
  printf("╠═════════════════════════════════════════════╣\n");
  printf("║                                             ║\n");
  printf("║  - F0      = %-3d                            ║\n");
  printf("║  - F1      = %-3d                            ║\n");
  printf("║  - A = B   = %-3d                            ║\n");
  printf("║  - F2      = %-3d                            ║\n");
  printf("║  - F3      = %-3d                            ║\n");
  printf("║  - P       = %-3d                            ║\n");
  printf("║  - Cn + 4  = %-3d                            ║\n");
  printf("║  - G       = %-3d                            ║\n");
  printf("║                                             ║\n");
  printf("╚═════════════════════════════════════════════╝\n",F[0],F[1],A_uguale_B,F[2],F[3],P,Cn_piu_4,G); 
  salva_in_memoria(Cn_piu_4); 
  FILE *file_out = fopen("risultati_alu_74181.txt", "w"); 
  if (file_out == NULL) { 
    printf("╔════════════════════════════════╗\n"); 
    printf("║            ERRORE              ║\n"); 
    printf("╠════════════════════════════════╣\n"); 
    printf("║                                ║\n"); 
    printf("║    Impossibile aprire file     ║\n"); 
    printf("║         di scrittura           ║\n"); 
    printf("║                                ║\n"); 
    printf("╚════════════════════════════════╝\n"); 
    return; 
  } 
  fprintf(file_out, "╔═════════════════════════════════════════════╗\n");
  fprintf(file_out, "║           RISULTATI ALU 74181               ║\n");
  fprintf(file_out, "╠═════════════════════════════════════════════╣\n");
  fprintf(file_out, "║                                             ║\n");
  fprintf(file_out, "║  - F0      = %-3d                            ║\n");
  fprintf(file_out, "║  - F1      = %-3d                            ║\n");
  fprintf(file_out, "║  - A = B   = %-3d                            ║\n");
  fprintf(file_out, "║  - F2      = %-3d                            ║\n");
  fprintf(file_out, "║  - F3      = %-3d                            ║\n");
  fprintf(file_out, "║  - P       = %-3d                            ║\n");
  fprintf(file_out, "║  - Cn + 4  = %-3d                            ║\n");
  fprintf(file_out, "║  - G       = %-3d                            ║\n");
  fprintf(file_out, "║                                             ║\n");
  fprintf(file_out, "╚═════════════════════════════════════════════╝\n", F[0],F[1],A_uguale_B,F[2],F[3],P,Cn_piu_4,G); 
  fclose(file_out); 
  sleep(2); 
  return;
}
void ALU32() {
    unsigned int operandoA, operandoB; 
    int Cn, M, S[4]; 
    char scelta[3];
    printf("Inserire dati manualmente? (S/N): "); 
    scanf("%s", scelta); 
    scelta[0] = toupper(scelta[0]);
    if (scelta[0] == 'S') {
        printf(">> Inserisci il primo operando (numero decimale a 32 bit): "); 
        scanf("%u", &operandoA);
        printf(">> Inserisci il secondo operando (numero decimale a 32 bit): "); 
        scanf("%u", &operandoB);
        printf(">> Inserisci i valori degli input (0 o 1):\n");
        printf(">> Cn: "); 
        scanf("%d", &Cn); 
        if (Cn == 0){
          /**/
        }else if(Cn == 1){
          /**/
        }else { 
          printf("╔════════════════════════════════╗\n");
          printf("║            ERRORE              ║\n");
          printf("╠════════════════════════════════╣\n");
          printf("║                                ║\n");
          printf("║      Cn deve essere 0 o 1      ║\n");
          printf("║                                ║\n");
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        printf(">> M:  "); 
        scanf("%d", &M); 
        if (M == 0){
          /**/
        }else if(M == 1){
          /**/
        }else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║       M deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        printf(">> S0: "); 
        scanf("%d", &S[0]); 
        if (S[0] == 0){
          /**/
        }
        else if(S[0] == 1){
          /**/
        }
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S0 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        printf(">> S1: "); 
        scanf("%d", &S[1]); 
        if (S[1] == 0){
          /**/
        }
        else if(S[1] == 1){
          /**/
        }
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S1 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        printf(">> S2: "); 
        scanf("%d", &S[2]); 
        if (S[2] == 0){
          /**/
        }
        else if(S[2] == 1){
          /**/
        }
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S2 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        printf(">> S3: "); 
        scanf("%d", &S[3]); 
        if (S[3] == 0){
          /**/
        }
        else if(S[3] == 1){
          /**/
        }
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S3 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
    } else {
        FILE *file = fopen("input_alu32.txt", "r"); 
        if (file == NULL) { 
          file = fopen("input_alu32.txt", "w"); 
          if (file == NULL) { 
            printf("ERRORE: Impossibile creare il file\n"); 
            return; 
          } 
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
          return; 
        } 
        char line[100];
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_operandoA = sscanf(line, "%*[^<]<%d>", &operandoA);
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_operandoB = sscanf(line, "%*[^<]<%d>", &operandoB);
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_Cn = sscanf(line, "%*[^<]<%d>", &Cn); 
        if (sscanf_result_Cn == 1) {
          /**/
        } 
        else { 
          printf("ERRORE: Valore non valido in Cn\n"); 
          fclose(file); 
          return; 
        } 
        if (Cn == 0) {
          /**/
        } 
        else if (Cn == 1) {
          /**/
        } 
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      Cn deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_M = sscanf(line, "%*[^<]<%d>", &M); 
        if (sscanf_result_M == 1) {
          /**/
        } 
        else { 
          printf("ERRORE: Valore non valido in M\n"); 
          fclose(file); 
          return; 
        } 
        if (M == 0) {
          /**/
        } 
        else if (M == 1) {
          /**/
        } 
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║       M deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_S0 = sscanf(line, "%*[^<]<%d>", &S[0]); 
        if (sscanf_result_S0 == 1) {
          /**/
        } 
        else { 
          printf("ERRORE: Valore non valido in S0\n"); 
          fclose(file); 
          return; 
        } 
        if (S[0] == 0) {
          /**/
        } 
        else if (S[0] == 1) {
          /**/
        } 
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S0 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_S1 = sscanf(line, "%*[^<]<%d>", &S[1]); 
        if (sscanf_result_S1 == 1) {
          /**/
        } 
        else { 
          printf("ERRORE: Valore non valido in S1\n"); 
          fclose(file); 
          return; 
        } 
        if (S[1] == 0) {
          /**/
        } 
        else if (S[1] == 1) {
          /**/
        } 
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S1 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_S2 = sscanf(line, "%*[^<]<%d>", &S[2]); 
        if (sscanf_result_S2 == 1) {
          /**/
        } 
        else { 
          printf("ERRORE: Valore non valido in S2\n"); 
          fclose(file); 
          return; 
        } 
        if (S[2] == 0) {
          /**/
        } 
        else if (S[2] == 1) {
          /**/
        } 
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S2 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        }
        if (fgets(line, sizeof(line), file) == NULL) { 
          printf("ERRORE: Formato file incompleto\n"); 
          fclose(file); 
          return; 
        } 
        int sscanf_result_S3 = sscanf(line, "%*[^<]<%d>", &S[3]); 
        if (sscanf_result_S3 == 1) {
          /**/
        } 
        else { 
          printf("ERRORE: Valore non valido in S3\n"); 
          fclose(file); 
          return; 
        } 
        if (S[3] == 0) {
          /**/
        } 
        else if (S[3] == 1) {
          /**/
        } 
        else { 
          printf("╔════════════════════════════════╗\n"); 
          printf("║            ERRORE              ║\n"); 
          printf("╠════════════════════════════════╣\n"); 
          printf("║                                ║\n"); 
          printf("║      S3 deve essere 0 o 1      ║\n"); 
          printf("║                                ║\n"); 
          printf("╚════════════════════════════════╝\n"); 
          return; 
        } 
        fclose(file); 
    }
    int D_A[32], D_B[32], D_F[32]; 
    for (int i = 0; i < 32; i++) { 
      unsigned int tempA = operandoA; 
      for (int i = 0; i < 32; i++) { 
        D_A[i] = tempA % 2; tempA /= 2; 
      } 
      unsigned int tempB = operandoB; 
      for (int i = 0; i < 32; i++) { 
        D_B[i] = tempB % 2; 
        tempB /= 2; 
      } 
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
    int result = 0; 
    int currentCn = Cn; 
    int F[4], A_uguale_B, P, Cn_piu_4, G;  
    for (int nibble = 0; nibble < 8; nibble++) { 
      int Abits[4], Bbits[4]; 
      for (int bit = 0; bit < 4; bit++) { 
        Abits[bit] = Q_A[nibble * 4 + bit]; 
        Bbits[bit] = Q_B[nibble * 4 + bit]; 
      } 
      n_ALU74181(currentCn, M, Abits, Bbits, S, F, &A_uguale_B, &P, &Cn_piu_4, &G); 
      unsigned int nibbleResult = F[0] + F[1]*2 + F[2]*4 + F[3]*8; 
      int power = 1; 
      for (int i = 0; i < nibble; i++) { 
        power *= 16; 
      } 
      result += nibbleResult * power; 
      currentCn = Cn_piu_4; 
    } 
    for (int i = 0; i < 32; i++) { 
      int temp = result; 
      for (int j = 0; j < i; j++) { 
        temp = temp / 2; 
      } 
      D_F[i] = temp % 2; 
    } 
    for (int i = 0; i < 4; i++) { 
      clock_step(&CLK, &prev_CLK, 100); 
      reg_PIPO32(D_F, S_reg, R_reg, CLK, prev_CLK_F, Q_F, Q_bar_F); 
    } 
    printf("\n"); 
    printf("╔═════════════════════════════════════════════╗\n"); 
    printf("║           RISULTATI ALU 32bit               ║\n"); 
    printf("╚═════════════════════════════════════════════╝\n"); 
    printf("- Risultato      = %-3d",result); 
    salva_in_memoria(result); 
    FILE *file_out = fopen("risultati_alu32", "w"); 
    if (file_out == NULL) { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║            ERRORE              ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║    Impossibile aprire file     ║\n"); 
      printf("║         di scrittura           ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
      return; 
    } 
    fprintf(file_out, "╔═════════════════════════════════════════════╗\n");
    fprintf(file_out, "║           RISULTATI ALU 32bit               ║\n");
    fprintf(file_out, "╚═════════════════════════════════════════════╝\n");
    fprintf(file_out, "- Risultato      = %-3d\n", result); 
    fclose(file_out); 
    sleep(2);
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
              printf("╔════════════════════════════════╗\n"
                     "║         SOMMA ALGEBRICA        ║\n"
                     "╚════════════════════════════════╝\n"
                     "Risultato: %d\n", res);
              salva_in_memoria(res);
          }
          return;
      case 2:
          {
              int res = sottrazione(a, b);
              printf("╔════════════════════════════════╗\n"
                     "║      SOTTRAZIONE ALGEBRICA     ║\n"
                     "╚════════════════════════════════╝\n"
                     "Risultato: %d\n", res);
              salva_in_memoria(res);
          }
          return;
      case 3:
          {
              int res = moltiplicazione(a, b);
              printf("╔════════════════════════════════╗\n"
                     "║    MOLTIPLICAZIONE ALGEBRICA   ║\n"
                     "╚════════════════════════════════╝\n"
                     "Risultato: %d\n", res);
              salva_in_memoria(res);
          }
          return;
      case 4:
          {
              if (b == 0) {
                  printf("Errore: divisione per zero.\n");
              } else {
                  int res = divisione(a, b);
                  printf("╔════════════════════════════════╗\n"
                         "║        DIVISIONE ALGEBRICA     ║\n"
                         "╚════════════════════════════════╝\n"
                         "Risultato: %d\n", res);
                  salva_in_memoria(res);
              }
          }
          return;
      default:
          printf("╔════════════════════════════════╗\n"
                 "║             ERRORE             ║\n"
                 "╠════════════════════════════════╣\n"
                 "║                                ║\n"
                 "║   Operazione non riconosciuta  ║\n"
                 "║                                ║\n"
                 "╚════════════════════════════════╝\n");
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
                  printf("╔════════════════════════════════╗\n"
                         "║         SOMMA ALGEBRICA        ║\n"
                         "╚════════════════════════════════╝\n"
                         "Risultato: %d\n", res);
                  salva_in_memoria(res);
              }
              return;
          case 3:
              {
                  int res = moltiplicazione(moltiplicazione(a, b), c);
                  printf("╔════════════════════════════════╗\n"
                         "║    MOLTIPLICAZIONE ALGEBRICA   ║\n"
                         "╚════════════════════════════════╝\n"
                         "Risultato: %d\n", res);
                  salva_in_memoria(res);
              }
              return;
          default:
              printf("╔════════════════════════════════╗\n"
                     "║             ERRORE             ║\n"
                     "╠════════════════════════════════╣\n"
                     "║                                ║\n"
                     "║   Operazione non riconosciuta  ║\n"
                     "║                                ║\n"
                     "╚════════════════════════════════╝\n");
              return;
      }
  } else {
      printf("╔════════════════════════════════╗\n"
             "║             ERRORE             ║\n"
             "╠════════════════════════════════╣\n"
             "║                                ║\n"
             "║ Numero di elementi non valido. ║\n"
             "║      Scegli tra 2 oppure 3     ║\n"
             "║                                ║\n"
             "╚════════════════════════════════╝\n");
  }
void misura_ciclo_clock() {
    printf("\n==============================\n");
    printf("  Rilevamento del sistema\n");
    printf("==============================\n\n");
    char *cpu = rileva_cpu();
    printf("CPU rilevata: %s\n", cpu);
    long freq = ottieni_clock(cpu);
    printf("Frequenza di clock trovata: %ld Hz\n", freq);
    double durata_ns = (1.0 / freq) * 1e9;
    printf("Durata stimata di un ciclo: %.3f ns\n", durata_ns);
    printf("\n==============================\n");
    printf("  Simulazione clock in corso\n");
    printf("==============================\n\n");
    for (int i = 0; i < 1000; i++) attendi_un_ciclo_clock();
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
    fgets(input, sizeof(input), stdin); 
    if (sscanf(input, "%d", &scelta) == 1) {
      /**/
    } 
    else { 
      printf("╔════════════════════════════════╗\n"); 
      printf("║             ERRORE             ║\n"); 
      printf("╠════════════════════════════════╣\n"); 
      printf("║                                ║\n"); 
      printf("║   Inserisci un numero valido  ║\n"); 
      printf("║                                ║\n"); 
      printf("╚════════════════════════════════╝\n"); 
        continue; 
      } if (scelta == 0) { 
        printf("Uscita dal programma...\n"); 
        break; 
      } 
      else if (scelta == 1) { 
        simula_alu_74181(); 
      } 
      else if (scelta == 2) { 
        sleep(2); 
        simula_alu_74181(); 
      } 
      else if (scelta == 3) { 
        operazioni_algebriche(); 
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
          } 
        } 
        else { 
          FILE *file = fopen("input_bin.txt", "r"); 
          if (!file) { 
            file = fopen("input_bin.txt", "w"); 
            if (!file) { 
              printf("ERRORE: Impossibile creare il file\n"); 
              return 1; 
            } 
            fprintf(file, "Numero Binario: <0>\n"); 
            fclose(file); 
            printf("Creato file input_bin.txt. Compilarlo e riavviare.\n"); 
            return 1; 
          } 
          char line[100]; 
          if (!fgets(line, sizeof(line), file)) { 
            printf("ERRORE: Formato file incompleto\n"); 
            fclose(file); 
            return 1; 
          } 
          fclose(file); 
          if (sscanf(line, "%*[^<]<%32[^>]>", bin) != 1) { 
            printf("ERRORE: Formato binario non trovato\n"); 
            return 1; 
          } 
          int risultato = BIN_DEC_DECODER(bin); 
          FILE *file_out = fopen("risultati_dec.txt", "w"); 
          if (!file_out) { 
            printf("╔════════════════════════════════╗\n"); 
            printf("║            ERRORE              ║\n"); 
            printf("╠════════════════════════════════╣\n"); 
            printf("║                                ║\n"); 
            printf("║    Impossibile aprire file     ║\n"); 
            printf("║         di scrittura           ║\n"); 
            printf("║                                ║\n"); 
            printf("╚════════════════════════════════╝\n"); 
            return 1; 
          } 
          fprintf(file_out, "╔═════════════════════════════════════════════╗\n"); 
          fprintf(file_out, "║          RISULTATI CONVERTITORE             ║\n"); 
          fprintf(file_out, "╚═════════════════════════════════════════════╝\n"); 
          fprintf(file_out, "Risultato      = %-3d", risultato); 
          fclose(file_out); 
          sleep(2); 
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
        } 
        else { 
          FILE *file = fopen("input_dec.txt", "r"); 
          if (!file) { 
            file = fopen("input_dec.txt", "w"); 
            if (!file) { 
              printf("ERRORE: Impossibile creare il file\n"); 
              return 1; 
            } 
            fprintf(file, "Numero Decimale: <0>\n"); 
            fclose(file); 
            printf("Creato file input_dec.txt. Compilarlo e riavviare.\n"); 
            return 1; 
          } 
          char line[100]; 
          if (!fgets(line, sizeof(line), file)) { 
            printf("ERRORE: Formato file incompleto\n"); 
            fclose(file); 
            return 1; 
          } 
          fclose(file); 
          char buffer[33]; 
          if (sscanf(line, "%*[^<]<%32[^>]>", buffer) != 1) { 
            printf("ERRORE: Formato binario non trovato\n"); 
            return 1; 
          } 
          dec = atoi(buffer); 
          FILE *file_out = fopen("risultati_bin.txt", "w"); 
          if (!file_out) { 
            printf("╔════════════════════════════════╗\n"); 
            printf("║            ERRORE              ║\n"); 
            printf("╠════════════════════════════════╣\n"); 
            printf("║                                ║\n"); 
            printf("║    Impossibile aprire file     ║\n"); 
            printf("║         di scrittura           ║\n"); 
            printf("║                                ║\n"); 
            printf("╚════════════════════════════════╝\n"); 
            return 1; 
          } 
          fprintf(file_out, "╔═════════════════════════════════════════════╗\n");
          fprintf(file_out, "║          RISULTATI CONVERTITORE             ║\n"); 
          fprintf(file_out, "╚═════════════════════════════════════════════╝\n");
          fprintf(file_out, "Risultato      = %-16s", DEC_BIN_CODER(dec)); 
          fclose(file_out); 
          sleep(2); 
        } 
      } 
      else if (scelta == 6) { 
        ALU32(); 
      } else if (scelta == 7) { 
        sleep(2); 
        ALU32(); 
      } 
      else if (scelta == 8) { 
        stampa_memoria(); 
        stato_memoria(); 
      } 
      else if (scelta == 9) { 
        misura_ciclo_clock(); 
      } 
      else { 
        printf("Scelta non valida!\n"); 
      } 
      sleep(2); 
    } 
    if (memoria != NULL) { 
      free(memoria); 
      memoria = NULL; 
      printf("[INFO] Memoria liberata.\n"); 
    } 
    return 0; 
  }