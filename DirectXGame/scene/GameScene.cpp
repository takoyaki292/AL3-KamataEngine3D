#include "GameScene.h"
#include "TextureManager.h"
#include "Skydome.h"
#include "Matrix4x4Function.h"
#include "Player.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() 
{ 
	for (std::vector<WorldTransform*>& worldTansformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTansformBlock : worldTansformBlockLine)
		{
			delete worldTansformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete modelBlock_;
	delete modelSkydome_;
	delete mapChipField_;
	delete skydome_;
	delete debugCamera_;
	delete player_;
}

void GameScene::Initialize() {

	//int height = 720;
	//int width = 1280;

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	tetureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
//	model_ = Model::Create();
	model_ = Model::CreateFromOBJ("player", true);

	worldTransform_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	//座標をマップトップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1,-5);
	// 自キャラの初期化
	
	player_->Initialize(model_, &viewProjection_, playerPosition);

	const uint32_t kNumBlockVirtical = 20;
	const uint32_t kNumBlockHorizontal = 100;

	numBlockVirtical_ = 20;
	numBlockHorizontal_ = 100;

	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * i;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * j;
		}
	}
	


	modelBlock_ = Model::Create();
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	//スカイドームの初期化
	skydome_ = new Skydome();
	modelSkydome_ = Model::Create();
	skydome_->Initialize(model_, &viewProjection_);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	wolrldTransform_.Initialize();
	viewProjection_.Initialize();
	wolrldTransform_;
	mapChipData_ = {};

	debugCamera_ = new DebugCamera(1280,720);


	GenerateBlocks();

}

void GameScene::Update() 
{
	skydome_->Update();
	//自キャラの更新
	player_->Update();

	for (std::vector<WorldTransform*> worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
		}
	}

	for (std::vector<WorldTransform*> worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			worldTransformBlock->UpdateMatrix();
		}
	}

	#ifdef _DEBUG
	if (input_->TriggerKey(DIK_P))
	{
		isDebugCameraActiive_ = true;
	}
	#endif
	
	debugCamera_->Update();

	if (isDebugCameraActiive_)
	{
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		//ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} 
	else
	{
		//ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

	

}

void GameScene::Draw()
{

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	//自キャラの描画
	player_->Draw();
	//天球の描画
	//skydome_->Draw();

	//マップチップの描画
	for (std::vector<WorldTransform*> worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			//modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();



#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() 
{
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	

	worldTransformBlocks_.resize(numBlockVirtical);

	for (uint32_t i = 0; i < numBlockVirtical; ++i)
	{
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t x = 0; numBlockVirtical > x;x++)
	{
		for (uint32_t y = 0; numBlockHorizontal > y;y++)
		{
			if (mapChipField_->GetMapChipTypeByIndex(y, x) == MapChipType::kBlock) 
			{
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				
				worldTransformBlocks_[x][y] = worldTransform;
				worldTransformBlocks_[x][y]->translation_ = mapChipField_->GetMapChipPositionByIndex(y, x);
			}
		}
	}

}

