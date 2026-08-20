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
*(Marque com um 'x' os bônus que a equipe conseguiu implementar)*
- [ ] **Vantagens de Tipo:** Lógica de vantagens e desvantagens entre os tipos de pokémon nas batalhas.
- [ ] **Equipe Rocket:** Equipe NPC que se movimenta roubando pokémons/insígnias após vencer batalhas, sumindo e reaparecendo no mapa aleatoriamente.

---

## 🚀 Como Executar

<!-- ESPAÇO RESERVADO: Preencha no futuro com as instruções para rodar o código (ex: comandos de terminal, dependências, versão do compilador/interpretador) -->
> *As instruções de execução e instalação de dependências serão adicionadas a esta seção no futuro.*

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