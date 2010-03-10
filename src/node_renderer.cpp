#include <string>

#include "node_renderer.h"
#include "macros.h"
#include "oroview.h"

using namespace std;

NodeRenderer::NodeRenderer(int tagid, string label) : tagid(tagid), label(label)
{
    elapsed = 0.0;
    fadetime = 1.0;

    size = 8.0;

    col = vec4f(1.0, 1.0, 1.0, 1.0);
    base_col = vec4f(1.0, 1.0, 1.0, 1.0);

    hovered = false;
    selected = false;

#ifndef TEXT_ONLY
    icon = texturemanager.grab("file.png");
#endif
}

void NodeRenderer::setColour(vec4f col) {
    base_col = col;
}

void NodeRenderer::setRenderingColour() {
    if (selected) col = SELECTED_COLOUR;
    else {
	if (hovered) col = HOVERED_COLOUR;
	else col = base_col;
    }
}

void NodeRenderer::draw(const vec2f& pos, rendering_mode mode, OroView& env) {

    float alpha = 1.0;
    vec2f offsetpos;
    float ratio, halfsize;

    switch (mode) {

    /****** NORMAL RENDERING ******/
    case NORMAL:
        setRenderingColour();

        glLoadName(tagid);

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);

        ratio = icon->h / (float) icon->w;
        halfsize = size * 0.5f;
        offsetpos = pos - vec2f(halfsize, halfsize);


        alpha = getAlpha();

        glBindTexture(GL_TEXTURE_2D, getIcon()->textureid);

        glPushMatrix();
            glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

            glColor4fv(col);

            glBegin(GL_QUADS);
                glTexCoord2f(0.0f,0.0f);
                glVertex2f(0.0f, 0.0f);

                glTexCoord2f(1.0f,0.0f);
                glVertex2f(size, 0.0f);

                glTexCoord2f(1.0f,1.0f);
                glVertex2f(size, size*ratio);

                glTexCoord2f(0.0f,1.0f);
                glVertex2f(0.0f, size*ratio);
            glEnd();

        glPopMatrix();

        glLoadName(0);

        break;

    /****** NAMES RENDERING ******/
    case NAMES:
        //compute that to get a nice fading effect
        float since_last_node_change = 2.5;

        alpha = std::max(0.0f, 5.0f - since_last_node_change) / 5.0f;

        glPushMatrix();
        glTranslatef(pos.x, pos.y, 0.0);

        glColor4f(1.0, 1.0, 1.0, alpha);

        vec3f screenpos = display.project(vec3f(0.0, 0.0, 0.0));

        glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
                glOrtho(0, display.width, display.height, 0, -1.0, 1.0);

            glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();

                env.font.draw(screenpos.x, screenpos.y, label);

            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

        glPopMatrix();

        break;

    }


}

//void NodeRenderer::drawBloom(Frustum& frustum, float dt) {
//
//    if(isVisible() && frustum.boundsInFrustum(quadItemBounds)) {
//
//	float bloom_radius = 10.0;
//
//	vec4f bloom_col = col;
//
//	glColor4f(bloom_col.x, bloom_col.y, bloom_col.z, 1.0);
//
//	glPushMatrix();
//	    glTranslatef(pos.x, pos.y, 0.0);
//
//	    glBegin(GL_QUADS);
//	    glTexCoord2f(1.0, 1.0);
//		glVertex2f(bloom_radius,bloom_radius);
//		glTexCoord2f(1.0, 0.0);
//		glVertex2f(bloom_radius,-bloom_radius);
//		glTexCoord2f(0.0, 0.0);
//		glVertex2f(-bloom_radius,-bloom_radius);
//		glTexCoord2f(0.0, 1.0);
//		glVertex2f(-bloom_radius,bloom_radius);
//	    glEnd();
//	glPopMatrix();
//
//    }
//}


void NodeRenderer::setMouseOver(bool over) {
    hovered = over;

}

void NodeRenderer::setSelected(bool select) {
    selected = select;
}
