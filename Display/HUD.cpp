// Heads Up Display.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include <Meta/OpenGL.h> // @todo: remove this, please

#include <Display/HUD.h>
#include <Math/Vector.h>
#include <Geometry/Face.h>

#include <Display/OrthogonalViewingVolume.h>

namespace OpenEngine {
namespace Display {

using std::list;
using OpenEngine::Math::Vector;
using OpenEngine::Geometry::Face;
using OpenEngine::Geometry::FacePtr;
using OpenEngine::Display::OrthogonalViewingVolume;
using OpenEngine::Renderers::RenderingEventArg;
using OpenEngine::Resources::ITextureResourcePtr;

HUD::HUD() {
    width = 800;
    height = 600;
}

HUD::~HUD() {
    list<Surface*>::iterator itr;
    for (itr=surfaces.begin(); itr != surfaces.end(); itr++)
        delete *itr;
    surfaces.clear();
}

/**
 * Redraw on a rendering event.
 * Usually you will want to attach this to the post-process event of
 * your renderer. 
 */
void HUD::Handle(RenderingEventArg arg) {
    // @todo: listen for frame changes so our HUD is correct
    OrthogonalViewingVolume* ot = new OrthogonalViewingVolume(-1, 1, 0, width, 0, height);
    arg.renderer.ApplyViewingVolume(*ot);
    delete ot;

    // We need to disabled the depth test otherwise we will not draw
    // things in the correct order.
    // @todo: Do this in a general way (read: not gl specific)
    bool depth = glIsEnabled(GL_DEPTH_TEST);
    if (depth) glDisable(GL_DEPTH_TEST);

    GLboolean blending = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLboolean lighting = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHTING);
    

    Vector<3,float> v1,v2,v3,v4,n;
    Vector<2,float> t1,t2,t3,t4;
    unsigned int x, y, w, h;
    list<Surface*>::iterator itr;
    for (itr=surfaces.begin(); itr != surfaces.end(); itr++) {
        //(*itr)->texr->Load();
        if ((*itr)->texr->GetID() == 0)
            arg.renderer.LoadTexture((*itr)->texr);
        //(*itr)->texr->Unload();

        x = (*itr)->x;
        y = (*itr)->y;
        w = (*itr)->texr->GetWidth();
        h = (*itr)->texr->GetHeight();
        if (w*h == 0) continue;

        n  = Vector<3,float>(0,0,1);
        v1 = Vector<3,float>(x,   y,   0);
        v2 = Vector<3,float>(x,   y+h, 0);
        v3 = Vector<3,float>(x+w, y+h, 0);
        v4 = Vector<3,float>(x+w, y,   0);
        t2 = Vector<2,float>(0, 0);
        t1 = Vector<2,float>(0, 1);
        t4 = Vector<2,float>(1, 1);
        t3 = Vector<2,float>(1, 0);

        FacePtr f = FacePtr(new Face(v1,v2,v3,n,n,n));
        f->mat->texr = (*itr)->texr;
        f->vert[0] = v1; f->vert[1] = v2; f->vert[2] = v3;
        f->texc[0] = t1; f->texc[1] = t2; f->texc[2] = t3;
        arg.renderer.DrawFace(f);
        f->vert[0] = v1; f->vert[1] = v3; f->vert[2] = v4;
        f->texc[0] = t1; f->texc[1] = t3; f->texc[2] = t4;
        arg.renderer.DrawFace(f);
    }

    if (depth) glEnable(GL_DEPTH_TEST);
    if (!blending) glDisable(GL_BLEND); //@todo: apply original blend function
    if (lighting) glEnable(GL_LIGHTING);
}

/**
 * Create a new surface from a texture.
 * To remove the surface from the HUD, simply delete it.
 */
HUD::Surface* HUD::CreateSurface(ITextureResourcePtr texr,
                                 const unsigned int x,
                                 const unsigned int y) {
    Surface* w = new Surface(*this, texr, x, y);
    surfaces.push_back(w);
    // save the iterator
    w->itr = --surfaces.end();
    return w;
}

HUD::Surface::Surface(HUD& hud,
                      ITextureResourcePtr texr,
                      unsigned int x,
                      unsigned int y)
    : hud(hud)
    , texr(texr)
    , x(x)
    , y(y) {
    
}

/**
 * Deleting a surface will remove it from the HUD.
 */
HUD::Surface::~Surface() {
    hud.surfaces.erase(itr);
}

/**
 * Move the surface to the front of the HUD.
 */
void HUD::Surface::MoveToFront() {
    hud.surfaces.erase(itr);    // invalidation
    hud.surfaces.push_back(this);
    itr = --hud.surfaces.end(); // new iterator
}

/**
 * Move the surface to the back of the HUD.
 */
void HUD::Surface::MoveToBack() {
    hud.surfaces.erase(itr);      // invalidation
    hud.surfaces.push_front(this);
    itr = hud.surfaces.begin();   // new iterator
}

/**
 * Set the position of the surface on screen.
 */
void HUD::Surface::SetPosition(const unsigned int x,
                               const unsigned int y) {
    this->x = x;
    this->y = y;
}

/**
 * Set the position of the surface on screen.
 */
void HUD::Surface::SetPosition(const HorisontalAlignment ha,
                               const VerticalAlignment va) {
    switch(ha) {
    case LEFT: x = 0; break;
    case RIGHT: x = hud.width - texr->GetWidth(); break;
    case MIDDLE: x = (hud.width - texr->GetWidth()) / 2; break;
    }
    
    switch(va) {
    case TOP: y = 0; break;
    case BOTTOM: y = hud.height - texr->GetHeight(); break;
    case CENTER: y = (hud.height - texr->GetHeight()) / 2; break;
    }
}

/**
 * Set the position of the surface on screen.
 */
void HUD::Surface::SetPosition(const Vector<2,int> xy) {
    x = xy.Get(0);
    y = xy.Get(1);
}

/**
 * Get the position of the surface on screen.
 */
Vector<2,int> HUD::Surface::GetPosition() const {
    return Vector<2,int>(x,y);
}

} // NS Display
} // NS OpenEngine
