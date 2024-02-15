#include <SDL.h>
#include <SDL_ttf.h>

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 800;
const int screen_height = 600;
const int screen_bpp = 32;

// use essa função pra carregar uma imagem.bmp e deixa o fundo transparente
SDL_Surface *fundo_transparente(const char *filename, Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_Surface *load = NULL;
    SDL_Surface *otimizado = NULL;

    load = SDL_LoadBMP(filename);

    if(load != NULL)
    {
        otimizado = SDL_DisplayFormat(load);
        SDL_FreeSurface(load);

        if(otimizado != NULL)
        {
            SDL_SetColorKey(otimizado, SDL_SRCCOLORKEY, SDL_MapRGB(otimizado->format, red, green, blue));
        }
    }

    return otimizado;
}


// use essa função pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}

// use essa função pra desenhar uma imagem cortada na tela
void DrawImageFrame(int x, int y, SDL_Surface *source, SDL_Surface *destination, int width, int height, int frame)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    int coluna = source->w / width;

    SDL_Rect corte;
    corte.x = (frame % coluna)*width;
    corte.y = (frame / coluna)*height;
    corte.w = width;
    corte.h = height;

    SDL_BlitSurface(source, &corte, destination, &mover);
}

TTF_Font *ttfFile = NULL;

SDL_Surface *iconImage = NULL;
SDL_Surface *backgroundImage = NULL;
SDL_Surface *playerImage     = NULL;
SDL_Surface *BlockImage = NULL;



// use essa função pra carregar arquivos
// nota: essa função só deve ser chamada no começo do programa
void LoadFiles()
{
    ttfFile = TTF_OpenFont("fontes/times.ttf", 40);
    backgroundImage = SDL_LoadBMP("gfx/background.bmp");
    playerImage     = SDL_LoadBMP("gfx/player.bmp");
    BlockImage = SDL_LoadBMP("gfx/blocks.bmp");
}


// use essa função pra fechar arquivos
// nota: essa função só deve ser chamada no final do programa
void CloseFiles()
{
    TTF_CloseFont(ttfFile);
    SDL_FreeSurface(iconImage);
    SDL_FreeSurface(backgroundImage);
    SDL_FreeSurface(playerImage);
    SDL_FreeSurface(BlockImage);
}

// para o framerate
Uint32 start = 0;
const int fps = 30;
const int framerate =  1000/fps;

// essa classe representa a janela
class Janela
{
    private:
    // essa variável serve para ativar/desativar tela cheia
    bool windowed;

    // essa variável serve para ver se teve algum erro
    bool windowOK;

    public:
    // Construtor
    Janela()
    {
        // criar a janela
        tela = SDL_SetVideoMode( screen_width, screen_height, screen_bpp, SDL_SWSURFACE | SDL_RESIZABLE );

        // se tem algum erro
        if( tela == NULL )
        {
            windowOK = false;
            return;
        }
        else
        {
            windowOK = true;
        }

        // título da janela
        SDL_WM_SetCaption("Breakout", NULL);

        // inicia a variável
        windowed = true;
    }

    // segura os eventos da janela
    void handle_events()
    {
        // se tem algo de errado com a janela
        if( windowOK == false )
        {
            return;
        }

        // se a janela for redimensionada
        if( evento.type == SDL_VIDEORESIZE )
        {
            // Redimensiona a tela
            tela = SDL_SetVideoMode( evento.resize.w, evento.resize.h, screen_bpp, SDL_SWSURFACE | SDL_RESIZABLE );

            // se tem algum erro
            if( tela == NULL )
            {
                windowOK = false;
                return;
            }
        }
        // se apertou o botão enter
        else if( ( evento.type == SDL_KEYDOWN ) && ( evento.key.keysym.sym == SDLK_RETURN) )
        {
            // liga/desliga fullscreen/tela cheia
            toggle_fullscreen();
        }

        // se a tela da janela foi alterada
        else if( evento.type == SDL_VIDEOEXPOSE )
        {
            // se ao atualizar a tela teve erro
            if( SDL_Flip( tela ) == -1 )
            {
                windowOK = false;
                return;
            }
        }
    }

    // esse método liga/desliga fullscreen/tela cheia
    void toggle_fullscreen()
    {
        // se a tela estiver em janela
        if( windowed == true )
        {
            // ativa a tela cheia
            tela = SDL_SetVideoMode( screen_width, screen_height, screen_bpp, SDL_SWSURFACE | SDL_RESIZABLE | SDL_FULLSCREEN );

            // se tem algum erro
            if( tela == NULL )
            {
                windowOK = false;
                return;
            }

            // muda o estado da variável windowed
            windowed = false;
        }

        // se a tela não tá cheia
        else if( windowed == false )
        {
            // deixe ela diminuida
            tela = SDL_SetVideoMode( screen_width, screen_height, screen_bpp, SDL_SWSURFACE | SDL_RESIZABLE );

            // se a tela for 0
            if( tela == NULL )
            {
                windowOK = false;
                return;
            }

            // se não for 0
            windowed = true;
        }
    }

    // verifica se há algo de errado com a janela
    bool error()
    {
        return !windowOK;
    }
};


