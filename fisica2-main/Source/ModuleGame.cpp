#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{

	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

protected:
	PhysBody* body;
	Module* listener;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, ColliderType _type)
		: PhysicEntity(physics->CreateCircle(_x, _y, 25), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

private:
	Texture2D texture;

};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 50), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};
class Flipper : public PhysicEntity
{
public:
	Flipper(ModulePhysics* physics, int _x, int _y, bool isLeft, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, _texture.width, _texture.height), _listener)
		, texture(_texture)
		, isLeft(isLeft)
	{
		b2Body* ground = physics->ground;

		b2RevoluteJointDef jointDef;
		jointDef.bodyA = ground;
		jointDef.bodyB = body->body;
		jointDef.localAnchorA.Set(_x, _y);
		jointDef.localAnchorB.Set(0, 0);
		jointDef.enableMotor = true;
		jointDef.enableLimit = true;

		jointDef.lowerAngle = -0.25f * b2_pi;
		jointDef.upperAngle = 0.0f;

		jointDef.maxMotorTorque = 2000.0f;
		jointDef.motorSpeed = isLeft ? -5.0f : 5.0f;

		flipperJoint = (b2RevoluteJoint*)physics->world->CreateJoint(&jointDef);


	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	void ControlFlipper(bool pressed)
	{
		if (pressed)
		{
			flipperJoint->SetMotorSpeed(isLeft ? 10.0f : -10.0f);
		}
		else
		{
			flipperJoint->SetMotorSpeed(isLeft ? -5.0f : 5.0f);
		}
	}

private:
	b2RevoluteJoint* flipperJoint;
	Texture2D texture;
	bool isLeft;
};
class Rick : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int table[110] = {
	489, 862,
	558, 818,
	558, 654,
	537, 633,
	536, 583,
	552, 567,
	569, 546,
	577, 524,
	580, 500,
	580, 129,
	575, 117,
	565, 106,
	555, 100,
	540, 104,
	489, 131,
	482, 126,
	484, 115,
	526, 73,
	535, 66,
	556, 65,
	575, 71,
	593, 86,
	605, 112,
	608, 126,
	607, 647,
	646, 648,
	647, 140,
	644, 98,
	639, 71,
	628, 48,
	611, 33,
	595, 20,
	580, 14,
	160, 15,
	137, 19,
	106, 39,
	82, 69,
	66, 108,
	56, 157,
	49, 207,
	48, 246,
	32, 269,
	26, 277,
	26, 437,
	36, 477,
	72, 516,
	72, 607,
	49, 641,
	50, 820,
	121, 863,
	0, 863,
	0, 0,
	647, 0,
	647, 863,
	490, 863
	};

	Rick(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(GetMouseX(), GetMouseY(), table, 110), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};



ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;

}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//Load textures
	circle = LoadTexture("Assets/wheel.png");
	box = LoadTexture("Assets/crate.png");
	rick = LoadTexture("Assets/MapComponents/Whole Map.png");
	flipperL = LoadTexture("Assets/MapComponents/flipper.png");
	flipperR = LoadTexture("Assets/MapComponents/flipper.png");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	entities.emplace_back(new Rick(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT, this, rick));

	Texture2D flipperTexture = LoadTexture("Assets/MapComponents/Flipper.png");
	/*entities.emplace_back(new Flipper(App->physics, PIXEL_TO_METERS(210), PIXEL_TO_METERS(765), true, this, flipperTexture));
	entities.emplace_back(new Flipper(App->physics, PIXEL_TO_METERS(315), PIXEL_TO_METERS(765), false, this, flipperTexture));*/

	// Load music
	background_music = App->audio->PlayMusic("Assets/music.ogg");

	// Load FX
	bonus_fx = App->audio->LoadFx("Assets/bonus.wav");
	flipper_fx = App->audio->LoadFx("Assets/flipper.wav");
	game_over_fx = App->audio->LoadFx("Assets/game_over.wav");
	start_fx = App->audio->LoadFx("Assets/start.wav");
	launch_fx = App->audio->LoadFx("Assets/launch.wav");


	//Load mierdas para conseguir puntos

	//Load pelotas bien

	//Load paredes

	//Crear death_trigger
	death_trigger = App->physics->CreateRectangle(0, 0, 10, 10);
	death_trigger->type = ColliderType::DEATH;

	//Crear launcher


	return ret;


}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{
	if (game_state == GameState::RESTART)
	{
		Restart();
	}
	ManageInputs();

	if (IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
	}

	// Prepare for raycast ------------------------------------------------------

	vec2i mouse;
	mouse.x = GetMouseX();
	mouse.y = GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	vec2f normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------


	for (PhysicEntity* entity : entities)
	{
		entity->Update();
		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}


	// ray -----------------
	if (ray_on == true)
	{
		vec2f destination((float)(mouse.x - ray.x), (float)(mouse.y - ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}
	/*for (PhysicEntity* entity : entities)
	{
		Flipper* flipper = dynamic_cast<Flipper*>(entity);
		if (flipper)
		{
			flipper->ControlFlipper(IsKeyDown(KEY_SPACE));
		}

		entity->Update();
	}*/
	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

	App->audio->PlayFx(bonus_fx);

	/*
	int x, y;
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/

	if (bodyA->type == ColliderType::BALL && bodyB->type == ColliderType::DEATH)
	{
		LoseLife();
	}

}

void ModuleGame::LoseLife()
{
	vidas--;
	if (vidas == 0)
	{
		GameOver();
	}
	else
	{
		game_state = GameState::RESTART;
	}
}

void ModuleGame::ManageInputs()
{
	//if game_state == start_menu, esperar a que se pulse el boton de start y cambiar el game state a playing
	if (game_state == GameState::START_MENU)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			game_state = GameState::PLAYING;
		}
	}

	//if game_state == playing, mover los flippers y el launcher

	if (game_state == GameState::PLAYING)
	{
		if (IsKeyDown(KEY_SPACE))
		{
			Flipper* flipper = dynamic_cast<Flipper*>(entities[1]);
			if (flipper)
			{
				flipper->ControlFlipper(IsKeyDown(KEY_SPACE));
			}
		}
		if (IsKeyDown(KEY_A)) {
			App->physics->flipperL->body->ApplyForceToCenter(b2Vec2(0, -50), true);

		}
		if (IsKeyDown(KEY_D)) {
			App->physics->flipperR->body->ApplyForceToCenter(b2Vec2(0, -50), true);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			entities.emplace_back(new Circle(App->physics, GetMouseX(), GetMouseY(), this, circle, ColliderType::BALL));

		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			entities.emplace_back(new Box(App->physics, GetMouseX(), GetMouseY(), this, box));
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
		{
			entities.emplace_back(new Rick(App->physics, GetMouseX(), GetMouseY(), this, rick));
		}

		if (IsKeyDown(KEY_DOWN)) {

			//lanzar bola

		}
	}

	//if game_state == game_over, esperar a que se pulse el boton de start y cambiar el game state a playing o que pulse al boton de exit y cambiar el game state a start_menu

	if (game_state == GameState::GAME_OVER)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			game_state = GameState::PLAYING;
		}
		if (IsKeyPressed(KEY_ESCAPE))
		{
			game_state = GameState::START_MENU;
		}
	}

}

void ModuleGame::GameOver()
{
	App->audio->PlayFx(game_over_fx);
	game_state = GameState::GAME_OVER;
}

void ModuleGame::Restart()
{
	//resetear el score, la vida, los objetos que den puntos y por ultimo cambiar el game state a jugar
}

