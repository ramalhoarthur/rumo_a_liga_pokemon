# Plano de Desenvolvimento — Rumo à Liga Pokémon

Este documento organiza o desenvolvimento do projeto em módulos funcionais e fases sequenciais, cobrindo todos os requisitos obrigatórios do enunciado e os itens bônus.

---

## 1. Modelagem e leitura de dados

- [ ] Definir o formato do arquivo de entrada do grafo (vértices, arestas, pesos).
- [ ] Implementar leitura do grafo a partir de arquivo texto.
- [ ] Definir o formato de arquivo com: quantidade de pokémons/treinadores/itens da região, localização de ginásios, CMP, laboratório e estádio.
- [ ] Implementar geração aleatória de atributos iniciais (XP, AP, DP, HP, localização) dos pokémons/treinadores/itens espalhados no mapa.
- [ ] Implementar leitura das fases de evolução de cada espécie (nomes distintos por fase, até 3 fases).
- [ ] Implementar leitura/validação do prazo máximo de inscrição na Liga (entre 10x e 15x a soma dos pesos das arestas).
- [ ] (Bônus) Definir e ler tabela de vantagens entre tipos de pokémon.

## 2. Estruturas de dados base

- [ ] **Grafo**: escolher representação eficiente (lista de adjacência recomendada, dado que o grafo tende a ser esparso).
- [ ] **Pokémon**: tipo(s), fase evolutiva, XP, AP, DP, HP, estado (consciente/inconsciente/muito machucado), ataques disponíveis.
- [ ] **Treinador**: até 6 pokémons ativos + 1 ovo não chocado (máx. 7), pokébolas (6 para treino/batalha + 1 para captura), insígnias conquistadas, XP próprio, posição atual no grafo.
- [ ] **Líder de Ginásio**: posição (fixa ou itinerante), insígnia associada, tempo de permanência no ginásio quando itinerante.
- [ ] **Ovo/Incubadora**: distância percorrida acumulada, tempo restante para chocar.
- [ ] **Item/Erva medicinal**: efeito (recuperação de HP).

## 3. Algoritmos sobre o grafo

> Cada integrante deve ficar responsável por pelo menos uma operação sobre o grafo — sugestão de divisão abaixo.

- [ ] Algoritmo de caminho mínimo (ex.: Dijkstra) — útil para calcular deslocamento entre pontos de interesse (ginásios, CMP, estádio) e simular o percurso vértice a vértice.
- [ ] Algoritmo de busca (BFS/DFS) — para movimentação livre pelo mapa e verificação de alcançabilidade.
- [ ] Estrutura/algoritmo para simular a movimentação incremental (um vértice por vez) de treinadores e pokémons selvagens.
- [ ] Lógica de retorno periódico de líderes de ginásio itinerantes ao seu ginásio de origem.

## 4. Sistema de pokémon

- [ ] Ganho de XP: vitória (+10), derrota (+3), deslocamento (+1 a cada 100 unidades de distância).
- [ ] Atualização de AP/DP: +1 unidade ao vencer oponente com XP ≥ seu; valor total = inicial + 10% do XP acumulado.
- [ ] Evolução: a cada 1000 XP acumulados, nova fase com AP/DP +30% em relação à fase anterior (máx. 3 fases).
- [ ] Sistema de ovos: incubação equivalente a 100 unidades de distância; ao nascer, XP = 0 e AP/DP de fase inicial da espécie; tipo desconhecido até chocar.
- [ ] Sistema de HP (1–100): consciente se HP ≥ 20; muito machucado se HP < 5 (vai ao CMP); recuperação natural de 1 HP a cada 10 unidades de distância; remédio natural (+10 HP, só em pokémons conscientes); HP = 100 ao sair do CMP.
- [ ] Tempo de indisponibilidade após batalha: aleatório entre 10 e 50 unidades de distância (inconsciente) ou no CMP (muito machucado).

## 5. Sistema de treinador

- [ ] Recebimento inicial: escolha entre 3 pokémons de tipos distintos (água, fogo, planta) ou 1 pokémon aleatório, + incubadora + 7 pokébolas.
- [ ] Gerenciamento do limite de 6 pokémons ativos (excedente vai para o Professor Carvalho).
- [ ] Ganho de XP do treinador: +3 (vitória sobre XP ≥ seu) ou +1 (caso contrário).
- [ ] Registro de insígnias conquistadas (permanentes, mesmo após derrotas futuras).

