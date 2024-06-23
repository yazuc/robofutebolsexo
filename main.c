#include <math.h>
#include <chipmunk.h>
#include <SOIL.h>
#include <stdbool.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Funções para movimentação de objetos
//Atacantes
void moveRobo(cpBody* body, void* data);
void moveRoboLower(cpBody* body, void* data);
void moveRoboRed(cpBody* body, void* data);
void moveRoboLowerRed(cpBody* body, void* data);

//Goleiros
void moveRoboGoleiro(cpBody* body, void* data);
void moveRoboGoleiroRed(cpBody* body, void* data);
//Zagueiros azul
void moveRoboZagueiroSup(cpBody* body, void* data);
void moveRoboZagueiroCen(cpBody* body, void* data);
void moveRoboZagueiroInf(cpBody* body, void* data);
//Zagueiros Vermelho
void moveRoboZagueiroSupDir(cpBody* body, void* data);
void moveRoboZagueiroCenDir(cpBody* body, void* data);
void moveRoboZagueiroInfDir(cpBody* body, void* data);
//bola.
void moveBola(cpBody* body, void* data);
bool isBallinGoleira1();
bool isBallOutOfBounds();
bool vetorContido(cpVect v, cpVect a, cpVect b);
//reposicionamento da bola
void resetaBall(cpBody* vect, float x, float y);
//função de movimento para retornar a posição especificada
void retornaJogadorOrigem(cpBody* body, cpVect pontoOrigem);
//chute a gol
void impulsionaBola(cpBody* body, cpVect goleira);

// Prototipos
void initCM();
void freeCM();
void restartCM();
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast);
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast);

// Score do jogo
int score1 = 0;
int score2 = 0;

// Flag de controle: 1 se o jogo tiver acabado
int gameOver = 0;

// cpVect e' um vetor 2D e cpv() e' uma forma rapida de inicializar ele.
cpVect gravity;

// O ambiente
cpSpace* space;

// Paredes "invisíveis" do ambiente
cpShape* leftWall, *rightWall, *topWall, *bottomWall;

cpBody* robotBody;
// A bola
cpBody* ballBody;

//Zagueiro vermelho
cpBody* Pyra;

//Zagueiro vermelho
cpBody* Mythra;

//Zagueiro vermelho
cpBody* Nia;

// Atacante vermelho
cpBody* Rex;

// Atacante vermelho
cpBody* RexPequeno;

// Goleiro Vermelho
cpBody* Glimmer;

// Atacante azul
cpBody* Matthew;

// Atacante azul
cpBody* Dunban;

// Zagueiro Azul
cpBody* Shulk;

// Zagueiro azul
cpBody* Nikol;

// Zagueiro azul
cpBody* Fiora;

// Goleiro Azul
cpBody* A;

const int distanciaDoAtacante = 150;

// Cada passo de simulação é 1/60 seg.
cpFloat timeStep = 1.0/60.0;

//const cpVect OrigemMatthew = cpv(480,440);
//cpVect OrigemRex = cpv(550,440);


