#include <math.h>
#include <chipmunk.h>
#include <SOIL.h>
#include <stdbool.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Funções para movimentação de objetos
void moveRobo(cpBody* body, void* data);
void moveRoboGoleiro(cpBody* body, void* data);
void moveRoboZagueiro(cpBody* body, void* data);
void moveBola(cpBody* body, void* data);
bool isBallinGoleira1();
bool isBallOutOfBounds();
void resetaBall(cpBody* vect, float x, float y);

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

// Um robô
cpBody* robotBody;

// Um robô
cpBody* Rex;

// Um robô
cpBody* Shulk;

// Um robô
cpBody* A;


// Cada passo de simulação é 1/60 seg.
cpFloat timeStep = 1.0/60.0;

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
    //robotBody = newCircle(cpv(120,350), 20, 5, "ship1.png", NULL, 0.2, 0.5);
    

    //shulk será dois zagueiro
    //Shulk = newCircle(cpv(200,450), 20, 5, "shulk_icon.png", moveRoboZagueiro, 0.2, 0.5);
    //robotBody = newCircle(cpv(200,300), 20, 5, "shulk_icon.png", moveRoboZagueiro, 0.2, 0.5);
    
    //rex será dois atacantes atacante
    Rex = newCircle(cpv(800,350), 20, 5, "rex_icon.png", moveRobo, 0.2, 0.5);
    
    //A será a goleira
    A = newCircle(cpv(90,350), 20, 5, "A_icon.png", moveRoboGoleiro, 0.2, 0.5);
}

