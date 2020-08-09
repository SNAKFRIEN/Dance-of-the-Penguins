#include "Spawner.h"

#include "EliMath.h"

glm::vec3 Spawner::FindOffScreenSpawnPoint(glm::vec3 cameraPos, glm::vec3 cameraLookAtPoint, float fov, float zOffset, float xOffset) const
{
    //Find the furthest point vertically that's visible on the screen
    
    //Create ray towards the top of the screen directly above where the camera is looking

    return EliMath::IntersectFloor(cameraPos, cameraLookAtPoint - cameraPos);
}
