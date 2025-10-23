
#include "states/move_state.h"

#include "world_object.h"
#include "raymath.h"

void MoveState::OnEnter()
{
	waitTimer = 3.0f;
	//random direction [-1,1]
	direction = { 2.0f * rand() / RAND_MAX - 1.0f,2.0f * rand() / RAND_MAX - 1.0f };
	direction = Vector2Normalize(direction);

	printf("enter move state\n");
}

State* MoveState::OnUpdate(float dt)
{
	waitTimer -= dt;

	printf("updating move dir %f %f\n", direction.x, direction.y);

	//TODO: PLEASE GET THIS OUT OF HERE
	const float moveSpeed = 100.0f;
	const float boundsLimitX = 1280.0f;
	const float boundsLimitY = 720.0f;

	float nextX = owner->position.x + direction.x * dt * moveSpeed;
	float nextY = owner->position.y + direction.y * dt * moveSpeed;

	//check for bounds
	if (nextX < 0 || nextX > boundsLimitX || nextY < 0 || nextY > boundsLimitY)
	{
		printf("out of bounds\n");
		return on_move_action_completed;
	}

	owner->position.x = nextX;
	owner->position.y = nextY;

	if (waitTimer <= 0.0f)
	{
		return on_move_action_completed;
	}

	return nullptr;
}

void MoveState::OnExit()
{
	waitTimer = 3.0f;
	printf("exit move state\n");
}