// Inicializa o ambiente: é chamada por init() em opengl.c, pois necessita do contexto
// OpenGL para a leitura das imagens
void initCM()
{
    gravity = cpv(0, 100);

    // Cria o universo
    space = cpSpaceNew();

    // Seta o fator de damping, isto é, de atrito do ar
    cpSpaceSetDamping(space, 0.8);

    // Descomente a linha abaixo se quiser ver o efeito da gravidade!
    //cpSpaceSetGravity(space, gravity);

    // Adiciona 4 linhas estáticas para formarem as "paredes" do ambiente
    leftWall   = newLine(cpv(0,0), cpv(0,ALTURA_JAN), 0, 1.0);
    rightWall  = newLine(cpv(LARGURA_JAN,0), cpv(LARGURA_JAN,ALTURA_JAN), 0, 1.0);
    bottomWall = newLine(cpv(0,0), cpv(LARGURA_JAN,0), 0, 1.0);
    topWall    = newLine(cpv(0,ALTURA_JAN), cpv(LARGURA_JAN,ALTURA_JAN), 0, 1.0);

    // Agora criamos a bola...
    // Os parâmetros são:
    //   - posição: cpVect (vetor: x e y)
    //   - raio
    //   - massa
    //   - imagem a ser carregada
    //   - ponteiro para a função de movimentação (chamada a cada passo, pode ser NULL)
    //   - coeficiente de fricção
    //   - coeficiente de elasticidade
    
    //cpv(512,350) meio do campo
    //caso bola dentro da área de x <= 44 e y1 <= 380 e y2 >= 325 gol time 2
    // x em 980 e pos y1 = 380, y2=325 score2 
    //caso bola dentro da área de x <= 980 e y1 <= 380 e y2 >= 325 gol time 1
    // x em 44 e pos y1 = 380, y2=325 score1

    ballBody = newCircle(cpv(512,350), 8, 2, "small_football.png", moveBola, 0.2, 2);


    //----------*ROBÔS DE TESTE*------------
    // Em desenvolvimento utilizei estes dois robos para descobrir os posicionamentos exatos dos bodys que eu estava testando
    // Também usei para descobrir áreas limites de cada jogador.
    //robotBody = newCircle(cpv(1000,600), 5, 5, "ship1.png", NULL, 0.2, 0.5);
    // Shulk = newCircle(cpv(460,420), 5, 5, "ship1.png", NULL, 0.2, 0.5);


    //----------*POSIÇÕES INICIAIS VERMELHO E RESPECTIVAS FUNÇÕES*-----------
    
    //Pyra e Mythra são zagueiras
    Pyra = newCircle(cpv(830,180), 20, 5, "pyra.png", moveRoboZagueiroSupDir, 0.2, 0.5);
    Nia = newCircle(cpv(800, 350), 20, 5, "Nia.png", moveRoboZagueiroCenDir, 0.2, 0.5);
    Mythra = newCircle(cpv(830,530), 20, 5, "mythra.png", moveRoboZagueiroInfDir, 0.2, 0.5);

    // Rex Adulto e Rex Criança são atacantes
    RexPequeno = newCircle(cpv(565,290), 20, 5, "rex_pequeno.png", moveRoboRed, 0.2, 0.5);
    Rex = newCircle(cpv(565,420), 20, 5, "RoundRex_icon.png", moveRoboLowerRed, 0.2, 0.5);
    
    // Glimmer é Goleira
    Glimmer = newCircle(cpv(930,350), 10, 5, "glimmer.png", moveRoboGoleiroRed, 0.2, 0.5);
    
    // //----------*POSIÇÕES INICIAIS AZUL E RESPECTIVAS FUNÇÕES*-----------
    
    //Matthew e Dunban são atacantes
    Dunban = newCircle(cpv(460,290), 20, 5, "dunban.png", moveRobo, 0.2, 0.5);
    Matthew = newCircle(cpv(460,420), 20, 5, "roundMatthe_icon.png", moveRoboLower, 0.2, 0.5);

    //Shulk, Nikol e Fiora são zagueiros
    Nikol = newCircle(cpv(190,180), 20, 5, "rounded_nikol.png", moveRoboZagueiroSup, 0.2, 0.5);
    Fiora = newCircle(cpv(225,350), 20, 5, "fiora.png", moveRoboZagueiroCen, 0.2, 0.5);
    Shulk = newCircle(cpv(190,530), 20, 5, "RoundShulk_icon.png", moveRoboZagueiroInf, 0.2, 0.5);
    // A será goleira
    A = newCircle(cpv(100,350), 10, 5, "RoundA_icon.png", moveRoboGoleiro, 0.2, 0.5);    
}

void moveRobo(cpBody* body, void* data)
{

    // Limite da velocidade do robô, usei velocidade aleatória pra poder ter mais chances de os dois times poderem fazer gols
    const float limiteVelocidade = (10 + rand() % 21);
    const float fatorDrible = (rand() % 9 - 3);    
    // Limite do impulso
    const float limiteImpulso = 5.0;

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, limiteVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect goleiraDireita = cpv(980, 350);

    // Calcula a distância entre o robô e a bola
    cpFloat distanciaDaBola = cpvdist(robotPos, ballPos);

    // Verifica se a bola está no meio de campo ou no campo adversário
    cpVect delta = cpvzero;

    
    if(ballPos.x >= 512){
        if(vetorContido(ballPos, cpv(512,350), cpv(1000, 0))){   
            //faz a checagm em ballPos.x pra ele poder passar por trás da bola antes de voltar a tragetória
            if (ballPos.x < robotPos.x )
            {
                // Se a bola está atrás do robô, calcular vetor para contornar a bola
                // Calcula um vetor do robô à bola (DELTA = B - R)
                retornaJogadorOrigem(body, cpv(460,290));
            }
            else
            {
                // Calcula um vetor do robô à bola (DELTA = B - R)
                delta = cpvsub(ballPos, robotPos);

                // Limita o impulso
                delta = cpvmult(cpvnormalize(delta), limiteImpulso);
                //funciona como debug para o goleiro vermelho impulsionaBola(ballBody, goleiraDireita);       
                
                if(robotPos.x >= 600){
                    if(distanciaDaBola <= 40){
                        //printf("impulsionou em direcao a goleira");
                        impulsionaBola(ballBody, goleiraDireita);                    
                    }
                }

                // Aplica impulso no robô para mover em direção à bola
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }
        }else
        {
            // caso a bola se encontre dentro na área de atacante superior, o jogador se move até uma área favoravel para pegar a bola e fazer o gol        
            retornaJogadorOrigem(body, cpv(700,180));
        }
    }else
    {
        // Se a bola está do lado esquerdo do campo o jogador fica em sua posição meio campo para receber a bola       
        retornaJogadorOrigem(body, cpv(460,290));
    }
}

