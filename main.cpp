#include <SDL.h>
#include <SDL_ttf.h>

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 800;
const int screen_height = 600;
const int screen_bpp = 32;

// use essa fun��o pra carregar uma imagem.bmp e deixa o fundo transparente
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


// use essa fun��o pra desenhar uma imagem na tela
void DrawImage(int x, int y, SDL_Surface *image)
{
    SDL_Rect mover;
    mover.x = x;
    mover.y = y;

    SDL_BlitSurface(image, NULL, tela, &mover);
}

SDL_Surface *iconImage = NULL;
SDL_Surface *backgroundImage = NULL;
SDL_Surface *playerImage     = NULL;

TTF_Font *ttfFile = NULL;

// use essa fun��o pra carregar arquivos
// nota: essa fun��o s� deve ser chamada no come�o do programa
void LoadFiles()
{
    ttfFile = TTF_OpenFont("fontes/times.ttf", 40);
    backgroundImage = SDL_LoadBMP("gfx/background.bmp");
    playerImage     = SDL_LoadBMP("gfx/player.bmp");
}


// use essa fun��o pra fechar arquivos
// nota: essa fun��o s� deve ser chamada no final do programa
void CloseFiles()
{
    SDL_FreeSurface(iconImage);
    SDL_FreeSurface(backgroundImage);
    SDL_FreeSurface(playerImage);
    TTF_CloseFont(ttfFile);
}

// para o framerate
Uint32 start = 0;
const int fps = 30;
const int framerate =  1000/fps;

// essa classe representa a janela
class Janela
{
    private:
    // essa vari�vel serve para ativar/desativar tela cheia
    bool windowed;

    // essa vari�vel serve para ver se teve algum erro
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

        // t�tulo da janela
        SDL_WM_SetCaption("Breakout", NULL);

        // inicia a vari�vel
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
        // se apertou o bot�o enter
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

    // esse m�todo liga/desliga fullscreen/tela cheia
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

            // muda o estado da vari�vel windowed
            windowed = false;
        }

        // se a tela n�o t� cheia
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

            // se n�o for 0
            windowed = true;
        }
    }

    // verifica se h� algo de errado com a janela
    bool error()
    {
        return !windowOK;
    }
};


// use essa fun��o pra desenhar texto na tela usando fonte ttf
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

// posi��o da �rea background.bmp
const int GAMEAREA_X1 = 20;
const int GAMEAREA_X2 = 598;
const int GAMEAREA_Y1 = 20;
const int GAMEAREA_Y2 = 600;

void ResetGame()
{
    player.x = (GAMEAREA_X2-GAMEAREA_X1)/2;
    player.y = 550;
    player.width = 100;
    player.height = 20;
    player.speed = 10;
    player.lives = 3;
}

// use essa fun��o pra f�sica e controle do player
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

    // colis�o lado esquerdo
    if(player.x < GAMEAREA_X1)
    {
        player.x = GAMEAREA_X1;
    }

    // colis�o lado direito
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
