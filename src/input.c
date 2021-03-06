/*

Gravit - A gravity simulator
Copyright 2003-2005 Gerald Kaszuba

Gravit is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Gravit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Gravit; if not, write to the Free Software
Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#include "gravit.h"

#ifndef NO_GUI

int hasCtrlOrCmdModifier() {
    
    return (SDL_GetModState() & KMOD_LALT || SDL_GetModState() & KMOD_RALT || SDL_GetModState() & KMOD_LMETA || SDL_GetModState() & KMOD_RMETA);
    
}

int processKeys() {

    SDL_Event event;

    while ( SDL_PollEvent( &event ) ) {

        if (event.type == SDL_QUIT) {
            cmdQuit(NULL);
            return 1;
        }

#ifndef WITHOUT_AGAR
        {
            AG_DriverEvent ev;
            int x;
            int y;
            AG_SDL_TranslateEvent(agDriverSw, &event, &ev);
            if (ev.type == AG_DRIVER_MOUSE_BUTTON_DOWN) {
                x = ev.data.button.x;
                y = ev.data.button.y;
                if (AG_WindowFocusAtPos(agDriverSw, x, y)) {
                    AG_ProcessEvent(0, &ev);
                    return 0;
                }
            } else {
                // if console is open, do not forward keyboard events to agar
	         if ( ! (view.consoleMode && ((event.type == SDL_KEYUP)||(event.type == SDL_KEYDOWN))) )
                    AG_ProcessEvent(0, &ev);
            }
        }
#endif
        
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            view.dirty = 1;

            if (view.screenSaver) {
                cmdQuit(0);
                return 1;
            }

            if (event.button.button == SDL_BUTTON_WHEELDOWN)
                view.zoomTarget *= (1 + (view.deltaVideoFrame * 0.01f));

            if (event.button.button == SDL_BUTTON_WHEELUP)
                view.zoomTarget *= (1 + (view.deltaVideoFrame * -0.01f));

#ifndef WITHOUT_AGAR
           // code introduced together with agar
            if (event.button.button == SDL_BUTTON_LEFT) {
                view.mouseButtons[0] = SDL_BUTTON(1);
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                view.mouseButtons[0] = SDL_BUTTON(3);
            }
            
        }
        
        if (event.type == SDL_MOUSEBUTTONUP) {
            view.dirty = 1;
            if (event.button.button == SDL_BUTTON_LEFT) {
                view.mouseButtons[0] = 0;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                view.mouseButtons[0] = 0;
            }
#endif
        }

        if (event.type == SDL_KEYUP) {
            view.keys[event.key.keysym.sym] = 0;
            if (view.screenSaver) {
                cmdQuit(0);
                return 1;
            }
        }

        if (event.type == SDL_KEYDOWN) {

            view.keys[event.key.keysym.sym] = 1;

            if (view.consoleMode) {
                conInput(event.key.keysym.sym, event.key.keysym.mod, event.key.keysym.unicode);
                return 0;
            }

            switch (event.key.keysym.sym) {

            case SDLK_ESCAPE:
                if (view.consoleMode) {
                    view.consoleMode = 0;
                    break;

                } else if (view.textMode == TM_HELP1 || view.textMode == TM_HELP2) {
                    view.textMode = TM_STANDARD;
                    break;
                }

                cmdQuit(NULL);
                return 1;
                break;

            case SDLK_RETURN:
                if (hasCtrlOrCmdModifier()) {
                    video.screenFS = !video.screenFS;
                    cmdVideoRestart(0);
                }
                break;

            case SDLK_F1:
                if (view.textMode == TM_HELP1)
                    view.textMode = TM_STANDARD;
                else
                    view.textMode = TM_HELP1;
                break;

            case SDLK_F2:
                if (view.textMode == TM_HELP2)
                    view.textMode = TM_STANDARD;
                else
                    view.textMode = TM_HELP2;
                break;

            case SDLK_F5:
                cmdPlay(NULL);
                break;

            case SDLK_F6:
                cmdRecord(NULL);
                break;

            case SDLK_F7:
                cmdStop(NULL);
                break;

            case SDLK_SPACE:
            case SDLK_F8:
                cmdStart(NULL);
                cmdRecord(NULL);
                break;

            case SDLK_F9:
                cmdScreenshot(NULL);
                break;

            case SDLK_F10:
                if (view.screenshotLoop) {
                    view.screenshotLoop = 0;
                    conAdd(LNORM, "Stopped taking many screenshots! Check your screenshots directory.");
                } else {
                    view.screenshotLoop = 1;
                    conAdd(LNORM, "Taking a screenshot every frame. Careful how long you run this!");
                }
                break;

            case SDLK_s:
                // save (control-s)
                if (hasCtrlOrCmdModifier())
                    cmdSaveFrameDump(NULL);
                // toggle stereo mode
                // mode 1 : side-by-side
                // mode 2 : anaglyph, offaxis frustum
                else {
                    if (view.stereoMode > 1)
                        view.stereoMode = 0;
                    else {
                        view.stereoMode ++;
                        cmdStereoWarning(0);
                    }

                    if (view.stereoMode ==2 ) {
                        conAdd(LNORM, "stereoMode set to %i (red-cyan 3D glasses)", view.stereoMode);
		    } else {
                        if (view.stereoMode ==1)
                           conAdd(LNORM, "stereoMode set to %i (freeview 3D)", view.stereoMode);
			else
                           conAdd(LNORM, "stereoMode set to %i", view.stereoMode);
		    }

                    setColours();
                }
                break;

            case SDLK_BACKQUOTE:
                view.consoleMode = (!view.consoleMode)?1:0;
                break;

            case SDLK_CARET:
            case SDLK_1:
                view.consoleMode = 1;
                break;

            case SDLK_t:
                if (++view.drawTree == 3)
                    view.drawTree = 0;
                conAdd(LNORM, "drawTree set to %i", view.drawTree);
                break;

            case SDLK_SLASH:
                view.particleColourMode ++;
                if (view.particleColourMode == CM_LAST)
                    view.particleColourMode = 0;
                conAdd(LNORM, "Colour mode set to: %s" , colourModes[view.particleColourMode]);
                setColours();
                break;

            case SDLK_l:
                view.blendMode ++;
                if (view.blendMode == 5)
                    view.blendMode = 0;
                conAdd(LNORM, "Blend mode set to: %i", view.blendMode);
                break;

            case SDLK_BACKSLASH:
                view.particleRenderMode++;
                if (view.particleRenderMode == 3)
                    view.particleRenderMode = 0;
                conAdd(LNORM, "particleRenderMode set to %i" , view.particleRenderMode);
                setColours();
                break;

            case SDLK_MINUS:
                view.particleSizeMin /= 2;
                if (view.particleSizeMin < 1)
                    view.particleSizeMin = 1;
                conAdd(LLOW, "particlesizemin set to %.1f ", view.particleSizeMin);
                break;

            case SDLK_EQUALS:
                if (view.particleSizeMin < 1)
                    view.particleSizeMin = 1;
                else
                    view.particleSizeMin *= 2;
                conAdd(LLOW, "particlesizemin set to %.1f ", view.particleSizeMin);
                if (view.particleSizeMin > view.particleSizeMax)
                    conAdd(LNORM, "Careful! particlesizemin is bigger then particlesizemax!");
                break;

            case SDLK_LEFTBRACKET:
                view.particleSizeMax /= 2;
                if (view.particleSizeMax < 1)
                    view.particleSizeMax = 1;
                conAdd(LLOW, "particlesizemax set to %.1f ", view.particleSizeMax);
                if (view.particleSizeMin > view.particleSizeMax)
                    conAdd(LNORM, "Careful! particlesizemin is bigger then particlesizemax!");
                break;

            case SDLK_RIGHTBRACKET:
                if (view.particleSizeMax < 1)
                    view.particleSizeMax = 1;
                else
                    view.particleSizeMax *= 2;
                conAdd(LLOW, "particlesizemax set to %.1f ", view.particleSizeMax);
                break;

            case SDLK_m:
                if (view.tailLength == -1) {
                    view.tailLength = 0;
                    conAdd(LLOW, "tailLength set to 0");
                } else {
                    view.tailLength = -1;
                    conAdd(LLOW, "tailLength set to infinite!");
                }
                break;

            case SDLK_n:
                if (view.tailLength <= 0)
                    view.tailLength = 1;
                else
                    view.tailLength *= 2;
                conAdd(LLOW, "tailLength set to %i", view.tailLength);
                break;

            case SDLK_b:
                if (view.tailLength < 0)
                    view.tailLength = 0;

                if (view.tailLength == 1)
                    view.tailLength = 0;
                else
                    view.tailLength /= 2;

                conAdd(LLOW, "tailLength set to %i", view.tailLength);
                break;

            case SDLK_v:
                view.tailOpacity += 0.1f;
                if (view.tailOpacity > 1)
                    view.tailOpacity = 1;
                conAdd(LLOW, "tailOpacity set to %.1f", view.tailOpacity);
                break;

            case SDLK_c:
                view.tailOpacity -= 0.1f;
                if (view.tailOpacity < 0)
                    view.tailOpacity = 0;
                conAdd(LLOW, "tailOpacity set to %.1f", view.tailOpacity);
                break;

            case SDLK_x:
                view.tailFaded = (!view.tailFaded)?1:0;
                conAdd(LLOW, "tailFaded set to %i", view.tailFaded);
                break;

            case SDLK_COMMA:
                view.tailSkip /= 2;
                if (view.tailSkip < 1)
                    view.tailSkip = 1;
                conAdd(LLOW, "tailSkip set to %i", view.tailSkip);
                break;

            case SDLK_PERIOD:
                view.tailSkip *= 2;
                conAdd(LLOW, "tailSkip set to %i", view.tailSkip);
                break;

            case SDLK_QUOTE:
                view.tailWidth += 1;
                if (view.tailWidth > 10)
                    view.tailWidth = 10;
                conAdd(LLOW, "tailWidth set to %.0f", view.tailWidth);
                break;

            case SDLK_SEMICOLON:
                view.tailWidth -= 1;
                if (view.tailWidth < 1)
                    view.tailWidth = 1;
                conAdd(LLOW, "tailWidth set to %.0f", view.tailWidth);
                break;

            case SDLK_o:
                view.drawOSD++;
                if (view.drawOSD == 3) view.drawOSD = 4;
                if (view.drawOSD > 4) view.drawOSD=0;
                view.drawColourScheme = (view.drawOSD == 1) ? 1 : 0;
                conAdd(LLOW, "drawOSD set to %i", view.drawOSD);
                break;

            case SDLK_p:
                view.drawSky ++;
                if(view.drawSky > SKYBOX_LAST) view.drawSky = 0;
                conAdd(LLOW, "drawSky set to %i", view.drawSky);
                break;

            case SDLK_g:
                view.glow ++;
                if(view.glow > 8)
                {
                    view.glow = 0;
                    conAdd(LLOW, "Star glow disabled");
                } else {
                    conAdd(LLOW, "Star glow set to %i", view.glow);
                }
                setColours();
                break;

            case SDLK_q:
                if (hasCtrlOrCmdModifier()) {
                    cmdQuit(NULL);  // OS X cmd-q
                    return 1;
                }
                view.frameSkip--;
                view.frameSkipCounter = 0;
                conAdd(LLOW, "frameSkip set to %i", view.frameSkip);
                break;

            case SDLK_w:
                view.frameSkip++;
                view.frameSkipCounter = 0;
                conAdd(LLOW, "frameSkip set to %i", view.frameSkip);
                break;

            case SDLK_d:
                view.stereoSeparation--;
                conAdd(LLOW, "stereoSeparation set to %.1f", view.stereoSeparation);
                break;

            case SDLK_f:
                view.stereoSeparation++;
                conAdd(LLOW, "stereoSeparation set to %.1f", view.stereoSeparation);
                break;

            default:
                break;

            }

        }

    }

    if (!view.consoleMode) {

        if (view.keys[SDLK_a]) {
            view.zoom /= (1 + (view.deltaVideoFrame * 0.005f));
            view.zoomTarget = view.zoom;
            view.zoomSpeed = 0;
            view.dirty = 1;
        }
        if (view.keys[SDLK_z]) {
            view.zoom *= (1 + (view.deltaVideoFrame * 0.005f));
            view.zoomTarget = view.zoom;
            view.zoomSpeed = 0;
        }

        // allow arrow keys instead of mouse dragging

        if (view.keys[SDLK_PAGEUP]) {
            view.zoomTarget *= (1 + (view.deltaVideoFrame * -0.005f));
            view.dirty = 1;
        }
        if (view.keys[SDLK_PAGEDOWN]) {
            view.zoomTarget *= (1 + (view.deltaVideoFrame * 0.005f));
            view.dirty = 1;
        }


        if (view.keys[SDLK_UP]) {
            view.rotTarget[0] -= view.deltaVideoFrame * 0.1f;
            view.dirty = 1;
            if (view.drawAxis==1) view.drawAxis=3;
        }
        if (view.keys[SDLK_DOWN]) {
            view.rotTarget[0] += view.deltaVideoFrame * 0.1f;
            view.dirty = 1;
            if (view.drawAxis==1) view.drawAxis=3;
        }
        if (view.keys[SDLK_LEFT]) {
            view.rotTarget[1] -= view.deltaVideoFrame * 0.1f;
            view.dirty = 1;
            if (view.drawAxis==1) view.drawAxis=3;
        }
        if (view.keys[SDLK_RIGHT]) {
            view.rotTarget[1] += view.deltaVideoFrame * 0.1f;
            view.dirty = 1;
            if (view.drawAxis==1) view.drawAxis=3;
        }


        /*
        		if (view.keys[SDLK_UP])
        			view.face[0] -= state.dt * 0.1f;

        		if (view.keys[SDLK_DOWN])
        			view.face[0] += state.dt * 0.1f;

        		if (view.keys[SDLK_LEFT])
        			view.face[1] -= state.dt * 0.1f;

        		if (view.keys[SDLK_RIGHT])
        			view.face[1] += state.dt * 0.1f;
        */

    }

    return 0;

}

