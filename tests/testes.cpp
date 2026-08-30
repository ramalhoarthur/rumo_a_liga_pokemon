#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>

#include "batalha.hpp"
#include "configuracao_regiao.hpp"
#include "jornada.hpp"
#include "regiao.hpp"
#include "treinador.hpp"
#include "vantagens_tipo.hpp"

namespace {

void exigir(bool condicao, const char* mensagem) {
    if (!condicao) throw std::runtime_error(mensagem);
}

EspeciePokemon especie(const char* nome) { return {{{nome, {"NORMAL"}}}}; }

void adicionarTresPokemon(Treinador& treinador, int atributo) {
    for (int i = 0; i < 3; ++i) treinador.receberPokemon(Pokemon(especie("Teste"), atributo, atributo));
}

void testarGrafo() {
    Grafo mapa("data/entrada.txt");
    exigir(mapa.quantidadeVertices() == 11 && mapa.quantidadeArestas() == 16, "Tamanho do mapa incorreto");
    exigir(mapa.somaPesosArestas() == 126 && mapa.existeCaminho(0, 10), "Grafo nao conectado");
    const Caminho caminho = mapa.caminhoMinimo(0, 10);
    exigir(caminho.existe() && caminho.distancia == 54, "Dijkstra retornou distancia incorreta");
}

void testarVantagensDeTipo() {
    exigir(std::fabs(TabelaVantagensTipo::multiplicador(TipoPokemon::FOGO, TipoPokemon::PLANTA) - 1.5F) < 0.001F,
           "Fogo deveria ter vantagem contra Planta");
    exigir(std::fabs(TabelaVantagensTipo::multiplicador(TipoPokemon::FOGO, TipoPokemon::AGUA) - 0.5F) < 0.001F,
           "Fogo deveria ter desvantagem contra Agua");
    exigir(std::fabs(TabelaVantagensTipo::multiplicador(TipoPokemon::NORMAL, TipoPokemon::NORMAL) - 1.0F) < 0.001F,
           "Tipos sem relacao devem ter multiplicador neutro");
    exigir(std::fabs(TabelaVantagensTipo::multiplicador({"PLANTA"}, {"AGUA", "PEDRA"}) - 2.25F) < 0.001F,
           "Tipos duplos devem combinar multiplicadores");
    exigir(std::fabs(TabelaVantagensTipo::multiplicador(TipoPokemon::DRAGAO, TipoPokemon::DRAGAO) - 1.5F) < 0.001F,
           "A vantagem de Dragao contra Dragao deve prevalecer");

    std::mt19937 gerador(31);
    Batalha batalha(gerador);
    Pokemon fogo({{{"Fogo", {"FOGO"}}}}, 30, 100);
    Pokemon planta({{{"Planta", {"PLANTA"}}}}, 1, 10);
    Pokemon normal({{{"Normal", {"NORMAL"}}}}, 30, 100);
    Pokemon normal_defensor({{{"Normal", {"NORMAL"}}}}, 1, 10);
    const ResultadoDuelo com_vantagem = batalha.duelo(planta, 0, fogo, 0);
    const ResultadoDuelo neutro = batalha.duelo(normal_defensor, 0, normal, 0);
    exigir(com_vantagem.turnos < neutro.turnos, "Multiplicador de tipo nao foi aplicado ao dano da batalha");
}

void testarPokemonETreinador() {
    EspeciePokemon evolutiva = {{{"A", {"NORMAL"}}, {"B", {"NORMAL"}}}};
    Pokemon pokemon(evolutiva, 10, 10, 100, 990);
    pokemon.ganharXp(10);
    exigir(pokemon.fase() == 2, "Pokemon nao evoluiu ao atingir 1000 XP");
    pokemon.sofrerDano(90, 10);
    exigir(!pokemon.podeBatalhar(), "Pokemon inconsciente ainda pode batalhar");

    Treinador treinador("Red", 0);
    treinador.receberPokemon(Pokemon(especie("P"), 10, 10));
    exigir(treinador.pokebolasEquipe() == 6 && treinador.pokebolasCaptura() == 1,
           "Inventario inicial de pokebolas incorreto");
    treinador.coletarOvo(OvoPokemon(especie("Ovo"), 10, 10));
    Grafo mapa("data/entrada.txt");
    for (int i = 0; i < 20; ++i) treinador.moverParaVizinho(mapa, i % 2 == 0 ? 1 : 0);
    exigir(treinador.quantidadeAtivos() == 2 && !treinador.possuiOvo(), "Ovo nao chocou apos 100 unidades");

    Treinador limite("Limite", 0);
    for (int i = 0; i < 7; ++i) limite.receberPokemon(Pokemon(especie("P"), 10 + i, 10));
    exigir(limite.quantidadeAtivos() == 6 && limite.quantidadeNoProfessor() == 1,
           "Excedente nao foi enviado ao Professor");
    limite.trocarAtivoComPokemonDoProfessor(0, 0);
    exigir(limite.pokemonAtivos().front().ataque() == 16, "Troca com o Professor falhou");
}

void testarBatalhaEJornada() {
    std::mt19937 gerador(7);
    Batalha batalha(gerador);
    Pokemon forte(especie("Forte"), 100, 100);
    Pokemon fraco(especie("Fraco"), 1, 1);
    exigir(batalha.duelo(fraco, 0, forte, 0).vencedor == VencedorDuelo::Desafiante,
           "Duelo nao reconheceu pokemon mais forte");

    Treinador jogador("Jogador", 4);
    Treinador lider("Lider", 4);
    adicionarTresPokemon(jogador, 100);
    adicionarTresPokemon(lider, 1);
    Jornada jornada(Grafo("data/entrada.txt"), std::move(jogador), 1260, 7);
    jornada.adicionarLider(LiderGinasio(std::move(lider), "PEDRA", 4));
    exigir(jornada.desafiarLider(0) && jornada.jogador().possuiInsignia("PEDRA"),
           "Insignia nao foi concedida");

    Treinador ferido("Ferido", 1);
    ferido.receberPokemon(Pokemon(especie("F"), 10, 10, 3));
    Jornada pmc(Grafo("data/entrada.txt"), std::move(ferido), 1260, 7);
    pmc.tratarEquipeNoPMC();
    exigir(pmc.jogador().pokemonAtivos().front().hp() == 100, "PMC nao recuperou HP");
}

void testarCenario() {
    const ConfiguracaoRegiao configuracao = ConfiguracaoRegiao::ler("data/cenario.txt");
    Regiao regiao(configuracao, 7);
    exigir(regiao.selvagens().size() == 12 && regiao.treinadores().size() == 4 &&
               regiao.ervas().size() == 6 && regiao.ovos().size() == 2,
           "Entidades regionais nao foram geradas");
}

void testarJornadaCompletaAteLiga() {
    Treinador jogador("Campeao", 2);
    adicionarTresPokemon(jogador, 100);
    Jornada jornada(Grafo("data/entrada.txt"), std::move(jogador), 1260, 11);

    const std::vector<int> ginasios = {2, 3, 4, 5, 6, 7, 8, 9};
    for (int posicao : ginasios) {
        Treinador lider("Lider", posicao);
        adicionarTresPokemon(lider, 1);
        jornada.adicionarLider(LiderGinasio(std::move(lider), "I" + std::to_string(posicao), posicao));
    }
    for (std::size_t i = 0; i < ginasios.size(); ++i) {
        const Caminho rota = jornada.mapa().caminhoMinimo(jornada.jogador().posicao(), ginasios[i]);
        for (std::size_t passo = 1; passo < rota.vertices.size(); ++passo) {
            jornada.moverJogador(rota.vertices[passo]);
        }
        exigir(jornada.desafiarLider(i), "O jogador deveria vencer o lider fraco");
    }
    const Caminho rota_liga = jornada.mapa().caminhoMinimo(jornada.jogador().posicao(), 10);
    for (std::size_t passo = 1; passo < rota_liga.vertices.size(); ++passo) {
        jornada.moverJogador(rota_liga.vertices[passo]);
    }
    exigir(jornada.jogador().quantidadeInsignias() == 8 && jornada.podeInscreverNaLiga(),
           "Fluxo completo ate a Liga falhou");
}

void testarEquipeRocket() {
    Treinador jogador("Red", 4);
    Treinador rocket("Rocket", 4);
    adicionarTresPokemon(jogador, 100);
    adicionarTresPokemon(rocket, 1);
    Jornada jornada(Grafo("data/entrada.txt"), std::move(jogador), 1260, 19);
    jornada.adicionarEquipeRocket(EquipeRocket(std::move(rocket)));
    exigir(jornada.equipeRocketEstaNaPosicaoDoJogador(), "Rocket deveria estar no local do jogador");
    const auto resultado = jornada.enfrentarEquipeRocket();
    exigir(resultado.vencedor == VencedorDuelo::Desafiante, "Jogador forte deveria derrotar Rocket");
    exigir(!jornada.equipeRocketEstaNaPosicaoDoJogador(), "Rocket derrotada deveria ser reposicionada para longe");

    Treinador fraco("Fraco", 4);
    Treinador rocket_forte("Rocket", 4);
    adicionarTresPokemon(fraco, 1);
    adicionarTresPokemon(rocket_forte, 100);
    fraco.receberInsignia("TESTE");
    Jornada derrota(Grafo("data/entrada.txt"), std::move(fraco), 1260, 23);
    derrota.adicionarEquipeRocket(EquipeRocket(std::move(rocket_forte)));
    const auto roubo = derrota.enfrentarEquipeRocket();
    exigir(roubo.vencedor == VencedorDuelo::Desafiado, "Rocket forte deveria vencer");
    exigir(roubo.pokemon_roubado || !roubo.insignia_roubada.empty(), "Rocket deveria roubar premio apos vencer");
    exigir(derrota.equipeRocket()->estaInvisivel(), "Rocket vencedora deveria ficar invisivel");
}

} // namespace

int main() {
    try {
        testarGrafo();
        testarVantagensDeTipo();
        testarPokemonETreinador();
        testarBatalhaEJornada();
        testarCenario();
        testarJornadaCompletaAteLiga();
        testarEquipeRocket();
        std::cout << "Todos os testes passaram.\n";
    } catch (const std::exception& erro) {
        std::cerr << "Falha: " << erro.what() << '\n';
        return 1;
    }
}
