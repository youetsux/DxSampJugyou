#include "Base.h"
#include "Stage.h"
#include "Math2D.h"
#include "Player.h"
#include "globals.h"
#include <DxLib.h>
#include "Bullet.h"
#include "Input.h"
#include <vector>
#include "Enemy.h"
#include "ExplosionEffect.h"


namespace
{
	//Player Parameters :　プレイヤーの初期値
	const Vector2D START_POS = {WIN_WIDTH/2, WIN_HEIGHT/2 };
	const Vector2D START_VEL = { 0.0f, 0.0f };
	const Vector2D START_DIR = { 0.0f, -1.0f };
	const float START_RADIUS = 30.0f;
	const float START_OMEGA = 2.0f;
	const unsigned int START_COLOR = GetColor(255, 0, 0);
	const unsigned int ENEMY_MAX = 100; //敵の最大数
	const unsigned int ENEMY_NUM = 10; //最初に出現する敵の数
	Player* player = nullptr;
	std::vector<Bullet*> bullets; //弾丸の保管庫
	std::vector<Enemy*> enemies; //敵の保管庫
	//std::vector<ExplosionEffect*> effects; //エフェクトの保管庫

	std::vector<Base*> objects; //全てのオブジェクトの保管庫
	//オブジェクトの保管庫にオブジェクトを追加する関数
	void AddObject(Base* obj)
	{
		objects.push_back(obj);
	}

	void UpdateAllObjects()
	{
		for (auto& obj : objects)
		{
			obj->Update();
		}
	}
	void DrawAllObjects()
	{
		for (auto& obj : objects)
		{
			obj->Draw();
		}
	}
}


Stage::Stage()
{
}

Stage::~Stage()
{
}

void Stage::Initialize()
{
	player = new Player(START_POS, START_VEL, START_COLOR,
		                START_DIR, START_RADIUS, START_OMEGA);

	AddObject(player);

	enemies.clear();
	enemies.reserve(ENEMY_NUM);

	//最初の敵を生成
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		Enemy* e = new Enemy(Enemy::Size::LARGE, 8);
		enemies.push_back(e);
		AddObject(e);
	}
}

void Stage::Update()
{

	//敵の位置と、当たり判定の半径
	//弾の位置
	//isAlive_ -> falseにする手段
	std::vector<Enemy*> aliveEnemies;
	std::vector<Bullet*> aliveBullets;

	aliveEnemies.clear();
	aliveBullets.clear();

	for (auto& itr : objects)
	{
		if (itr->GetType() == ENEMY)
		{
			Enemy* e = (Enemy *)(itr);
			if (e->IsAlive())
				aliveEnemies.push_back(e);
		}
		else if (itr->GetType() == BULLET)
		{
			Bullet* b = (Bullet *)(itr);
			if (!b->IsDead())
				aliveBullets.push_back(b);
		}
	}

	//for (auto& itr : bullets)
	//{
	//	for (int i = 0;i < enemies.size();i++)
	//	{
	//		if (!enemies[i]->IsAlive())
	//			continue; //敵が死んでたらスルー
	//		//itr->GetPos(); //弾の位置
	//		//enemies[i]->GetPos(); //敵の位置
	//		//enemies[i]->GetCollisionRadius(); //敵の当たり判定の半径
	//		float dist = Math2D::Length(Math2D::Sub(itr->GetPos(), 
	//			                        enemies[i]->GetPos()));
	//		if (dist < enemies[i]->GetCollisionRadius())
	//		{
	//			//当たった
	//			enemies[i]->Dead();//敵を消す(生存フラグをfalseに）
	//			//TODO:
	//			//分裂の処理をここでやりたい
	//			//大か中か小かを判定して
	//			//大なら中を2~4つ、中なら小を2~4つ、小なら何もしない(消すだけ）
	//			Vector2D enemyPos = enemies[i]->GetPos();
	//			Enemy::Size enemySize = enemies[i]->GetSize();
	//			if (enemySize == Enemy::Size::SMALL)
	//			{
	//				ExplosionEffect* effect = new ExplosionEffect(enemyPos);
	//				//effects.push_back(effect);
	//				AddObject(effect);

	//			}
	//			else if (enemySize == Enemy::Size::MEDIUM)
	//			{
	//				for(int i=0;i<4;i++)
	//				{
	//					Enemy* e = new Enemy(Enemy::Size::SMALL, 8);
	//					e->SetPos(enemyPos);
	//					//速さの設定は必要
	//					//x,yともに-100から100の間のランダムな速度
	//					e->SetVel({ (float)(GetRand(200) - 100), (float)(GetRand(200) - 100) });
	//					//e->SetVel(Vector2D((float)(GetRand(200) - 100), (float)(GetRand(200) - 100)));
	//					enemies.push_back(e);
	//					AddObject(e);
	//				}
	//			}
	//			else if (enemySize == Enemy::Size::LARGE)
	//			{
	//				for(int i=0;i<4;i++)
	//				{
	//					Enemy* e = new Enemy(Enemy::Size::MEDIUM, 8);
	//					e->SetPos(enemyPos);
	//					//速さの設定は必要
	//					e->SetVel({ (float)(GetRand(200) - 100), (float)(GetRand(200) - 100) });
	//					enemies.push_back(e);
	//					AddObject(e);
	//				}
	//			}

	//			itr->Dead();//弾も消す
	//		}
	//	}
	//}

	//賞味期限切れの弾を消す
	DeleteBullet();

	UpdateAllObjects();

	//Zキーが押されたら弾丸を生成
	if (Input::IsKeyDown(KEY_INPUT_Z))
	{
		ShootBullet();
	}
}

void Stage::Draw()
{
	DrawAllObjects();
}

void Stage::Release()
{
	if (player != nullptr)
		delete player;
	for (int i = 0;i < enemies.size();i++)
	{
		if (enemies[i] != nullptr)
			delete enemies[i];
	}
}

void Stage::DeleteBullet()
{
	//賞味期限切れの弾を消す
	for (auto it = bullets.begin(); it != bullets.end(); )
	{
		if ((*it)->IsDead() == true)
		{
			it = bullets.erase(it);
		}
		else
		{
			it++;
		}

	}
}

void Stage::ShootBullet()
{
	Vector2D pos = player->GetPos();
	Vector2D v = Math2D::Mul(player->GetDirVec(), 300.0f);
	unsigned int bcol = GetColor(255, 255, 255);
	float r = 2;
	float life = 2.0f;

	Bullet* b = new Bullet(pos, v, bcol, r, life);
	bullets.push_back(b);
	AddObject(b);
}
