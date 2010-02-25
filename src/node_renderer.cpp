#include "node_renderer.h"

NodeRenderer::NodeRenderer(int tagid) : tagid(tagid)
{
    elapsed = 0.0;
    fadetime = 1.0;

    size = 8.0;

    icon = texturemanager.grab("file.png");
}

void NodeRenderer::renderAt(const vec2f& pos) {

    glLoadName(tagid);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    float ratio = icon->h / (float) icon->w;
    float halfsize = size * 0.5f;
    vec2f offsetpos = pos - vec2f(halfsize, halfsize);


    float alpha = getAlpha();

    vec3f col = getColour();

    glBindTexture(GL_TEXTURE_2D, getIcon()->textureid);

    glPushMatrix();
	glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

	glColor4f(col.x, col.y, col.z, 1.0);

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
}
