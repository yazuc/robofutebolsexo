#include <math.h>
#include <chipmunk.h>
#include <SOIL.h>
#include <stdbool.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Funções para movimentação de objetos
void moveRobo(cpBody* body, void* data);
void moveRoboRed(cpBody* body, void* data);
void moveRoboGoleiro(cpBody* body, void* data);
void moveRoboGoleiroRed(cpBody* body, void* data);
void moveRoboZagueiroSup(cpBody* body, void* data);
void moveRoboZagueiroInf(cpBody* body, void* data);
void moveRoboZagueiroSupDir(cpBody* body, void* data);
void moveRoboZagueiroInfDir(cpBody* body, void* data);
void moveBola(cpBody* body, void* data);
bool isBallinGoleira1();
bool isBallOutOfBounds();
void resetaBall(cpBody* vect, float x, float y);
void retornaJogadorOrigem(cpBody* body, cpVect pontoOrigem);
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

// A bola
cpBody* ballBody;

//Zagueiro vermelho
cpBody* Pyra;

//Zagueiro vermelho
cpBody* Mythra;

// Zagueiro azul
cpBody* Nikol;

cpBody* robotBody;

// Atacante vermelho
cpBody* Rex;

// Atacante azul
cpBody* Matthew;

// Zagueiro Azul
cpBody* Shulk;

// Goleiro Azul
cpBody* A;

// Goleiro Vermelho
cpBody* Glimmer;


// Cada passo de simulação é 1/60 seg.
cpFloat timeStep = 1.0/60.0;

//cpVect OrigemMatthew = cpv(480,440);
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


    // ... e um robô de exemplo    
    //robotBody = newCircle(cpv(550,440), 20, 5, "ship1.png", NULL, 0.2, 0.5);

    //shulk e nikol são zagueiros
    Shulk = newCircle(cpv(200,450), 20, 5, "RoundShulk_icon.png", moveRoboZagueiroInf, 0.2, 0.5);
    Nikol = newCircle(cpv(200,300), 20, 5, "rounded_nikol.png", moveRoboZagueiroSup, 0.2, 0.5);
    
    //Pyra e Mythra são zagueiras
    Pyra = newCircle(cpv(830,300), 20, 5, "pyra.png", moveRoboZagueiroSupDir, 0.2, 0.5);
    Mythra = newCircle(cpv(830,430), 20, 5, "mythra.png", NULL, 0.2, 0.5);

    //rex será um atacantee
    Rex = newCircle(cpv(550,440), 20, 5, "RoundRex_icon.png", moveRoboRed, 0.2, 0.5);

    //Matthew será o outro atacante
    Matthew = newCircle(cpv(470,440), 20, 5, "roundMatthe_icon.png", moveRobo, 0.2, 0.5);

    //A será a goleira
    A = newCircle(cpv(100,350), 10, 5, "RoundA_icon.png", moveRoboGoleiro, 0.2, 0.5);
    
    //bern the goatleiro
    Glimmer = newCircle(cpv(930,350), 10, 5, "glimmer.png", moveRoboGoleiroRed, 0.2, 0.5);

}

