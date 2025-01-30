#include "Actor.h"

class PlayerMesh : public Actor
{
public:
	PlayerMesh(Game* game);
	void OnUpdate(float deltaTime) override;

private:
	class MeshComponent* mPlayerMesh;
};