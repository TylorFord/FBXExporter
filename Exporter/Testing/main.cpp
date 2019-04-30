#include "InterFace.h"
#include <iostream>


#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
using namespace std;

int main()
{
	// in main
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(469);//put number from leak output in function call
#endif
	
#pragma region filepaths

	std::string MageAssets = "..//MageAssets//";
	std::string DemonAssets = "..//DemonAssets//";
	std::string KnightAssets = "..//KnightAssets//";
	std::string ArcherAssets = "..//ArcherAssets//";
	std::string FireGolemAssets = "..//FireGolemAssets//";
	std::string LevelAssets = "..//LevelAssets//";
	std::string Output = "..//Bin//";

#pragma endregion

	/**************************************************** Fire golem *************************************************************************/
	if (false)
	{
		export_skinned_mesh(string(FireGolemAssets + "FireGolem.fbx").c_str(), string(FireGolemAssets + "FireGolem.bin").c_str());
		//export_animation(string(FireGolemAssets + "FireGolemIdle" + ".fbx").c_str(), string(FireGolemAssets + "FireGolemIdle" + ".bin").c_str());
		//export_animation(string(FireGolemAssets + "FireGolemWalk" + ".fbx").c_str(), string(FireGolemAssets + "FireGolemWalk" + ".bin").c_str());
		//export_animation(string(FireGolemAssets + "FireGolemMelee" + ".fbx").c_str(), string(FireGolemAssets + "FireGolemMelee" + ".bin").c_str());
	}

	/**************************************************** Viking *************************************************************************/
	if (false)
	{
		export_skinned_mesh	(string(MageAssets + "Viking.fbx").c_str(), string(MageAssets + "Viking.bin").c_str());
		//export_bindpose		(string(MageAssets + "VikingIdle.fbx").c_str(), string(MageAssets + "VikingBind.bin").c_str());
		//export_animation	(string(MageAssets + "VikingIdle" + ".fbx").c_str(), string(MageAssets + "VikingIdle" + ".bin").c_str());
		//export_animation	(string(MageAssets + "VikingDeath" + ".fbx").c_str(), string(MageAssets + "VikingDeath" + ".bin").c_str());
		//export_animation	(string(MageAssets + "VikingAttack" + ".fbx").c_str(), string(MageAssets + "VikingAttack" + ".bin").c_str());
		//export_animation	(string(MageAssets + "VikingWalk" + ".fbx").c_str(), string(MageAssets + "VikingWalk" + ".bin").c_str());
	}

	/**************************************************** Archer *************************************************************************/
	if (false)
	{
		export_skinned_mesh	(string(MageAssets + "ArcherAttack.fbx").c_str(), string(MageAssets + "ArcherTest.bin").c_str());
		export_bindpose		(string(MageAssets + "test_archer_001.fbx").c_str(), string(MageAssets + "ArcherBindTest.bin").c_str());
		export_animation	(string(MageAssets + "Attack.fbx").c_str(), string(MageAssets + "ArcherAttack.bin").c_str());
		export_animation	(string(MageAssets + "Hurt.fbx").c_str(), string(MageAssets + "ArcherHurt.bin").c_str());
		export_animation	(string(MageAssets + "Death.fbx").c_str(), string(MageAssets + "ArcherDeath.bin").c_str());
		export_animation	(string(MageAssets + "Walk.fbx").c_str(), string(MageAssets + "ArcherWalk.bin").c_str());
		export_animation	(string(MageAssets + "Idle.fbx").c_str(), string(MageAssets + "ArcherIdle.bin").c_str());
	}

	/**************************************************** Knight *************************************************************************/
	if (false)
	{
		export_skinned_mesh(string(KnightAssets + "paladin_idle.fbx").c_str(), string(KnightAssets + "Knight.bin").c_str());
		export_bindpose(string(KnightAssets + "paladin_Bind.fbx").c_str(), string(KnightAssets + "KnightBind.bin").c_str());
		export_animation(string(KnightAssets + "paladin_Idle.fbx").c_str(), string(KnightAssets + "KnightIdle.bin").c_str());
		export_animation(string(KnightAssets + "paladin_Walk.fbx").c_str(), string(KnightAssets + "KnightWalk.bin").c_str());
		export_animation(string(KnightAssets + "paladin_attack.fbx").c_str(), string(KnightAssets + "KnightAttack.bin").c_str());
		export_animation(string(KnightAssets + "paladin_Death.fbx").c_str(), string(KnightAssets + "KnightDeath.bin").c_str());
		export_animation(string(KnightAssets + "paladin_BlockPose.fbx").c_str(), string(KnightAssets + "KnightBlock.bin").c_str());
	}

	/**************************************************** Mage *************************************************************************/
	if (false)
	{
		export_skinned_mesh(string(MageAssets + "BattleMage" + ".fbx").c_str(), string(MageAssets + "Mage.bin").c_str());
		//export_bindpose(string(MageAssets + "Idle" + ".fbx").c_str(), string(MageAssets + "MageBind" + ".bin").c_str());
		//export_animation(string(MageAssets + "Idle" + ".fbx").c_str(), string(MageAssets + "MageIdle" + ".bin").c_str());
		//export_animation(string(MageAssets + "Walk" + ".fbx").c_str(), string(MageAssets + "MageWalk" + ".bin").c_str());
		//export_animation(string(MageAssets + "MageAttack" + ".fbx").c_str(), string(MageAssets + "MageAttack" + ".bin").c_str());
		//export_animation(string(MageAssets + "MageDeath" + ".fbx").c_str(), string(MageAssets + "MageDeath" + ".bin").c_str());
		//export_animation(string(MageAssets + "run" + ".fbx").c_str(), string(MageAssets + "MageRun" + ".bin").c_str());
	}

	/**************************************************** Other mesh's *************************************************************************/
	if (false)
	{
		//export_simple_mesh(string(LevelAssets + "ParticleQuad.fbx").c_str(), string(LevelAssets + "ParticleQuad.bin").c_str());
		export_simple_mesh(string(LevelAssets + "Arrow.fbx").c_str(), string(LevelAssets + "Arrow.bin").c_str());
		export_simple_mesh(string(LevelAssets + "Potion.fbx").c_str(), string(LevelAssets + "Potion.bin").c_str());
		//export_simple_mesh(string(MageAssets + "Projectile.fbx").c_str(), string(MageAssets + "Projectile.bin").c_str());
		//export_simple_mesh(string(MageAssets + "Spawn_Point.fbx").c_str(), string(MageAssets + "Spawn_Point.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level1.fbx").c_str(), string(LevelAssets + "Level1.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level2.fbx").c_str(), string(LevelAssets + "Level2New.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level3.fbx").c_str(), string(LevelAssets + "Level3.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level5.fbx").c_str(), string(LevelAssets + "Level5t.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Wall.fbx").c_str(), string(LevelAssets + "Wall.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Door.fbx").c_str(), string(LevelAssets + "Door.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "SpikeTrap.fbx").c_str(), string(LevelAssets + "SpikeTrap.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Pillar.fbx").c_str(), string(LevelAssets + "Pillar.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Torch.fbx").c_str(), string(LevelAssets + "Torch.bin").c_str());
	}

	/**************************************************** Creating animations in textures *************************************************************************/
	if (false)
	{
		const char* pchFilepaths[] =
		{ "..//FireGolemAssets//FireGolemIdle.bin", "..//FireGolemAssets//FireGolemWalk.bin", "..//FireGolemAssets//FireGolemMelee.bin", "..//FireGolemAssets//FireGolemIdle" , "FireGolem" };
		//LoadAnimations(pchFilepaths, 3);

		const char* pchFilepaths2[] = 
		{ "..//MageAssets//MageIdle.bin", "..//MageAssets//MageWalk.bin", "..//MageAssets//MageAttack.bin", "..//MageAssets//MageDeath.bin", "..//MageAssets//Idle" , "Mage" };
		//LoadAnimations(pchFilepaths2, 4);

		const char* pchFilepaths3[] = 
		{ "..//MageAssets//VikingIdle.bin", "..//MageAssets//VikingWalk.bin", "..//MageAssets//VikingAttack.bin", "..//MageAssets//VikingDeath.bin", "..//MageAssets//VikingIdle" , "Viking" };
		//LoadAnimations(pchFilepaths3, 4);

		const char* pchFilepaths4[] =
		{ "..//MageAssets//ArcherIdle.bin", "..//MageAssets//ArcherWalk.bin", "..//MageAssets//ArcherAttack.bin", "..//MageAssets//ArcherDeath.bin", "..//MageAssets//test_archer_001" , "Archer" };
		//LoadAnimations(pchFilepaths4, 4);

		const char* pchFilepaths5[] =
		{ "..//KnightAssets//KnightIdle.bin", "..//KnightAssets//KnightWalk.bin", "..//KnightAssets//KnightAttack.bin", "..//KnightAssets//KnightDeath.bin", "..//KnightAssets//paladin_Bind" , "Knight" };
		LoadAnimations(pchFilepaths5, 4);
	}

	/**************************************************** Inversing Meshes *************************************************************************/
	if (false)
	{
		export_inversed_animation	(string(MageAssets + "idle.fbx").c_str(), string(MageAssets + "MageIdleLH.bin").c_str());
		export_inversed_bindpose	(string(MageAssets + "idle.fbx").c_str(), string(MageAssets + "MageBindLH.bin").c_str());
		export_inversed_skinned_mesh(string(MageAssets + "BattleMage.fbx").c_str(), string(MageAssets + "MageMeshLH.bin").c_str());
	}

	/**************************************************** Demon *************************************************************************/
	if (false)
	{
		export_skinned_mesh(string(DemonAssets + "DemonIdle.fbx").c_str(), string(DemonAssets + "Demon.bin").c_str());
		//export_bindpose(string (DemonAssets + "Test.fbx").c_str(), string(DemonAssets + "DemonBind.bin").c_str());
		export_animation(string(DemonAssets + "DemonFireBall.fbx").c_str(), string(DemonAssets + "DemonFireBall.bin").c_str());
		export_animation(string(DemonAssets + "DemonIdle.fbx").c_str(), string(DemonAssets + "DemonIdle.bin").c_str());
		export_animation(string(DemonAssets + "DemonMelee.fbx").c_str(), string(DemonAssets + "DemonMelee.bin").c_str());
		export_animation(string(DemonAssets + "DemonFirewall.fbx").c_str(), string(DemonAssets + "DemonFirewall.bin").c_str());
	}

	system("pause");
	return 0;
}