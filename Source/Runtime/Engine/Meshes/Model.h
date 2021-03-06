// ===========================================================================
// 
// HevEn (C) 2017 by Hevedy <https://github.com/Hevedy>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, 
// You can obtain one at https://mozilla.org/MPL/2.0/.
// 
// ---------------------------------------------------------------------------
// 
// Sauerbraten game engine source code, any release.
// Tesseract game engine source code, any release.
// 
// Copyright (C) 2001-2017 Wouter van Oortmerssen, Lee Salzman, Mike Dysart, Robert Pointon, and Quinton Reeves
// Copyright (C) 2001-2017 Wouter van Oortmerssen, Lee Salzman, Mike Dysart, Robert Pointon, Quinton Reeves, and Benjamin Segovia
// 
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// 
// ---------------------------------------------------------------------------
//  File:			Model.h
//  Description: 	Models header
// ---------------------------------------------------------------------------
//  Log:			Source.
//
//
// ===========================================================================


enum { MDL_MD2 = 0, MDL_MD3, MDL_MD5, MDL_OBJ, MDL_SMD, MDL_IQM, NUMMODELTYPES };

struct model
{
    char *name;
    float spinyaw, spinpitch, spinroll, offsetyaw, offsetpitch, offsetroll;
    bool shadow, alphashadow, depthoffset;
    float scale;
    vec translate;
    BIH *bih;
    vec bbcenter, bbradius, bbextend, collidecenter, collideradius;
    float rejectradius, eyeheight, collidexyradius, collideheight;
    char *collidemodel;
    int collide, batch;

    model(const char *name) : name(name ? newstring(name) : NULL), spinyaw(0), spinpitch(0), spinroll(0), offsetyaw(0), offsetpitch(0), offsetroll(0), shadow(true), alphashadow(true), depthoffset(false), scale(1.0f), translate(0, 0, 0), bih(0), bbcenter(0, 0, 0), bbradius(-1, -1, -1), bbextend(0, 0, 0), collidecenter(0, 0, 0), collideradius(-1, -1, -1), rejectradius(-1), eyeheight(0.9f), collidexyradius(0), collideheight(0), collidemodel(NULL), collide(COLLIDE_OBB), batch(-1) {}
    virtual ~model() { DELETEA(name); DELETEP(bih); }
    virtual void calcbb(vec &center, vec &radius) = 0;
    virtual void calctransform(matrix4x3 &m) = 0;
    virtual int intersect(int anim, int basetime, int basetime2, const vec &pos, float yaw, float pitch, float roll, dynent *d, modelattach *a, float size, const vec &o, const vec &ray, float &dist, int mode) = 0;
    virtual void render(int anim, int basetime, int basetime2, const vec &o, float yaw, float pitch, float roll, dynent *d, modelattach *a = NULL, float size = 1, const vec4 &color = vec4(1, 1, 1, 1)) = 0;
    virtual bool load() = 0;
    virtual int type() const = 0;
    virtual BIH *setBIH() { return NULL; }
    virtual bool envmapped() const { return false; }
    virtual bool skeletal() const { return false; }
    virtual bool animated() const { return false; }
    virtual bool pitched() const { return true; }
    virtual bool alphatested() const { return false; }

    virtual void setshader(Shader *shader) {}
    virtual void setenvmap(float envmapmin, float envmapmax, Texture *envmap) {}
    virtual void setspec(float spec) {}
    virtual void setgloss(int gloss) {}
    virtual void setglow(float glow, float glowdelta, float glowpulse) {}
    virtual void setalphatest(float alpha) {}
    virtual void setfullbright(float fullbright) {}
    virtual void setcullface(int cullface) {}
    virtual void setcolor(const vec &color) {}

    virtual void genshadowmesh(vector<triangle> &tris, const matrix4x3 &orient) {}
    virtual void preloadBIH() { if(!bih) setBIH(); }
    virtual void preloadshaders() {}
    virtual void preloadmeshes() {}
    virtual void cleanup() {}

    virtual void startrender() {}
    virtual void endrender() {}

    void boundbox(vec &center, vec &radius)
    {
        if(bbradius.x < 0)
        {
            calcbb(bbcenter, bbradius);
            bbradius.add(bbextend);
        }
        center = bbcenter;
        radius = bbradius;
    }

    float collisionbox(vec &center, vec &radius)
    {
        if(collideradius.x < 0)
        {
            boundbox(collidecenter, collideradius);
            if(collidexyradius)
            {
                collidecenter.x = collidecenter.y = 0;
                collideradius.x = collideradius.y = collidexyradius;
            }
            if(collideheight)
            {
                collidecenter.z = collideradius.z = collideheight/2;
            }
            rejectradius = collideradius.magnitude();
        }
        center = collidecenter;
        radius = collideradius;
        return rejectradius;
    }

    float boundsphere(vec &center)
    {
        vec radius;
        boundbox(center, radius);
        return radius.magnitude();
    }

    float above()
    {
        vec center, radius;
        boundbox(center, radius);
        return center.z+radius.z;
    }
};

