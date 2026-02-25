#pragma once
#include <vector>



// ‘O•ûéŒ¾
class Player;
class Enemy;

class Stage
{
public:
	Stage();
	~Stage();
	void Initialize();
	void Update();
	void Draw();
	void Release();
private:
	void Enemy_vs_Bullet();
	void Enemy_vs_Player();
	//Player* player_;
	//Enemy* enemy_;
	long long gameScore_;
	void DeleteBullet();
	void DeleteEnemy();
	void DeleteEffect();
	void ShootBullet();
};

