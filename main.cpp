#include <SDL.h>

SDL_Event evento;
SDL_Surface *tela = NULL;
bool executando = true;

const int screen_width = 640;
const int screen_height = 480;
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

SDL_Surface *iconImage = NULL;
// use essa função pra carregar arquivos
// nota: essa função só deve ser chamada no começo do programa
void LoadFiles()
{

}


// use essa função pra fechar arquivos
// nota: essa função só deve ser chamada no final do programa
void CloseFiles()
{
    SDL_FreeSurface(iconImage);
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

int main(int argc, char*args[])
{
SDL_Init(SDL_INIT_EVERYTHING);
iconImage = SDL_LoadBMP("gfx/icon.bmp");

SDL_WM_SetIcon(iconImage, 0);
tela = SDL_SetVideoMode(screen_width,screen_height,screen_bpp,SDL_SWSURFACE);

// cria o objeto myWindow
Janela myWindow;

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
    SDL_Flip(tela);
    if(framerate > (SDL_GetTicks()-start))
    {
        SDL_Delay(framerate - (SDL_GetTicks()-start));
    }
}

SDL_Quit();
return 0;
}