// use essa função pra desenhar texto na tela usando fonte ttf
void DrawText(int x, int y, char *text, Uint8 red, Uint8 green, Uint8 blue, TTF_Font *font)
{
    SDL_Surface *buffer = NULL;
    SDL_Color color = {red, green, blue};

    buffer = TTF_RenderText_Solid(font, text, color);

    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(buffer, NULL, tela, &mover);
    SDL_FreeSurface(buffer);
}

class _Player
{
public:
    int x;
    int y;
    int width;
    int height;
    int speed;
    int lives;
};

_Player player;

// posição da área background.bmp
const int GAMEAREA_X1 = 20;
const int GAMEAREA_X2 = 598;
const int GAMEAREA_Y1 = 20;
const int GAMEAREA_Y2 = 600;

// essa classe representa o bloco
class _Block
{
    public:
    int x;
    int y;
    int width;
    int height;
    bool visible; // true desenha // false não desenha
    int frame;
};

const int BLOCK_COLUMNS = 11;
const int BLOCK_ROWS = 5;

//array de objetos
_Block block[BLOCK_COLUMNS*BLOCK_ROWS];


// inicia posição x,y dos blocos
// tamanho width e height
// deixa eles visíveis
// corta quatro blocos frame i mod 4
void SetBlocks()
{
    // tamanho do bloco
    static int BLOCK_WIDTH  = 50;
    static int BLOCK_HEIGHT = 25;

    // cada i é um bloco
    int i = 0;

    for(int x = 0; x < BLOCK_COLUMNS; x++)
    {
        for(int y = 0; y < BLOCK_ROWS; y++)
        {
            block[i].x = x*BLOCK_WIDTH + GAMEAREA_X1 + x*3;
            block[i].y = (y*2) * BLOCK_HEIGHT + GAMEAREA_Y1 + y*3;
            block[i].width = BLOCK_WIDTH;
            block[i].height = BLOCK_HEIGHT;
            block[i].visible = true;
            block[i].frame = i % 4;
            i++;
        }
    }
}

// desenha os frames de blocos visíveis, ou seja, se a variável visible for true
void DrawBlocks()
{
    for(int i = 0; i < BLOCK_COLUMNS*BLOCK_ROWS; i++)
    {
        if(block[i].visible == true)
        {
            DrawImageFrame(block[i].x,block[i].y,BlockImage,tela,block[i].width,block[i].height,block[i].frame);
        }
    }
}

// conta o número de blocos visíveis
int NumBlocksLeft()
{
    int result = 0;

    for(int i = 0; i < BLOCK_COLUMNS*BLOCK_ROWS; i++)
    {
        if(block[i].visible == true)
        {
            result++;
        }
    }

    return result;
}


// inicia as propriedades dos objetos de classes
void ResetGame()
{
    player.x = (GAMEAREA_X2-GAMEAREA_X1)/2;
    player.y = 550;
    player.width = 100;
    player.height = 20;
    player.speed = 10;
    player.lives = 3;

    SetBlocks();
}

// use essa função pra física e controle do player
void MovePlayer()
{
    Uint8 *tecla = SDL_GetKeyState(NULL);

    if(tecla[SDLK_LEFT])
    {
        player.x = player.x - player.speed;
    }

    if(tecla[SDLK_RIGHT])
    {
        player.x = player.x + player.speed;
    }

    // colisão lado esquerdo
    if(player.x < GAMEAREA_X1)
    {
        player.x = GAMEAREA_X1;
    }

    // colisão lado direito
    if(player.x > GAMEAREA_X2-player.width)
    {
        player.x = GAMEAREA_X2-player.width;
    }

    /*
    if(tecla[SDLK_SPACE] && ball.IsLocked == true)
    {
        ball.IsLocked = false;
        ball.vx = rand()%3 -1;
        ball.vy = BALL_SPEED;
    }
    */
}


void DrawGame()
{
    DrawImage(0,0,backgroundImage);
    DrawImage(player.x,player.y,playerImage);
    DrawBlocks();

    if(NumBlocksLeft() <= 0)
    {
        ResetGame();
    }
}

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);
TTF_Init();

ResetGame();

iconImage = SDL_LoadBMP("gfx/icon.bmp");

SDL_WM_SetIcon(iconImage, 0);
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_SWSURFACE);

// cria o objeto myWindow
Janela myWindow;

LoadFiles();

// se a janela falhou
if( myWindow.error() == true )
{
    return 1;
}

// game loop
while(executando)
{
    start = SDL_GetTicks();
    while(SDL_PollEvent(&evento))
    {
        // segura os eventos de janela
        myWindow.handle_events();

        // se apertou o escape
        if( ( evento.type == SDL_KEYDOWN ) && ( evento.key.keysym.sym == SDLK_ESCAPE ) )
        {
            // fecha o programa
            executando = false;
        }

        // clicou no x da janela
        if(evento.type == SDL_QUIT)
        {
            executando = false; // fecha o programa
        }

        // se a janela falhou
        if( myWindow.error() == true )
        {
            return 1;
        }
    }

    SDL_FillRect(tela, 0, 0);

    DrawGame();
    MovePlayer();

    SDL_Flip(tela);
    if(framerate > (SDL_GetTicks()-start))
    {
        SDL_Delay(framerate - (SDL_GetTicks()-start));
    }
}


TTF_Quit();
SDL_Quit();
return 0;
}