## 6. Sistema de batalhas

- [ ] Batalha pokémon vs. pokémon: turnos alternados (desafiado ataca primeiro), dano = AP atacante − DP defensor (mínimo 0), chance de esquiva proporcional a |XP atacante − XP defensor|, chance de dano crítico (dobro) proporcional à mesma diferença.
- [ ] Substituição automática de pokémon inconsciente por outro escolhido pelo treinador.
- [ ] Batalha treinador vs. treinador: exige 3 pokémons conscientes, vitória quando os 3 pokémons do oponente ficam inconscientes ou o desafiado desiste (desafiante não pode desistir).
- [ ] Captura de pokémon selvagem: batalha contra pokémon sem treinador; captura automática se ficar inconsciente; possibilidade de abandono (pokémon foge); +3 XP para treinador e pokémons envolvidos em captura bem-sucedida.
- [ ] Batalha contra Líder de Ginásio: vitória concede a insígnia correspondente.
- [ ] Restrição: nenhuma batalha pode ocorrer no CMP ou no laboratório do Professor Carvalho.
- [ ] Cada batalha consome 1 unidade de tempo percorrido.

## 7. Sistema de inscrição na Liga

- [ ] Verificação de 8 insígnias distintas conquistadas.
- [ ] Verificação do prazo máximo de inscrição (definido a partir do grafo).
- [ ] Determinação de apto/inapto para a competição.

## 8. Elementos bônus (extras)

- [ ] **Vantagens de tipo**: implementar tabela de fraquezas/resistências entre tipos e aplicar multiplicadores de dano nas batalhas, respeitando as vantagens originais dos jogos.
- [ ] **Equipe Rocket**: treinadores que roubam pokémons/insígnias via duelo obrigatório; ao perder, são reposicionados em local aleatório distante; ao vencer, ficam invisíveis por um tempo e reaparecem em local aleatório.

## 9. Testes e validação

- [ ] Testes unitários por módulo (grafo, pokémon, treinador, batalha).
- [ ] Simulação de ponta a ponta: jornada completa até a inscrição na Liga.
- [ ] Casos-limite: HP no limiar de consciência, XP exatamente em 1000, prazo de inscrição expirando, 7 pokémons (6 ativos + 1 ovo).

## 10. Documentação e entrega

- [ ] Comentários explicativos no código.
- [ ] Preencher a seção "Como executar" do README.
- [ ] Gravar vídeos individuais explicando a parte implementada por cada integrante.
- [ ] Publicar o repositório (GitHub) com código-fonte, instruções e links dos vídeos.

---

## Sugestão de divisão de trabalho (3 integrantes)

| Integrante | Foco sugerido | Operação de grafo sob responsabilidade |
|---|---|---|
| **Dev 1** | Grafo, leitura de arquivos, movimentação/simulação de tempo | Representação do grafo + algoritmo de busca (BFS/DFS) |
| **Dev 2** | Sistema de pokémon (XP, evolução, HP, ovos) e treinador | Algoritmo de caminho mínimo (Dijkstra) para deslocamento |
| **Dev 3** | Sistema de batalhas, insígnias, inscrição na Liga, elementos bônus | Lógica de movimentação/retorno periódico dos líderes itinerantes |

*(Ajustar conforme afinidade da equipe — o importante é que cada integrante fique claramente responsável por ao menos uma operação sobre o grafo, conforme exigido no enunciado.)*

## Ordem sugerida de implementação

1. Estruturas de dados + leitura do grafo e dos arquivos de entrada.
2. Algoritmos de grafo (busca e caminho mínimo).
3. Sistema de pokémon isolado (sem batalha).
4. Sistema de treinador isolado (sem batalha).
5. Sistema de batalhas (pokémon vs. pokémon → treinador vs. treinador → captura → líder de ginásio).
6. Sistema de insígnias e inscrição na Liga.
7. Elementos bônus (tipos e Equipe Rocket).
8. Testes, documentação e gravação dos vídeos.