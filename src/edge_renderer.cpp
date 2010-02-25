#include "edge_renderer.h"

EdgeRenderer::EdgeRenderer(int tagid) : tagid(tagid)
{

}

void EdgeRenderer::renderAt() {

    glLoadName(tagid);


    glLoadName(0);
}