void moveRoboLower(cpBody* body, void* data)
{

    // Limite da velocidade do robô
    const float limiteVelocidade = (10 + rand() % 21);
    const float fatorDrible = (rand() % 51 - 25);

    // Limite do impulso
    const float limiteImpulso = 5.0;

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, limiteVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);

    // Calcula a distância entre o robô e a bola
    cpFloat distanciaDaBola = cpvdist(robotPos, ballPos);

    // Coordenadas da goleira direita
    cpVect goleiraDireita = cpv(980, 350);

    if(ballPos.x >= 512){
        if(vetorContido(ballPos, cpv(512,350), cpv(1000, 700))){                
            cpVect delta = cpvzero;

            //faz a checagm em ballPos.x  pra ele poder passar por trás da bola antes de voltar a tragetória
            if (ballPos.x < robotPos.x)
            {
                // Se a bola está atrás do robô, retorna para pos inicial, consequentemente passa atrás da bola e volta a rota de fazer gol]
                retornaJogadorOrigem(body, cpv(ballPos.x + 30,420));
            }
            else
            {
                // Calcula um vetor do robô à bola (DELTA = B - R)
                delta = cpvsub(ballPos, robotPos);

                // Limita o impulso
                delta = cpvmult(cpvnormalize(delta), limiteImpulso);

                //printf("distancia da bola: %f", distancia);

                if(robotPos.x >= 600){
                    if(distanciaDaBola <= 40){                    
                        //printf("impulsionou em direcao a goleira");
                        impulsionaBola(ballBody, goleiraDireita);                    
                    }
                }
                
                // Aplica impulso no robô para mover em direção à bola
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }        
        }else{
            retornaJogadorOrigem(body, cpv(700,600));
        }
    }else
    {
        // se a bola passou do meio de campo aliado ele retorna pra sua posição inicial   
        retornaJogadorOrigem(body, cpv(460,420));
    }
}

void moveRoboRed(cpBody* body, void* data)
{
    // Limite da velocidade do robô
    const float limiteVelocidade = (10 + rand() % 21);
    const float fatorDrible = (rand() % 51 - 25);

    // Limite do impulso
    const float limiteImpulso = 5.0;

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, limiteVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect RexPos = cpBodyGetPosition(Rex);
    cpVect RexP = cpBodyGetPosition(RexPequeno);
    cpVect ballPos = cpBodyGetPosition(ballBody);

    // Calcula a distância entre o robô e a bola
    cpFloat distanciaChute = cpvdist(robotPos, cpv(700, 350));
    cpFloat distanciaDaBola = cpvdist(robotPos, ballPos);
    cpFloat distanciaDaBolaRex = cpvdist(RexPos, ballPos);
    cpFloat distanciaDaBolaRexP = cpvdist(RexP, ballPos);
    cpFloat DistanciaRex = cpvdist(RexPos, RexP);
    cpFloat DistanciaRexP = cpvdist(RexP, RexPos);

    // Coordenadas da goleira direita
    cpVect goleiraDireita = cpv(44, 350);

    //printf("robotPos.x : %f  robotPos.y: %f \n", robotPos.x, robotPos.y);
    if(ballPos.x <= 512){
        if(vetorContido(ballPos, cpv(512,350), cpv(40, 0))){                
                cpVect delta = cpvzero;

                //faz a checagm em ballPos.x - 20 pra ele poder passar por trás da bola antes de voltar a tragetória
                if (ballPos.x - 20 > robotPos.x && robotPos.x < 550)
                {
                    // Se a bola está atrás do robô, retorna para pos inicial, consequentemente passa atrás da bola e volta a rota de fazer gol]
                    retornaJogadorOrigem(body, cpv(ballPos.x + 30,440));
                }
                else
                {
                    // Calcula um vetor do robô à bola (DELTA = B - R)
                    delta = cpvsub(ballPos, robotPos);

                    // Limita o impulso
                    delta = cpvmult(cpvnormalize(delta), limiteImpulso);

                    //printf("distancia da bola: %f", distancia);

                    //condição para chute a gol é o robo ter passado um pouco do meio campo e estar com posse da bola com a distancia +/- menor ou igual a 40
                    if(robotPos.x <= 400){
                        if(distanciaDaBola <= 40){                    
                            //printf("impulsionou em direcao a goleira");
                            impulsionaBola(ballBody, goleiraDireita);                    
                        }
                    }
                    
                    // Aplica impulso no robô para mover em direção à bola
                    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
                }           
        }else{
            retornaJogadorOrigem(body, cpv(200,180));
        }
    }else
    {
        // Se a bola passou do meio de campo aliado o jogador retorna para a sua posição inicial        
        retornaJogadorOrigem(body, cpv(565,290));
    }
}

