#include "ExplosionEffect.h"
#include "DxLib.h"


namespace
{
	const float PARTICLE_LIFE = 1.0f; //パーティクルの寿命(秒)
	const float PARTICLE_RADIUS = 1.5f; //パーティクルの半径
	const float PARTICLE_SPEED[3] = { 50.0f, 80.0f, 120.0f }; //パーティクルの速度
	const float PARTICLE_DECAY = 0.95f; //パーティクルの減衰率
}


ExplosionEffect::ExplosionEffect(const Vector2D& pos, int particleCount)
	:Base(pos, { 0.0f, 0.0f }, GetColor(255, 255, 255))
{
	particles_.clear();
	isFinished_ = false;

	//パーティクル1個の初期化
	for (int i = 0; i < particleCount; i++)
	{
		Particle particle;
		particle.Offset = { 0.0f, 0.0f };
		particle.vel = { 0.0f, 0.0f };
		particle.life = PARTICLE_LIFE;//寿命3秒
		particle.radius = PARTICLE_RADIUS;//半径5.0f

		float angle_rad = (float)GetRand(360) * (Math2D::PI / 180.0f);
		Vector2D direction = Math2D::FromAngle(angle_rad);
		particle.vel = Math2D::Mul(direction, PARTICLE_SPEED[GetRand(2)]);

		//リストに追加
		//particles_[0] = particle;
		particles_.push_back(particle);
	}
}





void ExplosionEffect::Update()
{
	float dt = GetDeltaTime();
	bool allDead = true;

	for (auto& particle : particles_)
	{
		//寿命が残ってたら更新
		if (particle.life > 0.0f)
		{
			allDead = false;//まだ生きてるパーティクルがある

			//位置更新
			particle.Offset = Math2D::Add(particle.Offset, Math2D::Mul(particle.vel, dt));

			//速度減衰
			particle.vel = Math2D::Mul(particle.vel, PARTICLE_DECAY);

			//寿命減少
			particle.life -= dt;
			if (particle.life < 0.0f)
				particle.life = 0.0f;
		}
	}

	//全パーティクルが寿命切れならエフェクト終了
	if (allDead) //allDead == true と同じ
	{
		isFinished_ = true;
	}

}

void ExplosionEffect::Draw()
{
	if (isFinished_) return;//終了してたら描画しない

	for(auto& particle : particles_)
	{
		if (particle.life > 0.0f)
		{
			//パーティクルの描画
			Vector2D drawPos = Math2D::Add(GetPos(), particle.Offset);
			Vector2D screenPos = Math2D::World2Screen(drawPos);
			DrawCircle((int)screenPos.x,
						(int)screenPos.y,
						particle.radius,
						GetColor(255,255, 255));
		}
	}

}
//エフェクトがじんわり消えるところを実装
//あるlifeからだんだん色を薄くして、さいご黒になればいいよ。