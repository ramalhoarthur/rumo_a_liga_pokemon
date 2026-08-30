# 🏆 Rumo à Liga Pokémon

> **Disciplina:** Algoritmos em Grafos (CCT/UFCA) - 1° Semestre de 2026  
> **Professor:** Carlos Vinicius G. C. Lima  

## 📜 Sobre o Projeto
O projeto **"Rumo à Liga Pokémon"** é um simulador de jornada Pokémon baseado na teoria dos grafos. O objetivo principal é desenvolver um sistema onde um treinador viaja por uma região (representada por um grafo ponderado), enfrenta líderes de ginásio e coleta 8 insígnias oficiais para se inscrever no torneio da Liga Pokémon dentro de um prazo estabelecido.

O sistema simula a modelagem computacional da região, exigindo a aplicação de algoritmos de menor complexidade assintótica possível para navegação, além de implementar as dinâmicas clássicas de evolução, captura e batalhas.

## 👥 Equipe
- **Andrey Markov Maia Sousa** - [Descrever brevemente quais operações/algoritmos implementou]
- **Arthur de Souza Ramalho** - [Descrever brevemente quais operações/algoritmos implementou]
- **Luís Felipe Dantas Morais** - [Descrever brevemente quais operações/algoritmos implementou]

---

## 🗺️ O Mundo (Grafo)
A região é modelada como um grafo ponderado lido a partir de um arquivo texto.
* **Vértices e Arestas:** Representam caminhos possíveis. As arestas possuem pesos correspondentes ao tempo necessário para percorrer o trecho. É possível se mover em qualquer direção.
* **Movimentação:** Treinadores, líderes de ginásio e pokémons selvagens se movem um vértice por vez pela região.
* **Locais Especiais:**
  * **Laboratório do Prof. Carvalho:** Ponto inicial da jornada. Local de envio de pokémons excedentes. Não permite batalhas.
  * **Ginásios:** Locais fixos (ou periódicos) onde os líderes podem ser desafiados em troca de insígnias.
  * **Centro Médico Pokémon (PMC):** Local obrigatório para recuperação de pokémons gravemente feridos. Não permite batalhas.
  * **Estádio da Liga Pokémon:** Destino final para inscrição no torneio, antes do esgotamento do tempo limite.

### Formato do mapa

O arquivo `data/entrada.txt` é lido pela classe `Grafo` e segue este formato:

```text
N M
id "nome" tipo possui_cmp possui_ginasio possui_laboratorio
... (N linhas de vértices, em ordem de ID, de 0 a N-1)
origem destino peso
... (M linhas de arestas)
```

Os indicadores de locais especiais usam `0` (não) ou `1` (sim). O mapa é não direcionado: uma linha `u v peso` permite viajar de `u` para `v` e de `v` para `u`. Os pesos precisam ser positivos e representam o tempo de deslocamento.

Exemplo de uso no código:

```cpp
#include "headings/grafo.hpp"

Grafo mapa("data/entrada.txt");
const auto& cidade_inicial = mapa.vertice(0);
const auto& caminhos = mapa.vizinhos(0);
```

O cenário completo fica em `data/cenario.txt`. Ele informa o mapa, prazo da Liga, quantidades de entidades e as formas evolutivas das espécies:

```text
MAPA "entrada.txt"
PRAZO 1260
SELVAGENS 12
TREINADORES 4
ERVAS 6
OVOS 2
ESPECIES 3
3 "Bulbasaur" 1 GRAMA "Ivysaur" 1 GRAMA "Venusaur" 2 GRAMA VENENOSO
```

Em cada espécie, o primeiro número é a quantidade de formas (de 1 a 3). Para cada forma, são lidos nome entre aspas, número de tipos e os tipos. Ao carregar o cenário, as posições e atributos de selvagens, treinadores e ervas são sorteados com valores válidos.

## ⚔️ Mecânicas e Regras
* **Equipe Pokémon:** Máximo de 6 ativos (e/ou ovos, limite de 7 total). Pokémons excedentes capturados vão para o Professor Carvalho.
* **Batalhas em Turnos:** Baseadas em HP (Saúde), AP (Ataque) e DP (Defesa). Envolvem cálculos de dano e chances de esquiva proporcionais à diferença de XP (Experiência). O treinador desafiado ataca primeiro.
* **Evolução:** Pokémons ganham XP lutando, crescendo (distância percorrida) e através do treinador. Ao atingir 1000 XP, evoluem (aumentando AP e DP em 30%), com limite de até 3 fases.
* **Recursos de Aventura:**
  * **Pokébolas:** Utilizadas para manter a equipe ou capturar selvagens inconscientes.
  * **Encubadoras:** Chocam ovos após 100 unidades de distância percorridas.
  * **Remédios Naturais:** Feitos com ervas coletadas pelo caminho, recuperam 10 HP de pokémons conscientes.
* **Estados de Saúde:**
  * **Consciente:** HP $\ge$ 20.
  * **Inconsciente:** Indisponível para batalhas temporariamente.
  * **Gravemente Ferido:** HP $<$ 5 (Exige repouso obrigatório no PMC para restaurar HP a 100).

## ⭐ Requisitos Extras (Bônus)

- [x] **Vantagens de Tipo:** A matriz `17 × 17` em `headings/vantagens_tipo.hpp` mantém os tipos em ordem alfabética: Aço, Água, Dragão, Elétrico, Fada, Fantasma, Fogo, Gelo, Inseto, Lutador, Normal, Pedra, Planta, Psíquico, Sombrio, Terra, Venenoso e Voador. Linhas representam o atacante; colunas, o defensor. Vantagem vale `1.5`, desvantagem vale `0.5` e relações neutras valem `1.0`. Em Pokémon de dois tipos, é escolhido automaticamente o tipo atacante de maior efeito e os tipos defensivos multiplicam seus efeitos. Nos dois conflitos visuais da tabela (Dragão×Dragão e Fantasma×Fantasma), a vantagem prevalece, resultando em `1.5`.
- [x] **Equipe Rocket:** Atua como um treinador NPC com equipe própria. Caso vença um duelo, rouba uma insígnia ou Pokémon e fica invisível por 20 a 100 unidades de tempo, voltando em ponto aleatório válido. Caso perca, é reposicionada em um ponto aleatório distante do confronto.

---

## 🚀 Como Executar

É necessário um compilador compatível com C++17. A partir da raiz do repositório:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iheadings src/main.cpp -o rumo_liga
./rumo_liga data/cenario.txt
```

Também é possível gerar o executável com CMake:

```bash
cmake -S . -B build
cmake --build build
./build/rumo_liga data/cenario.txt
```

No menu, o jogador pode mover-se por arestas adjacentes, consultar uma rota mínima, usar ervas, enfrentar selvagens e treinadores, desafiar líderes, confrontar a Equipe Rocket e tentar a inscrição na Liga. O atendimento no PMC recupera a equipe após um tempo aleatório; batalhas no laboratório e no PMC são bloqueadas.

---

## 📁 Estrutura do Projeto e Configuração Inicial
* `src/` - Contém todo o código-fonte desenvolvido.
* `data/` - Arquivos de texto `.txt` utilizados para carregar a descrição inicial do mapa, quantidades de entidades e posições aleatórias.
* `README.md` - Documentação principal do repositório.

## 🎥 Apresentação
Links para os vídeos no YouTube/Drive contendo a explicação individual da implementação:
* 📺 **[Nome do Membro 1]:** [Link do vídeo]
* 📺 **[Nome do Membro 2]:** [Link do vídeo]
* 📺 **[Nome do Membro 3]:** [Link do vídeo]
