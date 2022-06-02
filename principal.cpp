// principal.cpp
// Programa principal do jogo Centipede.
//
// The MIT License (MIT)
//
// Copyright (c) 2017 João Vicente Ferreira Lima, UFSM
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <chrono>
#include <list>
#include <random>
#include <iostream>
#include <functional>
#include "tela.hpp"
#include "geom.hpp"

using namespace tela;
using namespace geom;

// programa de teste das funcoes geometricas
//
// o programa desenha algumas figuras
// se o mouse passar sobre alguma figura, ela muda de cor
// (se as funcoes ptemret e ptemcirc estiverem corretas)
// pode-se arrastar as figuras com o mouse (botao esquerdo pressionado)
//
// com o botao esquerdo pressionado, as figuras com intersecao mudam de cor
// (se as funcoes de intersecao estiverem funcionando)
//
// apertando a tecla q o programa termina

struct laser_t {
  float v;       /* velocidade */
  Retangulo ret; /* figura */
};

struct tiro_t {
  bool ativo;
  float v;   /* velocidade */
  Circulo  c; /* figura */
};

/* estados para o jogo */
enum Estado { nada, fim };

// Estrutura para controlar todos os objetos e estados do Jogo Centipede
struct Jogo {
  Estado estado;             // estado do jogo
  laser_t laser;             // laser
  std::list<tiro_t> tiros;   // tiros ativos
  std::list<Retangulo> centipede;  // blocos de centopeia
  Tela tela;                    // estrutura que controla a tela
  int tecla;                 // ultima tecla apertada pelo usuario

  // inicia estruturas principais do jogo
  void inicia(void) {
    tela.inicia(600, 400, "Centipede!");
    laser_inicia();
    estado = Estado::nada;

    // inicia jogo com apenas 1 retangulo
    Retangulo r1 = {{100, 30}, {40, 40}};
    centipede.push_back(r1);
  }

  // atualiza o jogo, como se fosse um passo na execução
  void atualiza(void) {
    // le ultima tecla
    tecla = tela.tecla();
    // tecla Q termina
    if (tecla != ALLEGRO_KEY_Q) {
      // faz o resto
      move_figuras();
      tela.limpa();
      desenha_figuras();
      tela.mostra();
      // espera 60 ms antes de atualizar a tela
      tela.espera(30);
    } else {
      // troca estado do jogo para terminado
      estado = Estado::fim;
    }
  }

  // verifica se o jogo terminou ou não
  // - retorna TRUE quando  termina
  // - retorna FALSE caso contrário
  bool verifica_fim(void) {
    if (estado == Estado::nada)
      return false;
    else
      return true;
  }

  void finaliza(void) {
    // fecha a tela
    tela.finaliza();
  }

  // move o tiro (se existir) em certa velocidade
  void tiro_movimenta(void) {
    for(auto t = tiros.begin(); t != tiros.end(); /* nao precisa aqui */){
      (*t).c.centro.y -= (*t).v;
      /* saiu da tela */
      if ((*t).c.centro.y < 0.0) {
        t = tiros.erase(t);
      } else
        t++;
    }
  }

  // lanca tiro do laser
  void tiro_fogo(void) {
    tiro_t tiro;
    tiro.ativo = true;
    tiro.v = 20;
    tiro.c.centro.x = laser.ret.pos.x;
    tiro.c.centro.y = laser.ret.pos.y - 10;
    tiro.c.raio = 5;
    tiros.push_back(tiro);
  }

 // desenha o tiro, sendo que a cada desenha sua cor muda.
  void tiro_desenha(void) {
    if (tiros.empty() == false) {
      for(auto t = tiros.begin(); t != tiros.end(); t++){
        Cor outra = {1.0, 0.0, 0.0};
        tela.cor(outra);
        tela.circulo((*t).c);
      }
    }
  }

