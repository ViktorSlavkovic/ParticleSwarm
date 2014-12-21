#include <iostream>
#include <cstring>

#include "windows.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "swarm.h"

using namespace std;

const int w = GetSystemMetrics(SM_CXSCREEN);
const int h = GetSystemMetrics(SM_CYSCREEN);

SDL_Window * window;
SDL_Renderer * renderer;
SDL_Texture * texture;
Uint32 * canvas;

SDL_Surface * fps_surface;
SDL_Texture * fps_texture;
TTF_Font * font;
SDL_TimerID timer;

opt::swarm * S;

bool param_kb = false;      //known best
bool param_rw = false;      //rand()*w
uint64_t param_ss = 60000;  //swarm size
double param_vmax = 1;      //maximal valocity
double param_cog = 0.001;   //cognitive
double param_soc = 0.001;   //social
double param_in = 2;        //inertia
double param_dt = 1;        //delta time

bool param_disp_fps = false; //display_fps
bool param_lock_fps = false; //lock fps
int param_fps = 30;          //fps

int cx,cy,cfps;              //current mouse pos, fps counter

//DONE
double cost(double x, double y) {
    return (x-cx)*(x-cx)+(y-cy)*(y-cy);
}

//DONE
void Machine_Init() {

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)) {
        cout<<"ERROR: SDL_Init"<<endl;
        exit(-1);
    }
    cout<<"SDL_Init ... OK"<<endl;

    window = SDL_CreateWindow("Particle Swarm",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_ALLOW_HIGHDPI);

    cout<<"SDL_CreateWindow ... OK"<<endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED/*|SDL_RENDERER_PRESENTVSYNC*/);

    cout<<"SDL_CreateRenderer ... OK"<<endl;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, w, h);

    cout<<"SDL_CreateTexture ... OK"<<endl;

    canvas = new Uint32[w * h];

    SDL_SetRenderDrawColor(renderer,255,0,0,255);

    if (TTF_Init() != 0){
        cout<<"ERROR: TTF_Init"<<endl;
        SDL_Quit();
        exit(-1);
    }

    if ((font = TTF_OpenFont("font.ttf", 12))==0) {
        cout<<"ERROR: TTF_OpenFont"<<endl;
        TTF_Quit();
        SDL_Quit();
        exit(-1);
    }
}

//DONE
void Machine_Clear() {

    delete[] canvas;

    SDL_RemoveTimer(timer);
    SDL_DestroyTexture(fps_texture);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    cout<<"Cleaning ... OK"<<endl;
}

//DONE
void Machine_setPixel(int x, int y, Uint8 color) {
    canvas[y * w + x] = color;
}

//DONE
void Machine_Draw() {
    for(uint64_t i=0; i<S->getSwarmSize(); i++) {
        long long x = (int)(S->getParticlePos_x(i));
        long long y = (int)(S->getParticlePos_y(i));
        Machine_setPixel(x,y,0);
    }
}

//DONE
void Machine_Render() {
    memset(canvas, 255, w*h*sizeof(Uint32));
    Machine_Draw();
    SDL_UpdateTexture(texture, NULL, canvas, w * sizeof(Uint32));
    //SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    //FPS !
    if (param_disp_fps) {

        SDL_sem* dataLock = SDL_CreateSemaphore( 1 );

        SDL_Rect dst;
        dst.x = 5;
        dst.y = 5;
        SDL_QueryTexture(fps_texture, NULL, NULL, &dst.w, &dst.h);
        if (param_disp_fps) SDL_RenderCopy(renderer, fps_texture, NULL, &dst);
        cfps++;

        SDL_DestroySemaphore(dataLock);
    }
    //end FPS

    SDL_RenderDrawLine(renderer,cx,cy-10,cx,cy+10);
    SDL_RenderDrawLine(renderer,cx-10,cy,cx+10,cy);
    SDL_RenderPresent(renderer);

}

//DONE
Uint32 Machine_callbackFPS(Uint32 interval, void *param) {

    if (!param_disp_fps) return 0;

    int pom=cfps;
    cfps=0;

    string s;
    ostringstream sout(s);
    sout<<"FPS: "<<pom;

    fps_surface = TTF_RenderText_Blended(font, sout.str().c_str(), { 255, 0, 0, 255 });
    fps_texture = SDL_CreateTextureFromSurface(renderer, fps_surface);
    SDL_FreeSurface(fps_surface);

    return interval;
}

