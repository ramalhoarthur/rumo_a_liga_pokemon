#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "batalha.hpp"
#include "configuracao_regiao.hpp"
#include "jornada.hpp"
#include "lider_ginasio.hpp"
#include "regiao.hpp"

namespace {

Pokemon criarPokemon(const EspeciePokemon& especie, std::mt19937& gerador) {
    return Pokemon(especie, std::uniform_int_distribution<int>(20, 30)(gerador),
                   std::uniform_int_distribution<int>(20, 30)(gerador));
}

const EspeciePokemon& especieComTipo(const std::vector<EspeciePokemon>& especies,
                                     const std::string& tipo) {
    const auto encontrado = std::find_if(especies.begin(), especies.end(), [&tipo](const auto& especie) {
        const auto& tipos = especie.formas.front().tipos;
        return std::find(tipos.begin(), tipos.end(), tipo) != tipos.end();
    });
    if (encontrado == especies.end()) {
        throw std::runtime_error("O cenario precisa de uma especie do tipo " + tipo);
    }
    return *encontrado;
}

bool batalhasPermitidas(const Jornada& jornada) {
    const Vertice& local = jornada.mapa().vertice(jornada.jogador().posicao());
    return !local.temCmp() && !local.temLaboratorio();
}

bool localTemPMC(const Jornada& jornada) {
    return jornada.mapa().vertice(jornada.jogador().posicao()).temCmp();
}

int sortearStatusNoIntervalo(int referencia, int minimo, int maximo, std::mt19937& gerador) {
    const int limite_inferior = std::max(minimo, static_cast<int>(std::ceil(referencia * 0.8)));
    const int limite_superior = std::min(maximo, static_cast<int>(std::floor(referencia * 1.5)));
    return std::uniform_int_distribution<int>(limite_inferior,
                                              std::max(limite_inferior, limite_superior))(gerador);
}

// O selvagem e sorteado uma vez dentro dos limites do pokemon ativo com maior XP.
void balancearPokemonSelvagem(PokemonSelvagem& selvagem, const Treinador& jogador,
                              std::mt19937& gerador) {
    if (selvagem.atributos_balanceados || jogador.pokemonAtivos().empty()) return;

    const auto& equipe = jogador.pokemonAtivos();
    const auto mais_forte = std::max_element(equipe.begin(), equipe.end(),
                                              [](const Pokemon& esquerdo, const Pokemon& direito) {
                                                  return esquerdo.xp() < direito.xp();
                                              });
    const Pokemon& referencia = *mais_forte;
    const int xp = sortearStatusNoIntervalo(referencia.xp(), 0, std::numeric_limits<int>::max(), gerador);
    selvagem.pokemon.definirAtributosDeEncontro(
        sortearStatusNoIntervalo(referencia.ataque(), xp / 10 + 1,
                                  std::numeric_limits<int>::max(), gerador),
        sortearStatusNoIntervalo(referencia.defesa(), xp / 10 + 1,
                                  std::numeric_limits<int>::max(), gerador),
        sortearStatusNoIntervalo(referencia.hp(), 1, 100, gerador),
        xp);
    selvagem.atributos_balanceados = true;
}

// Aceita somente as duas formas permitidas de escolher a equipe inicial.
bool selecionarEquipeInicial(int& escolha) {
    for (;;) {
        std::cout << "Escolha a equipe inicial: 1) agua, fogo e grama  2) um pokemon aleatorio: ";
        if (!(std::cin >> escolha)) {
            if (std::cin.eof()) return false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Digite 1 ou 2.\n";
            continue;
        }
        if (escolha == 1 || escolha == 2) return true;
        std::cout << "Opcao invalida. Digite 1 ou 2.\n";
    }
}

// Mostra somente deslocamentos permitidos e repete a leitura ate receber um deles.
// Retorna false apenas quando a entrada e encerrada pelo usuario (EOF).
bool selecionarDestinoVizinho(const Jornada& jornada, int& destino) {
    const Grafo& mapa = jornada.mapa();
    const int origem = jornada.jogador().posicao();
    const auto& vizinhos = mapa.vizinhos(origem);

    if (vizinhos.empty()) {
        std::cout << "Nao ha vertices vizinhos para mover a partir deste local.\n";
        return false;
    }

    std::cout << "Vertices vizinhos disponiveis:\n";
    for (const Aresta& aresta : vizinhos) {
        const Vertice& vertice = mapa.vertice(aresta.destino);
        std::cout << "  " << aresta.destino << " - " << vertice.descobrirNomeVertice()
                  << " (distancia " << aresta.peso << ")\n";
    }

    for (;;) {
        std::cout << "Digite o ID de um dos vertices listados: ";
        if (!(std::cin >> destino)) {
            if (std::cin.eof()) return false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Digite o ID de um dos vertices listados.\n";
            continue;
        }

        const bool eh_vizinho = std::any_of(vizinhos.begin(), vizinhos.end(),
                                             [destino](const Aresta& aresta) {
                                                 return aresta.destino == destino;
                                             });
        if (eh_vizinho) return true;
        std::cout << "Destino invalido. Digite o ID de um dos vertices listados.\n";
    }
}

// Lista todos os vertices do mapa e aceita somente um ID existente.
bool selecionarVerticeDoMapa(const Grafo& mapa, int& destino) {
    std::cout << "Vertices disponiveis:\n";
    for (int id = 0; id < mapa.quantidadeVertices(); ++id) {
        const Vertice& vertice = mapa.vertice(id);
        std::cout << "  " << id << " - " << vertice.descobrirNomeVertice() << '\n';
    }

    for (;;) {
        std::cout << "Digite o ID do vertice de destino: ";
        if (!(std::cin >> destino)) {
            if (std::cin.eof()) return false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Digite o ID de um vertice listado.\n";
            continue;
        }
        if (destino >= 0 && destino < mapa.quantidadeVertices()) return true;
        std::cout << "Destino invalido. Digite o ID de um vertice listado.\n";
    }
}

void mostrarStatus(const Jornada& jornada) {
    const Treinador& jogador = jornada.jogador();
    const Vertice& local = jornada.mapa().vertice(jogador.posicao());
    std::cout << "\nLocal: " << local.descobrirNomeVertice() << " (" << local.descobrirIdVertice()
              << ")\nPrazo da Liga: ";
    if (jornada.prazoInscricaoIniciado()) {
        std::cout << jornada.tempoDecorrido() << '/' << jornada.prazoInscricao();
    } else {
        std::cout << "---";
    }
    std::cout << " | Insignias: " << jogador.quantidadeInsignias() << " | Ervas: " << jogador.ervas()
              << " | Pokebolas: " << jogador.pokebolasEquipe() + jogador.pokebolasCaptura()
              << "\nEquipe:\n";
    for (std::size_t i = 0; i < jogador.pokemonAtivos().size(); ++i) {
        const Pokemon& pokemon = jogador.pokemonAtivos()[i];
        std::cout << "  [" << i << "] " << pokemon.nome() << " - HP " << pokemon.hp() << "/100, XP "
                  << pokemon.xp() << ", AP " << pokemon.ataque() << ", DP " << pokemon.defesa() << '\n';
    }
}

bool haSelvagemNoLocal(const Regiao& regiao, const Jornada& jornada) {
    return std::any_of(regiao.selvagens().begin(), regiao.selvagens().end(), [&](const PokemonSelvagem& selvagem) {
        return selvagem.posicao == jornada.jogador().posicao();
    });
}

bool haTreinadorNoLocal(const Regiao& regiao, const Jornada& jornada) {
    return std::any_of(regiao.treinadores().begin(), regiao.treinadores().end(), [&](const Treinador& treinador) {
        return treinador.posicao() == jornada.jogador().posicao();
    });
}

void mostrarMenu(const Jornada& jornada, const Regiao& regiao) {
    std::cout << "\n1-Mover  2-Rota minima  3-Usar erva";
    if (batalhasPermitidas(jornada)) {
        if (haSelvagemNoLocal(regiao, jornada)) std::cout << "  4-Capturar";
        if (haTreinadorNoLocal(regiao, jornada)) std::cout << "  5-Duelo treinador";
        std::cout << "  6-Ginasio  9-Rocket";
    }
    std::cout << "  7-Inscricao";
    if (localTemPMC(jornada)) std::cout << "  8-PMC";
    std::cout << "  10-Status ovo  0-Sair\nOpcao: ";
}

// Exibe somente lideres que estao no vertice atual e valida a escolha.
bool selecionarLiderNoLocal(const Jornada& jornada, std::size_t& indice) {
    const std::vector<std::size_t> lideres_disponiveis = jornada.lideresNoLocalDoJogador();
    if (lideres_disponiveis.empty()) {
        std::cout << "Nao ha lider de ginasio neste local.\n";
        return false;
    }

    std::cout << "Lideres disponiveis neste local:";
    for (const std::size_t indice_disponivel : lideres_disponiveis) {
        std::cout << ' ' << indice_disponivel;
    }
    std::cout << "\n";

    for (;;) {
        std::cout << "Indice do lider: ";
        if (!(std::cin >> indice)) {
            if (std::cin.eof()) return false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Digite um dos indices listados.\n";
            continue;
        }
        if (std::find(lideres_disponiveis.begin(), lideres_disponiveis.end(), indice) !=
            lideres_disponiveis.end()) {
            return true;
        }
        std::cout << "Lider invalido. Digite um dos indices listados.\n";
    }
}

// Um pokemon inconsciente nao pode iniciar a batalha de captura.
bool selecionarPokemonConscienteParaCaptura(const Treinador& treinador, std::size_t& indice) {
    const auto& equipe = treinador.pokemonAtivos();
    std::vector<std::size_t> disponiveis;
    for (std::size_t i = 0; i < equipe.size(); ++i) {
        if (equipe[i].podeBatalhar()) disponiveis.push_back(i);
    }
    if (disponiveis.empty()) {
        std::cout << "Nao ha pokemon consciente disponivel para capturar.\n";
        return false;
    }

    std::cout << "Pokemons conscientes disponiveis:";
    for (const std::size_t indice_disponivel : disponiveis) std::cout << ' ' << indice_disponivel;
    std::cout << "\n";

    for (;;) {
        std::cout << "Indice do pokemon que enfrentara o selvagem: ";
        if (!(std::cin >> indice)) {
            if (std::cin.eof()) return false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Digite um dos indices listados.\n";
            continue;
        }
        if (std::find(disponiveis.begin(), disponiveis.end(), indice) != disponiveis.end()) return true;
        std::cout << "Pokemon invalido ou inconsciente. Digite um dos indices listados.\n";
    }
}

bool selecionarTresPokemon(Treinador& treinador) {
    if (!treinador.podeBatalharContraTreinador()) {
        std::cout << "Sao necessarios tres pokemons conscientes.\n";
        return false;
    }
    std::cout << "Escolha os tres pokemons para a batalha (indices): ";
    std::array<std::size_t, 3> escolhidos{};
    std::cin >> escolhidos[0] >> escolhidos[1] >> escolhidos[2];
    auto& equipe = treinador.pokemonAtivos();
    for (std::size_t indice : escolhidos) {
        if (indice >= equipe.size() || !equipe[indice].podeBatalhar() ||
            std::count(escolhidos.begin(), escolhidos.end(), indice) != 1) {
            std::cout << "Selecao invalida.\n";
            return false;
        }
    }

    std::vector<Pokemon> reorganizada;
    reorganizada.reserve(equipe.size());
    for (std::size_t indice : escolhidos) reorganizada.push_back(std::move(equipe[indice]));
    for (std::size_t i = 0; i < equipe.size(); ++i) {
        if (std::find(escolhidos.begin(), escolhidos.end(), i) == escolhidos.end()) {
            reorganizada.push_back(std::move(equipe[i]));
        }
    }
    equipe = std::move(reorganizada);
    return true;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        const std::string arquivo_cenario = argc > 1 ? argv[1] : "data/cenario.txt";
        const ConfiguracaoRegiao configuracao = ConfiguracaoRegiao::ler(arquivo_cenario);
        std::mt19937 gerador(std::random_device{}());
        Regiao regiao(configuracao, gerador());

        int laboratorio = -1;
        for (int id = 0; id < regiao.mapa().quantidadeVertices(); ++id) {
            if (regiao.mapa().vertice(id).temLaboratorio()) laboratorio = id;
        }
        if (laboratorio == -1) throw std::runtime_error("O mapa deve possuir o laboratorio do Professor Carvalho");

        std::cout << "Rumo a Liga Pokemon\nNome do treinador: ";
        std::string nome;
        std::getline(std::cin, nome);
        if (nome.empty()) nome = "Treinador";
        Treinador jogador(nome, laboratorio);

        int escolha;
        if (!selecionarEquipeInicial(escolha)) return 0;
        if (escolha == 2) {
            const auto& especie = configuracao.especies[
                std::uniform_int_distribution<std::size_t>(0, configuracao.especies.size() - 1)(gerador)];
            jogador.receberPokemon(criarPokemon(especie, gerador));
        } else {
            for (const char* tipo : {"AGUA", "FOGO", "GRAMA"}) {
                jogador.receberPokemon(criarPokemon(especieComTipo(configuracao.especies, tipo), gerador));
            }
        }

        Jornada jornada(Grafo(configuracao.caminho_mapa), std::move(jogador),
                         configuracao.prazo_inscricao, gerador());
        std::size_t quantidade_lideres = 0;
        for (int id = 0; id < jornada.mapa().quantidadeVertices(); ++id) {
            if (!jornada.mapa().vertice(id).temGinasio()) continue;
            Treinador lider("Lider " + std::to_string(++quantidade_lideres), id, 500);
            for (int i = 0; i < 3; ++i) lider.receberPokemon(criarPokemon(configuracao.especies[i % configuracao.especies.size()], gerador));
            jornada.adicionarLider(LiderGinasio(std::move(lider), "Insignia " + std::to_string(quantidade_lideres), id));
        }
        Treinador rocket("Equipe Rocket", 4, 700);
        for (int i = 0; i < 3; ++i) {
            rocket.receberPokemon(criarPokemon(configuracao.especies[(i + 1) % configuracao.especies.size()], gerador));
        }
        jornada.adicionarEquipeRocket(EquipeRocket(std::move(rocket)));

        for (;;) {
            mostrarStatus(jornada);
            mostrarMenu(jornada, regiao);
            int opcao;
            if (!(std::cin >> opcao)) {
                if (std::cin.eof()) break;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Opcao invalida. Digite uma opcao do menu.\n";
                continue;
            }
            if (opcao == 0) break;
            if (opcao < 0 || opcao > 10) {
                std::cout << "Opcao invalida. Digite uma opcao do menu.\n";
                continue;
            }

            if (opcao == 1) {
                int destino;
                if (!selecionarDestinoVizinho(jornada, destino)) {
                    if (std::cin.eof()) break;
                    continue;
                }
                jornada.moverJogador(destino);
                auto& ervas = regiao.ervas();
                const auto primeira_erva = std::remove_if(ervas.begin(), ervas.end(), [&](const ErvaRegional& erva) {
                    if (erva.posicao != jornada.jogador().posicao()) return false;
                    jornada.jogador().adicionarErva();
                    return true;
                });
                if (primeira_erva != ervas.end()) {
                    std::cout << "Voce encontrou ervas medicinais.\n";
                    ervas.erase(primeira_erva, ervas.end());
                }
                auto& ovos = regiao.ovos();
                const auto ovo_encontrado = std::find_if(ovos.begin(), ovos.end(), [&](const OvoRegional& ovo) {
                    return ovo.posicao == jornada.jogador().posicao();
                });
                if (ovo_encontrado != ovos.end()) {
                    std::cout << "Voce encontrou um ovo. Pegar? 1-Sim  2-Nao: ";
                    int pegar;
                    std::cin >> pegar;
                    if (pegar == 1) {
                        if (jornada.jogador().possuiOvo()) {
                            std::cout << "Voce ja possui um ovo nao chocado.\n";
                        } else {
                            jornada.jogador().coletarOvo(std::move(ovo_encontrado->ovo));
                            ovos.erase(ovo_encontrado);
                            std::cout << "Ovo colocado na incubadora.\n";
                        }
                    }
                }
                regiao.moverEntidadesUmaEtapa(gerador);
            } else if (opcao == 2) {
                int destino;
                if (!selecionarVerticeDoMapa(jornada.mapa(), destino)) {
                    if (std::cin.eof()) break;
                    continue;
                }
                const Caminho rota = jornada.mapa().caminhoMinimo(jornada.jogador().posicao(), destino);
                if (!rota.existe()) std::cout << "Destino inalcançavel.\n";
                else {
                    std::cout << "Distancia " << rota.distancia << ": ";
                    for (int id : rota.vertices) std::cout << id << ' ';
                    std::cout << '\n';
                }
            } else if (opcao == 3) {
                int pokemons_curados = 0;
                if (!jornada.jogador().usarErva(&pokemons_curados)) {
                    std::cout << "Sem ervas.\n";
                } else if (pokemons_curados == 0) {
                    std::cout << "Erva usada, mas nenhum pokemon consciente precisava de cura.\n";
                } else {
                    std::cout << "Erva usada. " << pokemons_curados << " pokemon(s) recuperaram 10 HP.\n";
                }
            } else if (opcao == 4) {
                if (!batalhasPermitidas(jornada)) {
                    std::cout << "Batalhas sao proibidas no PMC e no laboratorio.\n";
                    continue;
                }
                auto& selvagens = regiao.selvagens();
                std::vector<std::size_t> selvagens_no_local;
                for (std::size_t i = 0; i < selvagens.size(); ++i) {
                    if (selvagens[i].posicao == jornada.jogador().posicao()) {
                        selvagens_no_local.push_back(i);
                    }
                }
                if (selvagens_no_local.empty()) {
                    std::cout << "Nao ha pokemon selvagem neste local.\n";
                } else {
                    const std::size_t indice_selvagem = selvagens_no_local[
                        std::uniform_int_distribution<std::size_t>(0, selvagens_no_local.size() - 1)(gerador)];
                    PokemonSelvagem& selvagem = selvagens[indice_selvagem];
                    balancearPokemonSelvagem(selvagem, jornada.jogador(), gerador);
                    const Pokemon& oponente = selvagem.pokemon;
                    std::cout << "Pokemon selvagem encontrado: " << oponente.nome() << " - HP "
                              << oponente.hp() << "/100, XP " << oponente.xp() << ", AP "
                              << oponente.ataque() << ", DP " << oponente.defesa() << "\n";
                    std::size_t indice_pokemon;
                    if (!selecionarPokemonConscienteParaCaptura(jornada.jogador(), indice_pokemon)) {
                        if (std::cin.eof()) break;
                        continue;
                    }
                    const bool equipe_cheia = jornada.jogador().quantidadeAtivos() == 6;
                    Batalha batalha(gerador);
                    const bool capturado = batalha.capturar(jornada.jogador(),
                                                            jornada.jogador().pokemonAtivos()[indice_pokemon],
                                                            selvagem.pokemon);
                    jornada.jogador().passarTempo(1);
                    if (capturado) {
                        selvagens.erase(selvagens.begin() + static_cast<std::ptrdiff_t>(indice_selvagem));
                        if (equipe_cheia) {
                            std::cout << "Equipe cheia. Qual indice ativo sera enviado ao Professor? ";
                            std::size_t indice_ativo;
                            std::cin >> indice_ativo;
                            jornada.jogador().trocarAtivoComPokemonDoProfessor(
                                indice_ativo, jornada.jogador().quantidadeNoProfessor() - 1);
                        }
                        std::cout << "Pokemon capturado.\n";
                    } else std::cout << "O pokemon escapou.\n";
                }
            } else if (opcao == 5) {
                if (!batalhasPermitidas(jornada)) {
                    std::cout << "Batalhas sao proibidas no PMC e no laboratorio.\n";
                    continue;
                }
                auto& treinadores = regiao.treinadores();
                const auto encontrado = std::find_if(treinadores.begin(), treinadores.end(), [&](const Treinador& t) {
                    return t.posicao() == jornada.jogador().posicao();
                });
                if (encontrado == treinadores.end()) std::cout << "Nenhum treinador neste local.\n";
                else if (!selecionarTresPokemon(jornada.jogador())) continue;
                else {
                    Batalha batalha(gerador);
                    const auto resultado = batalha.treinadores(*encontrado, jornada.jogador());
                    jornada.jogador().passarTempo(1);
                    std::cout << (resultado.vencedor == VencedorDuelo::Desafiante ? "Voce venceu.\n" : "Voce perdeu.\n");
                }
            } else if (opcao == 6) {
                if (!batalhasPermitidas(jornada)) {
                    std::cout << "Batalhas sao proibidas no PMC e no laboratorio.\n";
                    continue;
                }
                std::size_t indice;
                if (!selecionarLiderNoLocal(jornada, indice)) {
                    if (std::cin.eof()) break;
                    continue;
                }
                if (!selecionarTresPokemon(jornada.jogador())) continue;
                std::cout << (jornada.desafiarLider(indice) ? "Insignia conquistada.\n" : "Derrota no ginasio.\n");
            } else if (opcao == 7) {
                std::cout << (jornada.podeInscreverNaLiga() ? "Inscricao realizada. Boa sorte na Liga!\n"
                                                            : "Ainda nao e possivel se inscrever.\n");
            } else if (opcao == 8) {
                if (!localTemPMC(jornada)) {
                    std::cout << "O tratamento so pode ser realizado no Centro Medico Pokemon.\n";
                    continue;
                }
                std::cout << "Tratamento concluido apos " << jornada.tratarEquipeNoPMC()
                          << " unidades de tempo.\n";
            } else if (opcao == 9) {
                if (!batalhasPermitidas(jornada)) {
                    std::cout << "Batalhas sao proibidas no PMC e no laboratorio.\n";
                } else if (!jornada.equipeRocketEstaNaPosicaoDoJogador()) {
                    std::cout << "A Equipe Rocket nao esta neste local.\n";
                } else if (!selecionarTresPokemon(jornada.jogador())) {
                    continue;
                } else {
                    const auto resultado = jornada.enfrentarEquipeRocket();
                    if (resultado.vencedor == VencedorDuelo::Desafiante) {
                        std::cout << "Voce derrotou a Equipe Rocket. Ela foi enviada para longe.\n";
                    } else if (resultado.pokemon_roubado) {
                        std::cout << "A Equipe Rocket venceu e roubou um pokemon.\n";
                    } else if (!resultado.insignia_roubada.empty()) {
                        std::cout << "A Equipe Rocket venceu e roubou a insignia "
                                  << resultado.insignia_roubada << ".\n";
                    } else {
                        std::cout << "A Equipe Rocket venceu, mas nao encontrou nada para roubar.\n";
                    }
                }
            } else if (opcao == 10) {
                const std::optional<int> distancia_restante = jornada.jogador().distanciaOvoRestante();
                if (!distancia_restante) {
                    std::cout << "Nao ha ovo na incubadora.\n";
                } else {
                    std::cout << "Faltam " << *distancia_restante
                              << " unidades de distancia para o ovo chocar.\n";
                }
            }
        }
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
    return 0;
}