void moveRoboLowerRed(cpBody* body, void* data)
{
    // Limite da velocidade do robô
    const float limiteVelocidade = (10 + rand() % 21);
    const float fatorDrible = (rand() % 51 - 25);

    // Limite do impulso
    const float limiteImpulso = 5.0;

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, limiteVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);

    // Calcula a distância entre o robô e a bola
    cpFloat distanciaChute = cpvdist(robotPos, cpv(700, 350));
    cpFloat distanciaDaBola = cpvdist(robotPos, ballPos);

    // Coordenadas da goleira direita
    cpVect goleiraDireita = cpv(44, 350);

    if(ballPos.x <= 512){
        if(vetorContido(ballPos, cpv(512,350), cpv(40, 700))){    
                cpVect delta = cpvzero;

                //faz a checagm em ballPos.x - 20 pra ele poder passar por trás da bola antes de voltar a tragetória
                if (ballPos.x - 20 > robotPos.x && robotPos.x < 550)
                {
                    // Se a bola está atrás do robô, retorna para pos inicial, consequentemente passa atrás da bola e volta a rota de fazer gol]
                    retornaJogadorOrigem(body, cpv(ballPos.x + 30,420));
                }
                else
                {
                    // Calcula um vetor do robô à bola (DELTA = B - R)
                    delta = cpvsub(ballPos, robotPos);

                    // Limita o impulso
                    delta = cpvmult(cpvnormalize(delta), limiteImpulso);

                    //printf("distancia da bola: %f", distancia);

                    if(robotPos.x <= 400){
                        if(distanciaDaBola <= 40){                    
                            //printf("impulsionou em direcao a goleira");
                            impulsionaBola(ballBody, goleiraDireita);                    
                        }
                    }
                    
                    // Aplica impulso no robô para mover em direção à bola
                    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
                }                        
        }else{
            retornaJogadorOrigem(body, cpv(200,530));
        }
    }else
    {
        // Se a bola está muito próxima do próprio gol, retornar ele para receber a bola impulsionada        
        retornaJogadorOrigem(body, cpv(565,420));
    }
}