//DONE
void Machine_Loop() {

    SDL_Event event;
    bool quit = false;
    bool lmbd = false;
    bool start=false;

    timer = SDL_AddTimer(1000, Machine_callbackFPS, 0);

    Uint32 t1;
    double ft=1000.0/param_fps;

    while (!quit) {

        t1=SDL_GetTicks();

        if (start) if (param_kb) S->update(param_dt,cx,cy, param_rw);
                   else S->update(param_dt, param_rw);

        Machine_Render();

        if (SDL_PollEvent(&event))
            switch (event.type) {

                case SDL_QUIT:
                    quit = true;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) quit=true;
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        lmbd = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                        lmbd = true;
                        start = true;

                case SDL_MOUSEMOTION:
                    if (lmbd)
                    {
                        int mouseX = event.motion.x;
                        int mouseY = event.motion.y;
                        cx=mouseX;
                        cy=mouseY;
                    }
                    break;
            }

            if (param_lock_fps) {
                Uint32 t2=SDL_GetTicks();
                Uint32 delta=t2-t1;
                if (delta < (Uint32)ft) SDL_Delay((Uint32)ft-delta);
            }
    }
}

//DONE
void APP_handle_args(int argc, char ** argv) {
    int i=1;
    while (i<argc) {
        if (strcmp(argv[i],"-swarm_size")==0) {
            i++;
            if (i>=argc || (param_ss=(int)strtoull(argv[i],0,10))==0) {
                cout<<"ERROR: Check argument: "<<argv[i-1]<<endl;
                exit(0);
            }
            i++;
            continue;
        }
        if (strcmp(argv[i],"-cognitive")==0) {
            i++;
            if (i>=argc || (param_cog=strtod(argv[i],0))==0) {
                cout<<"ERROR: Check argument: "<<argv[i-1]<<endl;
                exit(0);
            }
            i++;
            continue;
        }
        if (strcmp(argv[i],"-social")==0) {
            i++;
            if (i>=argc || (param_soc=strtod(argv[i],0))==0) {
                cout<<"ERROR: Check argument: "<<argv[i-1]<<endl;
                exit(0);
            }
            i++;
            continue;
        }
        if (strcmp(argv[i],"-inertia")==0) {
            i++;
            if (i>=argc || (param_in=strtod(argv[i],0))==0) {
                cout<<"ERROR: Check argument: "<<argv[i-1]<<endl;
                exit(0);
            }
            i++;
            continue;
        }
        if (strcmp(argv[i],"-v_max")==0) {
            i++;
            if (i>=argc || (param_vmax=strtod(argv[i],0))==0) {
                cout<<"ERROR: Check argument: "<<argv[i-1]<<endl;
                exit(0);
            }
            i++;
            continue;
        }
        if (strcmp(argv[i],"-dt")==0) {
            i++;
            if (i>=argc || (param_dt=strtod(argv[i],0))==0) {
                cout<<"ERROR: Check argument: "<<argv[i-1]<<endl;
                exit(0);
            }
            i++;
            continue;
        }
        if (strcmp(argv[i],"-known_gb")==0) {
            param_kb = true;
            i++;
            continue;
        }
        if (strcmp(argv[i],"-rand_w")==0) {
            param_rw = true;
            i++;
            continue;
        }
        if (strcmp(argv[i],"-display_fps")==0) {
            param_disp_fps = true;
            i++;
            continue;
        }
        if (strcmp(argv[i],"-lock_fps")==0) {
            param_lock_fps = true;
            i++;
            if (i>=argc || (param_fps=(int)strtoull(argv[i],0,10))==0) {
                cout<<"ERROR: Check argument: "<<argv[i-1]<<endl;
                exit(0);
            }
            i++;
            continue;
        }
        else {
            cout<<"ERROR: Ivalid argument: "<<argv[i]<<endl;
            exit(0);
        }
    }
}

//DONE
int main(int argc, char ** argv) {

    APP_handle_args(argc, argv);

    Machine_Init();

    S = new opt::swarm(cost, param_ss, param_vmax, 0, w-1, 0, h-1);
    S->setCognitive(param_cog);
    S->setSocial(param_soc);
    S->setInertia(param_in);

    Machine_Loop();

    delete S;

    Machine_Clear();

    return 0;
}