void processMouse() {

    int x,y;

#ifdef WITHOUT_AGAR
    // old code
    view.mouseButtons[1] = view.mouseButtons[0];
    memcpy(view.lastMousePosition, view.currentMousePosition, sizeof(view.currentMousePosition));
    view.mouseButtons[0] = SDL_GetMouseState(&view.currentMousePosition[0], &view.currentMousePosition[1]);
#else
    // newer code introduced together with agar
    memcpy(view.lastMousePosition, view.currentMousePosition, sizeof(view.currentMousePosition));
    SDL_GetMouseState(&view.currentMousePosition[0], &view.currentMousePosition[1]);
#endif
    x = view.currentMousePosition[0] - view.lastMousePosition[0];
    y = view.currentMousePosition[1] - view.lastMousePosition[1];

    // if it's a screensaver, and the mouse has moved, and the simulation has been running for a bit...
    if (view.screenSaver && (x || y) && (getMS() - view.firstTimeStamp > 1000)) {
        cmdQuit(0);
        return;
    }

    if ((view.mouseButtons[0] & SDL_BUTTON(1)) || (view.mouseButtons[0] & SDL_BUTTON(3))) {

        // Unfortunately, on OS X WarpMouse seems to act like the mouse isn't pressed anymore.
        
#ifndef __MACH__
        
        Uint16 wx,wy;
        wx = video.screenW / 2;
        wy = video.screenH / 2;
        SDL_WarpMouse(wx,wy);
        view.currentMousePosition[0] = wx;
        view.currentMousePosition[1] = wy;

#endif

        // turn off cursor only after the 2nd warp mouse, otherwise showcursor does strange things.
        if ((view.mouseButtons[1] & SDL_BUTTON(1)) || (view.mouseButtons[1] & SDL_BUTTON(3))) {
            SDL_ShowCursor(0);
        }

        if (view.mouseButtons[0] & SDL_BUTTON(1)) {
            // left mousebutton --> rotate around x / y axis
            view.rotTarget[1] += 0.5 * x;
            view.rotTarget[0] += 0.5 * y;
        } else {
	    // right mousebutton --> rotate around y / z axis
            view.rotTarget[1] += 0.5 * x;
            view.rotTarget[2] -= 0.5 * y;
        }
        view.dirty = 1;
        if (view.drawAxis==1) view.drawAxis=3;

    } else {

        SDL_ShowCursor(view.showCursor);

    }
#ifndef WITHOUT_AGAR
    
    // code introduced together with agar
    view.mouseButtons[1] = view.mouseButtons[0];
#endif
}

#else

int processKeys() {

    int key;

#ifdef WIN32

    while (_kbhit()) {
        key = _getche();
        conInput(key);
        if (key == 13 || key == 10) {
            break;
        }
    }

#else

char numbytes;
fd_set rfds;
struct timeval tv;
int retval;

FD_ZERO(&rfds);
FD_SET(0, &rfds);
tv.tv_sec = 0;
tv.tv_usec = 1;

while ((retval = select(1, &rfds, NULL, NULL, &tv))) {

    if (retval) {

        numbytes = read(0,&key, 1); // = getchar();
        if (numbytes)
            conInput(key);
    }

}

#endif

    return 0;

}

#endif