 // inicia o laser na posição inicial
  void laser_inicia(void) {
    Tamanho tam = tela.tamanho();
    laser.v = 0.0;
    laser.ret.pos.x = tam.larg / 2;
    laser.ret.pos.y = tam.alt - 20;
    laser.ret.tam.larg = 10;
    laser.ret.tam.alt = 20;
  }

  // laser lança um tiro
  void laser_atira() {
    if (tecla == ALLEGRO_KEY_F) {
      tiro_fogo();
    }
  }

  // altera velocidade do laser com as teclas A ou D
  void laser_altera_velocidade() {
    if (tecla == ALLEGRO_KEY_A) {
      /* altera velocidade mais a esquerda */
      laser.v -= 1;
    } else if (tecla == ALLEGRO_KEY_D) {
      /* altera velocidade mais a direita */
      laser.v += 1;
    }
  }

  // apenas movimenta o laser na tela se necessário
  void laser_move(void) {
    Tamanho tam = tela.tamanho();
    if (((laser.ret.pos.x + laser.v) < 0.0) ||
        ((laser.ret.pos.x + laser.v) > (tam.larg - 10)))
      laser.v = 0.0;
    laser.ret.pos.x += laser.v;
  }

  // desenha o laser
  void laser_desenha(void) {
    Cor vermelho = {1.0, 0.0, 0.0};
    tela.cor(vermelho);
    tela.retangulo(laser.ret);
  }

  // desenha todas as figuras e objetos na tela
  void desenha_figuras() {
    Cor azul = {0.2, 0.3, 0.8};
    // Cor vermelho = {1, 0.2, 0};
    // Cor verde = {0.2, 0.9, 0.6};
    // Cor preto = {0, 0, 0};

    // mudar cor da tela
    tela.cor(azul);
    // desenha os objetos do vetor
    for (Retangulo &ret : centipede) {
      tela.retangulo(ret);
    }

    // desenha laser e tiro
    laser_altera_velocidade();
    laser_atira();
    laser_desenha();
    tiro_desenha();
  }

  void tiro_verifica_interceptacao(void){
    // testa por uma colisão entre objetos e o tiro
    if (tiros.empty() == false) {
      for( auto t = tiros.begin(); t != tiros.end(); t++ ) {
        for (auto ret = centipede.begin(); ret != centipede.end(); /* nao precisa aqui*/ ) {
          if (intercr((*t).c, *ret))
            // remove este objeto do vetor, retorna o proximo
            ret = centipede.erase(ret);
          else
            // incrementa para o proximo elemento
            ret++;
        } // for centipede
      } // for tiros
    } // if tiros
  }

  // Esta função tem os seguintes passos:
  // 1 - movimenta todos os objetos do centipede
  // 2 - movimenta o laser e tiro
  // 3 - testa por colisões entre objetos e tiro do laser
  void move_figuras(void) {
    float vel = 5;
    Tamanho tam = tela.tamanho();

    // movimenta todos os retangulos
    for (auto ret = centipede.begin(); ret != centipede.end(); ret++) {
      if ( (((*ret).pos.x + vel) < 0.0) || (((*ret).pos.x + vel) > (tam.larg - 10)) ) {
        vel = 0.0;
        (*ret).pos.x = 0.0;
        (*ret).pos.y += (*ret).tam.alt;
      }
      (*ret).pos.x += vel;
    }

    // move o laser
    laser_move();
    // movimenta tiros
    tiro_movimenta();
    // verifica se algum tiro pegou um bloco
    tiro_verifica_interceptacao();
  }

  void legenda(void){
    std::cout << "Pressione: " << std::endl;
    std::cout << " - 'a' ou 'd' para mover " << std::endl;
    std::cout << " - 'f' para atirar " << std::endl;
    std::cout << " - 'q' sair" << std::endl;
  }
};

int main(int argc, char **argv) {
  Jogo jogo;

  jogo.inicia();
  jogo.legenda();
  while (jogo.verifica_fim() == false) {
    jogo.atualiza();
  }
  jogo.finaliza();
  return 0;
}
