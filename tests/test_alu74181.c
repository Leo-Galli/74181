/*
 * Test di regressione per n_ALU74181 (74181.c).
 *
 * Verifica il nucleo gate-level dell'ALU rispetto a identita' aritmetiche e
 * logiche universalmente note del chip 74181 reale (somma con riporto in
 * modalita' aritmetica, AND/OR/XOR/passaggio di A in modalita' logica, e la
 * semantica del flag A=B), su TUTTE le combinazioni di operandi rilevanti
 * (16 x 16 per ciascuna identita', 16 x 16 x 16 per A=B su tutti i 16
 * codici di selezione). Include 74181.c direttamente, rinominando main()
 * per evitare la doppia definizione.
 */
#define main main_simulatore_non_usato_nei_test
#include "../74181.c"
#undef main

static void set4(int v, int out[4]) {
    out[0] = v & 1;
    out[1] = (v >> 1) & 1;
    out[2] = (v >> 2) & 1;
    out[3] = (v >> 3) & 1;
}
static int get4(int in[4]) {
    return in[0] | (in[1] << 1) | (in[2] << 2) | (in[3] << 3);
}

int main(void) {
    int fallimenti = 0, totale = 0;
    int F[4], AeqB, P, Cout, G;

    /* S=1001 (9), M=0: identita' "ADD" -> F = (A+B+riporto) mod 16.
       Cn e Cn+4 sono attivi bassi (Cn=0 => riporto in ingresso = 1),
       fedelmente al pin reale del chip 74181. */
    for (int a = 0; a < 16; a++) for (int b = 0; b < 16; b++) for (int cin = 0; cin < 2; cin++) {
        int A[4], B[4], S[4];
        set4(a, A); set4(b, B); set4(9, S);
        n_ALU74181(1 - cin, 0, A, B, S, F, &AeqB, &P, &Cout, &G);
        int somma = a + b + cin;
        int atteso = somma & 0xF;
        int riporto_atteso = (somma > 15) ? 1 : 0;
        int cout_atteso_pin = 1 - riporto_atteso;
        totale++;
        if (get4(F) != atteso || Cout != cout_atteso_pin) {
            fallimenti++;
            printf("[ADD] a=%d b=%d cin=%d -> F=%d Cout=%d, atteso F=%d Cout=%d\n",
                a, b, cin, get4(F), Cout, atteso, cout_atteso_pin);
        }
    }

    /* S=1011 (11), M=1: identita' "AND" -> F = A AND B */
    for (int a = 0; a < 16; a++) for (int b = 0; b < 16; b++) {
        int A[4], B[4], S[4];
        set4(a, A); set4(b, B); set4(11, S);
        n_ALU74181(0, 1, A, B, S, F, &AeqB, &P, &Cout, &G);
        totale++;
        if (get4(F) != (a & b)) {
            fallimenti++;
            printf("[AND] a=%d b=%d -> F=%d, atteso %d\n", a, b, get4(F), a & b);
        }
    }

    /* S=1110 (14), M=1: identita' "OR" -> F = A OR B */
    for (int a = 0; a < 16; a++) for (int b = 0; b < 16; b++) {
        int A[4], B[4], S[4];
        set4(a, A); set4(b, B); set4(14, S);
        n_ALU74181(0, 1, A, B, S, F, &AeqB, &P, &Cout, &G);
        totale++;
        if (get4(F) != (a | b)) {
            fallimenti++;
            printf("[OR] a=%d b=%d -> F=%d, atteso %d\n", a, b, get4(F), a | b);
        }
    }

    /* S=0110 (6), M=1: identita' "XOR" -> F = A XOR B */
    for (int a = 0; a < 16; a++) for (int b = 0; b < 16; b++) {
        int A[4], B[4], S[4];
        set4(a, A); set4(b, B); set4(6, S);
        n_ALU74181(0, 1, A, B, S, F, &AeqB, &P, &Cout, &G);
        totale++;
        if (get4(F) != (a ^ b)) {
            fallimenti++;
            printf("[XOR] a=%d b=%d -> F=%d, atteso %d\n", a, b, get4(F), a ^ b);
        }
    }

    /* S=1111 (15), M=1: identita' "PASS A" -> F = A */
    for (int a = 0; a < 16; a++) {
        int A[4], B[4], S[4];
        set4(a, A); set4(0, B); set4(15, S);
        n_ALU74181(0, 1, A, B, S, F, &AeqB, &P, &Cout, &G);
        totale++;
        if (get4(F) != a) {
            fallimenti++;
            printf("[PASS A] a=%d -> F=%d, atteso %d\n", a, get4(F), a);
        }
    }

    /* Flag A=B: sull'hardware reale e' definito come AND di tutti i bit F
       (F0 & F1 & F2 & F3), indipendentemente dall'operazione selezionata. */
    for (int s = 0; s < 16; s++) for (int a = 0; a < 16; a++) for (int b = 0; b < 16; b++) {
        int A[4], B[4], S[4];
        set4(a, A); set4(b, B); set4(s, S);
        n_ALU74181(0, 1, A, B, S, F, &AeqB, &P, &Cout, &G);
        totale++;
        int atteso = (get4(F) == 15) ? 1 : 0;
        if (AeqB != atteso) {
            fallimenti++;
            printf("[A=B] s=%d a=%d b=%d -> AeqB=%d, atteso %d\n", s, a, b, AeqB, atteso);
        }
    }

    /* BIN_DEC_DECODER / DEC_BIN_CODER: round-trip su tutti i valori a 8 bit. */
    for (int v = 0; v < 256; v++) {
        char *bin = DEC_BIN_CODER(v);
        int round_trip = BIN_DEC_DECODER(bin);
        totale++;
        if (round_trip != v) {
            fallimenti++;
            printf("[BIN<->DEC] v=%d -> bin=%s -> %d\n", v, bin, round_trip);
        }
    }

    printf("\nTotale asserzioni: %d, fallite: %d\n", totale, fallimenti);
    if (fallimenti == 0) {
        printf("OK: n_ALU74181 e i convertitori binari superano tutte le verifiche.\n");
    }
    return fallimenti != 0;
}
