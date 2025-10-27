# Simulatore ALU 74181

[![C](https://img.shields.io/badge/Linguaggio-C-blue.svg?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Standard](https://img.shields.io/badge/Standard-C99-orange.svg)](https://en.wikipedia.org/wiki/C99)
![Licenza](https://img.shields.io/github/license/Leo-Galli/74181)
![Downloads](https://img.shields.io/github/downloads/Leo-Galli/74181/total)

![Simulator](https://img.shields.io/badge/Simulatore-74181-blue)
![Simulator](https://img.shields.io/badge/Simulatore-74198-blue)
![GitHub Release](https://img.shields.io/github/v/release/Leo-Galli/74181)
![GitHub Tag](https://img.shields.io/github/v/tag/Leo-Galli/74181)

[![Allegati](https://img.shields.io/badge/Allegato-Report_Tecnico-yellow)](https://docs.google.com/document/d/1t80KM4RDQKBMuIQoPLAFnJRQeKX5ISx1Id5GahRIR-E/edit?usp=sharing)
[![Allegati](https://img.shields.io/badge/Allegato-Circuito_Logico-yellow)](https://circuitverse.org/users/311719/projects/alu-74181-final)
[![Allegati](https://img.shields.io/badge/Allegato-Licenza-yellow)](https://github.com/Leo-Galli/74181/blob/main/LICENSE)
[![Allegati](https://img.shields.io/badge/Allegato-Codice_di_Condotta-yellow)](https://github.com/Leo-Galli/74181/blob/main/CODE_OF_CONDUCT.md)
[![Allegati](https://img.shields.io/badge/Allegato-Wiki-yellow)](https://github.com/Leo-Galli/74181/wiki)

![file or directory count](https://img.shields.io/github/directory-file-count/Leo-Galli/74181)
[![Repo Size](https://img.shields.io/github/repo-size/Leo-Galli/74181)](https://github.com/Leo-Galli/74181)
[![Linee di Codice](https://tokei.rs/b1/github/Leo-Galli/74181)](https://github.com/Leo-Galli/74181)

[![Stelle](https://img.shields.io/github/stars/Leo-Galli/74181?style=flat)](https://github.com/Leo-Galli/74181/stargazers)
[![Forks](https://img.shields.io/github/forks/Leo-Galli/74181?style=flat)](https://github.com/Leo-Galli/74181/network/members)
[![Issue](https://img.shields.io/github/issues/Leo-Galli/74181)](https://github.com/Leo-Galli/74181/issues)
[![Pull Request](https://img.shields.io/github/issues-pr/Leo-Galli/74181)](https://github.com/Leo-Galli/74181/pulls)
[![Ultimo Commit](https://img.shields.io/github/last-commit/Leo-Galli/74181)](https://github.com/Leo-Galli/74181/commits/main)
![Visualizzazioni](https://komarev.com/ghpvc/?username=Leo-Galli-74181&label=Visualizzazioni+Repository&color=blueviolet&style=flat)

[![C/C++ CI](https://github.com/Leo-Galli/74181/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/Leo-Galli/74181/actions/workflows/c-cpp.yml)
[![CodeQL](https://github.com/Leo-Galli/74181/actions/workflows/github-code-scanning/codeql/badge.svg?branch=main)](https://github.com/Leo-Galli/74181/actions/workflows/github-code-scanning/codeql)
[![Microsoft C Code Analysis (74181)](https://github.com/Leo-Galli/74181/actions/workflows/msvc.yml/badge.svg)](https://github.com/Leo-Galli/74181/actions/workflows/msvc.yml)

[![CMake on a single platform](https://github.com/Leo-Galli/74181/actions/workflows/cmake-single-platform.yml/badge.svg?branch=main)](https://github.com/Leo-Galli/74181/actions/workflows/cmake-single-platform.yml)
[![CMake on multiple platforms](https://github.com/Leo-Galli/74181/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=main)](https://github.com/Leo-Galli/74181/actions/workflows/cmake-multi-platform.yml)

[![Dependency review](https://github.com/Leo-Galli/74181/actions/workflows/dependency-review.yml/badge.svg?branch=main)](https://github.com/Leo-Galli/74181/actions/workflows/dependency-review.yml)

[![Security Policy](https://img.shields.io/badge/Security-Policy-blueviolet)](https://github.com/Leo-Galli/74181/security/policy)
[![Dependabot](https://img.shields.io/badge/Dependabot-Enabled-success)](https://github.com/Leo-Galli/74181/network/updates)
[![License Scan](https://img.shields.io/badge/License-Scan-green)](https://github.com/Leo-Galli/74181/blob/main/LICENSE)

---

## Descrizione

Simulatore didattico in **C (C99)** dell’unità aritmetico-logica **74181**.  
Supporta operazioni a 4 bit e un’estensione a **32 bit** mediante concatenazione di più ALU.  
Pensato per studio e analisi dei flag **Carry, Overflow, Zero, Negative**.

---

## Funzionalità

- Operazioni logiche: AND, OR, XOR, NOT  
- Operazioni aritmetiche: somma, sottrazione, incremento, decremento  
- Conversione binario ↔ decimale  
- ALU a 32 bit con propagazione carry look-ahead  
- Input/output da file e generazione automatica risultati  
- Clock simulato per studio di latenze  

---

## File

**Input**  
`input_alu.txt` – Operazioni 74181  
`input_alu32.txt` – Operazioni 32 bit  
`input_bin.txt` – Binario → Decimale  
`input_dec.txt` – Decimale → Binario  

**Output**  
`risultati_alu_74181.txt`  
`risultati_alu32.txt`  
`risultati_bin.txt`  
`risultati_dec.txt`  

---

## Installazione

```bash
git clone https://github.com/Leo-Galli/74181.git
cd 74181
gcc 74181.c -o simulatore_74181
./simulatore_74181
````

---

## Requisiti

* Compilatore C conforme a C99 (es. gcc, clang)
* Sistema operativo: Windows / Linux / macOS

---

## Statistiche

![Star History](https://api.star-history.com/svg?repos=Leo-Galli/74181\&type=Date)
![RepoBeats](https://repobeats.axiom.co/api/embed/a576eab04e8ea577550bfb4dd32de862737655ab.svg)

---

## Risorse

* [Report tecnico](https://docs.google.com/document/d/1t80KM4RDQKBMuIQoPLAFnJRQeKX5ISx1Id5GahRIR-E/edit?usp=sharing)
* [Schema CircuitVerse](https://circuitverse.org/users/311719/projects/alu-74181-final)
* [Lista bug](https://github.com/Leo-Galli/74181/blob/main/bugs.txt)
* [Simulazione Wokwi](https://wokwi.com/projects/439836726113417217)

---

## Licenza

Distribuito sotto licenza **GPL v3.0**.
Vedi [LICENSE](LICENSE) per i dettagli.

---

## Contributors

<a href="https://github.com/Leo-Galli/74181/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Leo-Galli/74181" />
</a>
