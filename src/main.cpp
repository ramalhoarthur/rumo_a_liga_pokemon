#include <algorithm>
#include <array>
#include <iostream>
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

void mostrarStatus(const Jornada& jornada) {
    const Treinador& jogador = jornada.jogador();
    const Vertice& local = jornada.mapa().vertice(jogador.posicao());
    std::cout << "\nLocal: " << local.descobrirNomeVertice() << " (" << local.descobrirIdVertice()
              << ")\nTempo: " << jornada.tempoDecorrido() << '/' << jornada.prazoInscricao()
              << " | Insignias: " << jogador.quantidadeInsignias() << " | Ervas: " << jogador.ervas()
              << " | Pokebolas: " << jogador.pokebolasEquipe() + jogador.pokebolasCaptura()
              << "\nEquipe:\n";
    for (std::size_t i = 0; i < jogador.pokemonAtivos().size(); ++i) {
        const Pokemon& pokemon = jogador.pokemonAtivos()[i];
        std::cout << "  [" << i << "] " << pokemon.nome() << " - HP " << pokemon.hp() << "/100, XP "
                  << pokemon.xp() << ", AP " << pokemon.ataque() << ", DP " << pokemon.defesa() << '\n';
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

        std::cout << "Escolha a equipe inicial: 1) agua, fogo e grama  2) um pokemon aleatorio: ";
        int escolha = 1;
        std::cin >> escolha;
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
            std::cout << "\n1-Mover  2-Rota minima  3-Usar erva  4-Capturar  5-Duelo treinador"
                         "  6-Ginasio  7-Inscricao  8-PMC  9-Rocket  0-Sair\nOpcao: ";
            int opcao;
            if (!(std::cin >> opcao)) break;
            if (opcao == 0) break;

            if (opcao == 1) {
                std::cout << "Vertice de destino: ";
                int destino;
                std::cin >> destino;
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
                std::cout << "Vertice de destino: ";
                int destino;
                std::cin >> destino;
                const Caminho rota = jornada.mapa().caminhoMinimo(jornada.jogador().posicao(), destino);
                if (!rota.existe()) std::cout << "Destino inalcançavel.\n";
                else {
                    std::cout << "Distancia " << rota.distancia << ": ";
                    for (int id : rota.vertices) std::cout << id << ' ';
                    std::cout << '\n';
                }
            } else if (opcao == 3) {
                std::cout << (jornada.jogador().usarErva() ? "Erva usada.\n" : "Sem ervas.\n");
            } else if (opcao == 4) {
                if (!batalhasPermitidas(jornada)) throw std::logic_error("Batalhas sao proibidas neste local");
                auto& selvagens = regiao.selvagens();
                const auto encontrado = std::find_if(selvagens.begin(), selvagens.end(), [&](const auto& selvagem) {
                    return selvagem.posicao == jornada.jogador().posicao();
                });
                if (encontrado == selvagens.end()) {
                    std::cout << "Nao ha pokemon selvagem neste local.\n";
                } else if (jornada.jogador().pokemonAtivos().empty()) {
                    std::cout << "Nao ha pokemon disponivel.\n";
                } else {
                    std::cout << "Indice do pokemon que enfrentara o selvagem: ";
                    std::size_t indice_pokemon;
                    std::cin >> indice_pokemon;
                    if (indice_pokemon >= jornada.jogador().pokemonAtivos().size()) {
                        std::cout << "Indice invalido.\n";
                        continue;
                    }
                    const bool equipe_cheia = jornada.jogador().quantidadeAtivos() == 6;
                    Batalha batalha(gerador);
                    const bool capturado = batalha.capturar(jornada.jogador(),
                                                            jornada.jogador().pokemonAtivos()[indice_pokemon],
                                                            encontrado->pokemon);
                    jornada.jogador().passarTempo(1);
                    if (capturado) {
                        selvagens.erase(encontrado);
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
                if (!batalhasPermitidas(jornada)) throw std::logic_error("Batalhas sao proibidas neste local");
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
                std::cout << "Indice do lider (0 a " << (quantidade_lideres - 1) << "): ";
                std::size_t indice;
                std::cin >> indice;
                if (!selecionarTresPokemon(jornada.jogador())) continue;
                std::cout << (jornada.desafiarLider(indice) ? "Insignia conquistada.\n" : "Derrota no ginasio.\n");
            } else if (opcao == 7) {
                std::cout << (jornada.podeInscreverNaLiga() ? "Inscricao realizada. Boa sorte na Liga!\n"
                                                            : "Ainda nao e possivel se inscrever.\n");
            } else if (opcao == 8) {
                std::cout << "Tratamento concluido apos " << jornada.tratarEquipeNoPMC()
                          << " unidades de tempo.\n";
            } else if (opcao == 9) {
                if (!jornada.equipeRocketEstaNaPosicaoDoJogador()) {
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
            }
        }
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
    return 0;
}