void moveRobo(cpBody* body, void* data)
{
    // Limite da área onde o atacante deve evitar (próximo ao próprio gol)
    const float limiteProprioGolX = 50.0;

    // Limite da velocidade do robô
    const float limiteVelocidade = (10 + rand() % 21);
    const float fatorDrible = (rand() % 9 - 3);
    
    // Limite do impulso
    const float limiteImpulso = 3.0;

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, limiteVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect goleiraDireita = cpv(980, 350);

    // Calcula a distância entre o robô e a bola
    cpFloat distanciaChute = cpvdist(robotPos, cpv(700, 350));
    cpFloat distanciaDaBola = cpvdist(robotPos, ballPos);

    // Coordenadas da goleira direita
    float traveCimaY = 380.0;
    float traveBaixoY = 325.0;

    // Verifica se a bola está no meio de campo ou no campo adversário
    cpVect delta = cpvzero;

    //faz a checagm em ballPos.x - 20 pra ele poder passar por trás da bola antes de voltar a tragetória
    if (ballPos.x < robotPos.x )
    {
        // Se a bola está atrás do robô, calcular vetor para contornar a bola
        // Calcula um vetor do robô à bola (DELTA = B - R)
        retornaJogadorOrigem(body, cpv(480,440));
    }
    else
    {
        // Calcula um vetor do robô à bola (DELTA = B - R)
        delta = cpvsub(ballPos, robotPos);

        // Limita o impulso
        delta = cpvmult(cpvnormalize(delta), limiteImpulso);

        // printf("distancia do chute: %f \n", distanciaChute);
        // printf("distancia da bola: %f \n", distanciaDaBola);

        //todo: quando atacante chegar próximo da coordenada x definida como chuteAGol, deverá ser propulsioinada a bola em direção a goleira
        
        //funciona como debug para o goleiro vermelho impulsionaBola(ballBody, goleiraDireita);       
        
        if(robotPos.x >= 700){
            if(distanciaDaBola <= 30){
                printf("impulsionou em direcao a goleira");
                impulsionaBola(ballBody, goleiraDireita);                    
            }
        }

        // Aplica impulso no robô para mover em direção à bola
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
}

void moveRoboRed(cpBody* body, void* data)
{
    // Limite da área onde o atacante deve evitar (próximo ao próprio gol)
    const float limiteProprioGolX = 900;

    // Limite da velocidade do robô
    const float limiteVelocidade = (10 + rand() % 21);
    const float fatorDrible = (rand() % 7 - 3);

    // Limite do impulso
    const float limiteImpulso = 3.0;

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
    float traveCimaY = 380.0;
    float traveBaixoY = 325.0;

    // Verifica se a bola está no meio de campo ou no campo adversário
    if (ballPos.x < limiteProprioGolX)
    {
        cpVect delta = cpvzero;

        //faz a checagm em ballPos.x - 20 pra ele poder passar por trás da bola antes de voltar a tragetória
        if (ballPos.x - 20 > robotPos.x )
        {
            // Se a bola está atrás do robô, calcular vetor para contornar a bola
            retornaJogadorOrigem(body, cpv(550,440));
        }
        else
        {
            // Calcula um vetor do robô à bola (DELTA = B - R)
            delta = cpvsub(ballPos, robotPos);

            // Limita o impulso
            delta = cpvmult(cpvnormalize(delta), limiteImpulso);

            //printf("distancia da bola: %f", distancia);

            if(robotPos.x <= 300){
                if(distanciaDaBola <= 30){
                    printf("impulsionou em direcao a goleira");
                    impulsionaBola(ballBody, goleiraDireita);                    
                }
            }
            
            // Aplica impulso no robô para mover em direção à bola
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    }
    else
    {
        // Se a bola está muito próxima do próprio gol, retornar ele para receber a bola impulsionada        
        retornaJogadorOrigem(body, cpv(550,440));
    }
}
//arrumar movimentação
void moveRoboZagueiroSup(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 350;
    const float limiteYCima = 100;
    const float centroCampoY = 350;
    const float maxVelocidade = 12.0;
    const float maxImpulso = 10.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(200, 300);

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, maxVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect Atacante = cpBodyGetPosition(Rex);
    // Calcula a distância entre o goleiro e a bola
    cpFloat distancia = cpvdist(robotPos, ballPos);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    // Condição para mover apenas quando estiver próximo da bola
    if (distancia < 200.0) {
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites
        if (robotPos.x < 400 && robotPos.y <= limiteYBaixo && robotPos.y <= limiteYCima) {
            delta = cpvsub(Atacante, robotPos);
        }

        // Mantenha o robô na posição x original do zagueiro
        delta.x = posicaoOriginal.x - robotPos.x;
    } else {
       retornaJogadorOrigem(body, posicaoOriginal);
    }

    // Limita o impulso em 10 unidades
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
    const float maxVelocidade = 5.0;
    const float maxImpulso = 10.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(200, 450);

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
    if (distancia < 200.0) {
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites
        if (robotPos.x < 400 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima) {            
            delta = cpvsub(Atacante, robotPos);
        }        
    } else {
       retornaJogadorOrigem(body, posicaoOriginal);
    }

    // Limita o impulso em 10 unidades
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
    const float maxVelocidade = 12.0;
    const float maxImpulso = 5.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(830, 300);

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
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites   
        if (robotPos.x > 700 && robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima)     
            delta = cpvsub(Atacante, robotPos);        
    } else if(distancia >= 300){
       retornaJogadorOrigem(body, posicaoOriginal);
    }

    // Limita o impulso em 10 unidades
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
    const float maxVelocidade = 5.0;
    const float maxImpulso = 5.0;

    // Posição original do zagueiro
    cpVect posicaoOriginal = cpv(830, 450);

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
    // Limita o impulso em 10 unidades
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
    vel = cpvclamp(vel, 50);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do goleiro e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    // Calcula a distância entre o goleiro e a bola
    cpFloat distancia = cpvdist(robotPos, ballPos);

    // Vetor delta para calcular o movimento do goleiro
    cpVect delta = cpvzero;

    // Impulso que será aplicado na bola
    cpVect ballImpulse = cpv(7, (rand() % 7)); // Direção e magnitude do impulso na bola


    //checa se o goleiro está onde deveria
    if (robotPos.x >= 60 && robotPos.x <= posOrigem.x && robotPos.y <= areaCimaY && robotPos.y >= areaBaixoY)
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
    vel = cpvclamp(vel, 10);
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
    if(isBallinGoleira1())
        resetaBall(body, 512.0, 350.0);    
    //isBallOutOfBounds();

}

void resetaBall(cpBody* body, float x, float y){
    // Posição desejada para onde a bola vai
    cpVect meioCampo = cpv(x, y);

    cpVect RexPos = cpv(550,440);
    cpVect ShulkPos = cpv(200,450);
    cpVect APos = cpv(90, 350);

    cpBodySetVelocity(body, cpvzero);
    // Define a posição da bola diretamente para o meio do campo
    cpBodySetPosition(body, meioCampo);
    //Condição adicionada para evitar problemas em debug, caso cpBody não exista não da problema em execução
    if(Rex)
        cpBodySetPosition(Rex, RexPos);
    //Condição adicionada para evitar problemas em debug, caso cpBody não exista não da problema em execução
    if(Shulk)
        cpBodySetPosition(Shulk, ShulkPos);
    if(A)
        cpBodySetPosition(A, APos);
    if(Matthew)
        cpBodySetPosition(Matthew, cpv(480,440));
    if(Glimmer)
        cpBodySetPosition(Glimmer, cpv(930,350));
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
    
    float traveCimaY = 380.0;
    float traveBaixoY = 325.0;
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
        resetaBall(ballBody, 100.0, 350.0);     
        return true;
    }

    // Verifica se a bola está abaixo ou acima da goleira direita
    if (ballPos.x >= goleiraDireita.x && ballPos.y >= traveCimaY || ballPos.x >= goleiraDireita.x && ballPos.y <= traveBaixoY) {
        //printf("esta acima ou abaixo direita");
        resetaBall(ballBody, 900.0, 350.0);
        return true;
    }

    return false;
}

bool isBallinGoleira1() {
    // Coordenadas das goleiras e traves
    cpVect goleiraEsquerda = cpv(44, 350);
    cpVect goleiraDireita = cpv(980, 350);
    
    float traveCimaY = 380.0;
    float traveBaixoY = 325.0;
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
    // Calcula a direção e a magnitude do impulso na bola
    cpVect ballPos = cpBodyGetPosition(ballBody);
    cpVect delta = cpvsub(goleira, ballPos);

    // Normaliza e aplica um impulso à bola
    cpFloat magnitude = 11.0; // Ajuste este valor conforme necessário
    cpVect impulso = cpvmult(cpvnormalize(delta), magnitude);

    cpBodyApplyImpulseAtWorldPoint(ballBody, impulso, ballPos);
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
