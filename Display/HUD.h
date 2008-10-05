// Heads up display.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OE_HUD_H_
#define _OE_HUD_H_

#include <Core/IListener.h>
#include <Renderers/IRenderer.h>
#include <Resources/ITextureResource.h>
#include <Math/Vector.h>
#include <list>

namespace OpenEngine {
namespace Display {

/**
 * Heads Up Display.
 *
 * @class HUD HUD.h Display/HUD.h
 */
class HUD : public Core::IListener<Renderers::RenderingEventArg> {
public:

    /**
     * Surface in the HUD.
     *
     * Can only be created by the HUD.
     * Deleting a surface will automatically remove it from the HUD.
     *
     * @see HUD::CreateSurface
     */
    class Surface {
    public:
        enum HorisontalAlignment { RIGHT, LEFT, MIDDLE };
        enum VerticalAlignment { TOP, BOTTOM, CENTER };

        virtual ~Surface();
        void MoveToFront();
        void MoveToBack();
        void SetPosition(const unsigned int x, const unsigned int y);
        void SetPosition(const HorisontalAlignment ha,
                         const VerticalAlignment va);
        void SetPosition(const Math::Vector<2,int> xy);
        Math::Vector<2,int> GetPosition() const;
    protected:
        friend class HUD;
        HUD& hud;
        std::list<Surface*>::iterator itr;
        Resources::ITextureResourcePtr texr;
        unsigned int x, y;
        Surface(HUD&,
                Resources::ITextureResourcePtr,
                unsigned int x,
                unsigned int y);
    };

    HUD();
    virtual ~HUD();
    virtual void Handle(Renderers::RenderingEventArg arg);
    virtual Surface* CreateSurface(Resources::ITextureResourcePtr texr,
                                   const unsigned int x=0,
                                   const unsigned int y=0);

protected:
    std::list<Surface*> surfaces;
    unsigned int width, height;
};

} // NS Display
} // NS OpenEngine

#endif // _OE_HUD_H_