// Exemplo de função de movimentação: move o robô em direção à bola
void moveRobo(cpBody* body, void* data)
{
    // Limite da área onde o atacante deve evitar (próximo ao próprio gol)
    const float limiteProprioGolX = 100.0;

    // Limite da velocidade do robô
    const float limiteVelocidade = 20.0;

    // Limite do impulso
    const float limiteImpulso = 10.0;

    // Obtém e limita a velocidade do robô
    cpVect vel = cpBodyGetVelocity(body);
    vel = cpvclamp(vel, limiteVelocidade);
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos = cpBodyGetPosition(ballBody);

    // Verifica se a bola está no meio de campo ou no campo adversário
    if (ballPos.x > limiteProprioGolX)
    {
        // Calcula um vetor do robô à bola (DELTA = B - R)
        cpVect delta = cpvsub(ballPos, robotPos);

        // Limita o impulso
        delta = cpvmult(cpvnormalize(delta), limiteImpulso);

        // Aplica impulso no robô para mover em direção à bola
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    else
    {
        // Se a bola está muito próxima do próprio gol, não se mover
        cpBodySetVelocity(body, cpvzero);
    }
}


void moveRoboZagueiro(cpBody* body, void* data) {
    // Detalhes das posições limites
    const float limiteGoleiroX = 90;
    const float limiteYBaixo = 525;
    const float limiteYCima = 262;
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

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;

    // Condição para mover apenas quando estiver próximo da bola
    if (cpvdist(robotPos, ballPos) < 200.0) {
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites
        if (robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima) {
            delta.y = ballPos.y - robotPos.y;
        }

        // Mantenha o robô na posição x original do zagueiro
        delta.x = posicaoOriginal.x - robotPos.x;
    } else {
        // Ajusta a posição em y do robô para retornar a sua posição original
        if (robotPos.y <= limiteYBaixo && robotPos.y >= limiteYCima) {
            delta.y = posicaoOriginal.y - robotPos.y;
        }

        // Mantenha o robô na posição x original do zagueiro
        delta.x = posicaoOriginal.x - robotPos.x;
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
    // Detalhe: x em 90 é o limite máximo do goleiro
    // Detalhe: y limite max é 525 p baixo, 262 p cima, 350 é o centro
    //fazer o calculo da distancia do vetor do jogador até a distância máxima que ele pode andar 
    //ou seja
    //se o goleiro andar até a área menor para empurrar a bola para fora, ele para no limite da área menor

    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    //printf("vel: %f %f", vel.x, vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect delta = cpvzero;
    //Condição para realmente só mexer quando estiver próximo

    printf("distância do robo em relacao a bola: %f \n", cpvdist(robotPos, ballPos));


    if(cpvdist(robotPos, ballPos) < 200.0){
        printf("entrou");
        // Ajusta a posição em y do robô para seguir a bola dentro dos limites
        if (ballPos.y <= 430 && ballPos.y >= 290) {
            delta.y = ballPos.y - robotPos.y;
        }

        // Mantenha o robô na linha do gol (x = 90)
        if (robotPos.x > 90) {
            delta.x = 90;
        }            

        // Limita o impulso em 10 unidades
        if (cpvlength(delta) > 10) {
            delta = cpvmult(cpvnormalize(delta), 100);
        }

        // Finalmente, aplica impulso no robô
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
}


// Exemplo: move a bola para o meio de campo caso ela se encontre parada por determinada quantidade de tempo
void moveBola(cpBody* body, void* data)
{
    //cpv(512,350) meio do campo    
    if(isBallinGoleira1())
        resetaBall(body, 512.0, 350.0);    

    if(isBallOutOfBounds())
        resetaBall(body, 100.0, 350.0);

}

void resetaBall(cpBody* body, float x, float y){
    // Posição desejada para o centro do campo
    cpVect meioCampo = cpv(x, y);

    cpVect RexPos = cpv(800,350);
    cpVect ShulkPos = cpv(200,450);

    // Define a posição da bola diretamente para o meio do campo
    cpBodySetPosition(body, meioCampo);
    //Condição adicionada para evitar problemas em debug, caso cpBody não exista não da problema em execução
    if(Rex)
        cpBodySetPosition(Rex, RexPos);
    //Condição adicionada para evitar problemas em debug, caso cpBody não exista não da problema em execução
    if(Shulk)
        cpBodySetPosition(Shulk, ShulkPos);
}

bool isBallOutOfBounds() {
    // Coordenadas das goleiras e traves
    
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
    printf("ballPos.x : %f \n", ballPos.x);
    printf("ballPos.y %f \n", ballPos.y);
    printf("goleiraDireita.x : %f \n", goleiraDireita.x);
    printf("traveCima.y : %f \n", traveCimaY);
    printf("traveBaixo.y : %f \n", traveBaixoY);

    // Verifica se a bola está abaixo ou acima da goleira esquerda
    if (ballPos.x <= goleiraEsquerda.x && ballPos.y >= traveCimaY ||  ballPos.x <= goleiraEsquerda.x &&  ballPos.y <= traveBaixoY) {  
        printf("esta acima ou abaixo");      
        return true;
    }

    // Verifica se a bola está abaixo ou acima da goleira direita
    if (ballPos.x >= goleiraDireita.x && ballPos.y >= traveCimaY || ballPos.x >= goleiraDireita.x && ballPos.y >= traveBaixoY) {
        printf("esta acima ou abaixo direita");
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

// Libera memória ocupada por cada corpo, forma e ambiente
// Acrescente mais linhas caso necessário
void freeCM()
{
    printf("Cleaning up!\n");
    UserData* ud = cpBodyGetUserData(ballBody);
    cpShapeFree(ud->shape);
    cpBodyFree(ballBody);

    ud = cpBodyGetUserData(robotBody);
    cpShapeFree(ud->shape);
    cpBodyFree(robotBody);

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
    // Remove todas as shapes associadas ao corpo

    // if (Shulk) {
    //     printf("Tentou destruir o Shulk\n");

    //     cpSpaceAddPostStepCallback(space, destroyShulk, Rex, NULL);
    //     cpSpaceAddPostStepCallback(space, destroyShulk, Shulk, NULL);
    //     cpSpaceAddPostStepCallback(space, destroyShulk, ballBody, NULL);
    // }

    
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