void moveRoboZagueiroSup(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 350;
    const float limiteYCima = 100;
    const float centroCampoY = 350;
    const float maxVelocidade = 8.0;
    const float maxImpulso = 3.0;

    cpVect OrigemNikol = cpv(190,180);

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, maxVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect Atacante = cpBodyGetPosition(RexPequeno);

    // Calcula a distância entre o goleiro e o atacanet
    cpFloat distancia = cpvdist(robotPos, Atacante);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    // Condição para mover apenas quando estiver próximo da bola
    if (distancia < distanciaDoAtacante && robotPos.x <= 300) {
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites
        if (robotPos.x <= 400 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima) {
            delta = cpvsub(Atacante, robotPos);
        }
    } else {
       retornaJogadorOrigem(body, OrigemNikol);
    }

    // Limita o impulso 
    if (cpvlength(delta) > maxImpulso) {
        delta = cpvmult(cpvnormalize(delta), maxImpulso);
    }

    // Finalmente, aplica impulso no robô
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void moveRoboZagueiroCen(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 540;
    const float limiteYCima = 100;
    const float centroCampoY = 350;
    const float maxVelocidade = 12.0;
    const float maxImpulso = 3.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(225, 350);

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, maxVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect Atacante = Rex != NULL ? cpBodyGetPosition(Rex) : cpvzero;
    // Calcula a distância entre o goleiro e o atacanet
    cpFloat distancia = cpvdist(robotPos, Atacante);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    // Condição para mover apenas quando estiver próximo da bola
    if (distancia < distanciaDoAtacante && robotPos.x <= 300) {
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites
        if (robotPos.x <= 400 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima) {
            delta = cpvsub(Atacante, robotPos);
        }
    } else {
       retornaJogadorOrigem(body, posicaoOriginal);
    }

    // Limita o impulso
    if (cpvlength(delta) > maxImpulso) {
        delta = cpvmult(cpvnormalize(delta), maxImpulso);
    }

    // Finalmente, aplica impulso no robô
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void moveRoboZagueiroInf(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 540;
    const float limiteYCima = 350;
    const float centroCampoY = 350;
    const float maxVelocidade = 10.0;
    const float maxImpulso = 3.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(190, 530);

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, maxVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect Atacante = cpBodyGetPosition(Rex);
        
    // Calcula a distância entre o goleiro e a bola
    cpFloat distancia = cpvdist(robotPos, Atacante);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    // Condição para mover apenas quando estiver próximo da bola
    if (distancia < distanciaDoAtacante) {
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites
        if (robotPos.x < 400 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima) {            
            delta = cpvsub(Atacante, robotPos);
        }        
    } else {
       retornaJogadorOrigem(body, posicaoOriginal);
    }

    // Limita o impulso
    if (cpvlength(delta) > maxImpulso) {
        delta = cpvmult(cpvnormalize(delta), maxImpulso);
    }

    // Finalmente, aplica impulso no robô
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void moveRoboZagueiroSupDir(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 350;
    const float limiteYCima = 100;
    const float centroCampoY = 350;
    const float maxVelocidade = 8.0;
    const float maxImpulso = 3.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(830, 180);

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, maxVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect Atacante = cpBodyGetPosition(Dunban);
    
    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    // Condição para mover apenas quando estiver próximo da bola
    if(ballPos.x > 600){
        if (robotPos.x > 650 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima)     
            delta = cpvsub(Atacante, robotPos);   
        else
            retornaJogadorOrigem(body, posicaoOriginal);
    }    
    else
        retornaJogadorOrigem(body, posicaoOriginal);
    

    // Limita o impulso
    if (cpvlength(delta) > maxImpulso) {
        delta = cpvmult(cpvnormalize(delta), maxImpulso);
    }

    // Finalmente, aplica impulso no robô
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void moveRoboZagueiroCenDir(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 540;
    const float limiteYCima = 100;
    const float centroCampoY = 350;
    const float maxVelocidade = 12.0;
    const float maxImpulso = 3.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(800, 350);

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, maxVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect Atacante = Matthew != NULL ? cpBodyGetPosition(Matthew) : cpvzero;
    // Calcula a distância entre o goleiro e o atacanet
    cpFloat distancia = cpvdist(ballPos, Atacante);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    if(ballPos.x > 700 && distancia < 50){
        if (robotPos.x > 650 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima)     
            delta = cpvsub(Atacante, robotPos);   
        else
            retornaJogadorOrigem(body, posicaoOriginal);
    }    
    else
        retornaJogadorOrigem(body, posicaoOriginal);

    // Limita o impulso
    if (cpvlength(delta) > maxImpulso) {
        delta = cpvmult(cpvnormalize(delta), maxImpulso);
    }

    // Finalmente, aplica impulso no robô
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void moveRoboZagueiroInfDir(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 600;
    const float limiteYCima = 350;
    const float centroCampoY = 350;
    const float maxVelocidade = 8.0;
    const float maxImpulso = 3.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(830, 530);

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, maxVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect Atacante = cpBodyGetPosition(Matthew);
        
    // Calcula a distância entre o goleiro e a bola
    cpFloat distancia = cpvdist(robotPos, Atacante);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    // Condição para mover apenas quando estiver próximo da bola
    if (distancia < 200.0) {
        // Ajusta a área em que os zagueiros vão interferir no ataque
        if (robotPos.x > 700 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima) {
            delta = cpvsub(Atacante, robotPos);        
        }
    } else {
       retornaJogadorOrigem(body, posicaoOriginal);
    }
    // Limita o impulso
    if (cpvlength(delta) > maxImpulso) {
        delta = cpvmult(cpvnormalize(delta), maxImpulso);
    }
    // Finalmente, aplica impulso no robô
    cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void moveRoboGoleiro(cpBody* body, void* data)
{
    // Limites da área do goleiro
    float areaCimaY = 525.0;
    float areaBaixoY = 262.0;
    
    cpVect posOrigem = cpv(100, 350);
    
    int chanceBloqueio = (rand() % 50);

    // Limita a velocidade do goleiro
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 5);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do goleiro e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    // Calcula a distância entre o goleiro e a bola
    cpFloat distancia = cpvdist(robotPos, ballPos);

    // Vetor delta para calcular o movimento do goleiro
    cpVect delta = cpvzero;

    //checa se o goleiro está onde deveria
    if (robotPos.x >= 60 && robotPos.x <= posOrigem.x + 30 && robotPos.y <= areaCimaY && robotPos.y >= areaBaixoY)
    {                
        // Ajusta a posição em y do goleiro para seguir a bola dentro dos limites        
        delta = cpvsub(ballPos,robotPos);             
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);                            
    }
    else
    {
        // Retorna o goleiro à posição original
        retornaJogadorOrigem(body, posOrigem);
    }
}

void moveRoboGoleiroRed(cpBody* body, void* data)
{
    // Limites da área do goleiro
    float areaCimaY = 525.0;
    float areaBaixoY = 262.0;
    
    cpVect posOrigem = cpv(930, 350);
    
    int chanceBloqueio = (rand() % 50);

    // Limita a velocidade do goleiro
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, 15);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do goleiro e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    // Calcula a distância entre o goleiro e a bola
    cpFloat distancia = cpvdist(robotPos, ballPos);

    // Vetor delta para calcular o movimento do goleiro
    cpVect delta = cpvzero;

    //checa se o goleiro está onde deveria
    if (robotPos.x <= 1000 && robotPos.x >= posOrigem.x && robotPos.y <= areaCimaY && robotPos.y >= areaBaixoY)
    {               
        // Ajusta a posição em y do goleiro para seguir a bola dentro dos limites        
        delta = cpvsub(ballPos, robotPos);             

        cpFloat magnitude = 11.0; // Ajuste este valor conforme necessário
        cpVect impulso = cpvmult(cpvnormalize(delta), magnitude);

        // Aplica impulso no goleiro
        cpBodyApplyImpulseAtWorldPoint(body, impulso, robotPos);                            
    }
    else
    {
        // Retorna o goleiro à posição original
        retornaJogadorOrigem(body, posOrigem);
    }
}

void moveBola(cpBody* body, void* data)
{
    //cpv(512,350) meio do campo    
    //caso aconteça gol reseta no meio de campo
    if(isBallinGoleira1())
        resetaBall(body, 512.0, 350.0);    
    //caso a bola passe da goleira e não esteja na área da goleira ela reseta pro meio de campo por conveniencia
    isBallOutOfBounds();
}

void resetaBall(cpBody* body, float x, float y){
    // Posição desejada para onde a bola vai
    cpVect meioCampo = cpv(x, y);
    //seta velocidade da bola para não resetar posição em movimento
    cpBodySetVelocity(body, cpvzero);
    // Define a posição da bola diretamente para o meio do campo
    cpBodySetPosition(body, meioCampo);
    //Condição adicionada para evitar problemas em debug, caso cpBody não exista não da problema em execução
    if(Rex)
        cpBodySetPosition(Rex, cpv(565,420));
    if(RexPequeno)
        cpBodySetPosition(RexPequeno, cpv(565,290));
    if(Shulk)
        cpBodySetPosition(Shulk, cpv(190,530));
    if(Nikol)
        cpBodySetPosition(Nikol, cpv(190,180));
    if(A)
        cpBodySetPosition(A, cpv(90, 350));
    if(Matthew)
        cpBodySetPosition(Matthew, cpv(460,420));
    if(Glimmer)
        cpBodySetPosition(Glimmer, cpv(930,350));
    if(Pyra)
        cpBodySetPosition(Pyra, cpv(830,180));
    if(Mythra)
        cpBodySetPosition(Mythra, cpv(830,530));
    if(Fiora)
        cpBodySetPosition(Fiora, cpv(225,350));    
    if(Nia)
        cpBodySetPosition(Nia, cpv(800,350));
    if(Dunban)
        cpBodySetPosition(Dunban, cpv(460,290));
    if(score1 == 5)
        gameOver = 1;
    if(score2 == 5)
        gameOver = 1;
}

bool isBallOutOfBounds() {
    // Coordenadas das goleiras e traves

    //falta trazer baseado em qual lado
    
    //caso bola passe de 44 acima de 380 retorna a bola na frente do goleiro
    //caso bola passe de 44 abaixo de 325 retorna a bola na frente do goleiro
    cpVect goleiraEsquerda = cpv(44, 350);

    //caso bola passe de 980 acima de 380 retorna a bola na frente do goleiro
    //caso bola passe de 980 abaixo de 325 retorna a bola na frente do goleiro
    cpVect goleiraDireita = cpv(980, 350);
    
    float traveCimaY = 400.0;
    float traveBaixoY = 300.0;
    cpVect ballPos = cpBodyGetPosition(ballBody);
    
    // Debugging
    // printf("ballPos.x : %f \n", ballPos.x);
    // printf("ballPos.y %f \n", ballPos.y);
    // printf("goleiraDireita.x : %f \n", goleiraDireita.x);
    // printf("traveCima.y : %f \n", traveCimaY);
    // printf("traveBaixo.y : %f \n", traveBaixoY);

    // Verifica se a bola está abaixo ou acima da goleira esquerda
    if (ballPos.x <= goleiraEsquerda.x && ballPos.y >= traveCimaY ||  ballPos.x <= goleiraEsquerda.x &&  ballPos.y <= traveBaixoY) {  
        //printf("esta acima ou abaixo"); 
        resetaBall(ballBody, 512.0, 350.0);     
        return true;
    }

    // Verifica se a bola está abaixo ou acima da goleira direita
    if (ballPos.x >= goleiraDireita.x && ballPos.y >= traveCimaY || ballPos.x >= goleiraDireita.x && ballPos.y <= traveBaixoY) {
        //printf("esta acima ou abaixo direita");
        resetaBall(ballBody, 512.0, 350.0);
        return true;
    }

    return false;
}

bool isBallinGoleira1() {
    // Coordenadas das goleiras e traves
    cpVect goleiraEsquerda = cpv(44, 350);
    cpVect goleiraDireita = cpv(980, 350);
    
    float traveCimaY = 400.0;
    float traveBaixoY = 315.0;
    cpVect ballPos = cpBodyGetPosition(ballBody);

    // Debugging
    // printf("ballPos.x : %f \n", ballPos.x);
    // printf("ballPos.y %f \n", ballPos.y);
    // printf("goleiraDireita.x : %f \n", goleiraDireita.x);
    // printf("traveCima.y : %f \n", traveCimaY);
    // printf("traveBaixo.y : %f \n", traveBaixoY);

    // Verifica se a bola está dentro da área da goleira esquerda
    if (ballPos.x <= goleiraEsquerda.x && 
        ballPos.y <= traveCimaY && 
        ballPos.y >= traveBaixoY) {
        score2++;
        return true;
    }

    // Verifica se a bola está dentro da área da goleira direita
    if (ballPos.x >= goleiraDireita.x && 
        ballPos.y <= traveCimaY && 
        ballPos.y >= traveBaixoY) {
        score1++;
        return true;
    }

    return false;
}

void retornaJogadorOrigem(cpBody* body, cpVect pontoOrigem){
    // Se a bola está muito próxima do próprio gol, retornar ele para receber a bola impulsionada
    //no final essa função podia ser chamada de movimento do jogador, mas envolviam outras variáveis para cada situação, preferi manter do jeito atual
        cpVect delta = cpvzero;
        cpVect robotPos = cpBodyGetPosition(body);
        const float fatorAleatorio = (rand() % 15);

        delta = cpvsub(pontoOrigem, robotPos);     
        //delta.y = delta.y + fatorAleatorio * 0.3;   

        // Limita o impulso no robô para retornar à posição original
        if (cpvlength(delta) > 10) {
            delta = cpvmult(cpvnormalize(delta), 10);
        }

        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
}

void impulsionaBola(cpBody* ballBody, cpVect goleira)
{
    //O herói do código, o código de impulsionar a bola, sem essa parte o jogo não funcionaria tão bem quanto está.
    // Calcula a direção e a magnitude do impulso na bola
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect delta = cpvsub(goleira, ballPos);

    // Normaliza e aplica um impulso à bola
    cpFloat magnitude = 11.0; // Ajuste este valor conforme necessário
    cpVect impulso = cpvmult(cpvnormalize(delta), magnitude);

    cpBodyApplyImpulseAtWorldPoint(ballBody, impulso, ballPos);
}

bool vetorContido(cpVect v, cpVect a, cpVect b) {
    // Obtem os limites mínimos e máximos do retângulo
    // Essa função poderia ter sido utilizada para várias outras partes no jogo, mas só pensei em fazer ela quando desenvolvi o funcionamento dos atacantes
    // Então não tive vontade de implementar ela no resto do código, afinal as condições já estavam cumprindo o que eu esperava
    double min_x = fmin(a.x, b.x);
    double max_x = fmax(a.x, b.x);
    double min_y = fmin(a.y, b.y);
    double max_y = fmax(a.y, b.y);
    
    // Verifica se o vetor v está dentro dos limites
    return (v.x >= min_x && v.x <= max_x && v.y >= min_y && v.y <= max_y);
}

// Libera memória ocupada por cada corpo, forma e ambiente
// Acrescente mais linhas caso necessário
void freeCM()
{
    printf("Cleaning up!\n");
    UserData* ud = cpBodyGetUserData(ballBody);
    cpShapeFree(ud->shape);
    cpBodyFree(ballBody);

    ud = cpBodyGetUserData(Nikol);
    cpShapeFree(ud->shape);
    cpBodyFree(Nikol);

    cpShapeFree(leftWall);
    cpShapeFree(rightWall);
    cpShapeFree(bottomWall);
    cpShapeFree(topWall);

    cpSpaceFree(space);
}

void destroyShulk(cpSpace* space, void* ptr, void* data) {
    cpBody* body = (cpBody*)ptr;

    // Get the user data associated with the body
    UserData* ud = (UserData*)cpBodyGetUserData(body);

    // Remove the shape from the space
    if (ud && ud->shape) {
        cpSpaceRemoveShape(space, ud->shape);
        cpShapeFree(ud->shape);
    }

    // Remove the body from the space
    cpSpaceRemoveBody(space, body);

    // Destroy the body
    cpBodyDestroy(body);

    // Set the pointer to NULL to avoid dangling references
    Shulk = NULL;
}

// Função chamada para reiniciar a simulação
void restartCM()
{
    // Escreva o código para reposicionar os jogadores, ressetar o score, etc
    resetaBall(ballBody, 512, 350);
    score1 = 0;
    score2 = 0;
    gameOver = 0;
    // Não esqueça de ressetar a variável gameOver!
    //gameOver = 0;
}

// ************************************************************
//
// A PARTIR DESTE PONTO, O PROGRAMA NÃO DEVE SER ALTERADO
//
// A NÃO SER QUE VOCÊ SAIBA ***EXATAMENTE*** O QUE ESTÁ FAZENDO
//
// ************************************************************

int main(int argc, char** argv)
{
    // Inicialização da janela gráfica
    init(argc,argv);

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}

// Cria e adiciona uma nova linha estática (segmento) ao ambiente
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast)
{
   cpShape* aux = cpSegmentShapeNew(cpSpaceGetStaticBody(space), inicio, fim, 0);
   cpShapeSetFriction(aux, fric);
   cpShapeSetElasticity(aux, elast);
   cpSpaceAddShape(space, aux);
   return aux;
}

// Cria e adiciona um novo corpo dinâmico, com formato circular
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast)
{
    // Primeiro criamos um cpBody para armazenar as propriedades fisicas do objeto
    // Estas incluem: massa, posicao, velocidade, angulo, etc do objeto
    // A seguir, adicionamos formas de colisao ao cpBody para informar o seu formato e tamanho

    // O momento de inercia e' como a massa, mas para rotacao
    // Use as funcoes cpMomentFor*() para calcular a aproximacao dele
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    // As funcoes cpSpaceAdd*() retornam o que voce esta' adicionando
    // E' conveniente criar e adicionar um objeto na mesma linha
    cpBody* newBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));

    // Por fim, ajustamos a posicao inicial do objeto
    cpBodySetPosition(newBody, pos);

    // Agora criamos a forma de colisao do objeto
    // Voce pode criar multiplas formas de colisao, que apontam ao mesmo objeto (mas nao e' necessario para o trabalho)
    // Todas serao conectadas a ele, e se moverao juntamente com ele
    cpShape* newShape = cpSpaceAddShape(space, cpCircleShapeNew(newBody, radius, cpvzero));
    cpShapeSetFriction(newShape, fric);
    cpShapeSetElasticity(newShape, elast);

    UserData* newUserData = malloc(sizeof(UserData));
    newUserData->tex = loadImage(img);
    newUserData->radius = radius;
    newUserData->shape= newShape;
    newUserData->func = func;
    cpBodySetUserData(newBody, newUserData);
    printf("newCircle: loaded img %s\n", img);
    return newBody;
}
