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
	std::string BrendenFBX = "..//Brenden FBX//";

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
		export_skinned_mesh	(string(ArcherAssets + "ArcherAttack.fbx").c_str(), string(ArcherAssets + "ArcherTest.bin").c_str());
		export_bindpose		(string(ArcherAssets + "test_archer_001.fbx").c_str(), string(ArcherAssets + "ArcherBindTest.bin").c_str());
		export_animation	(string(ArcherAssets + "Attack.fbx").c_str(), string(ArcherAssets + "ArcherAttack.bin").c_str());
		export_animation	(string(ArcherAssets + "Hurt.fbx").c_str(), string(ArcherAssets + "ArcherHurt.bin").c_str());
		export_animation	(string(ArcherAssets + "Death.fbx").c_str(), string(ArcherAssets + "ArcherDeath.bin").c_str());
		export_animation	(string(ArcherAssets + "Walk.fbx").c_str(), string(ArcherAssets + "ArcherWalk.bin").c_str());
		export_animation	(string(ArcherAssets + "Idle.fbx").c_str(), string(ArcherAssets + "ArcherIdle.bin").c_str());
	}

	/**************************************************** Knight *************************************************************************/
	if (false)
	{
		//export_skinned_mesh(string(KnightAssets + "paladin_idle.fbx").c_str(), string(KnightAssets + "Knight.bin").c_str());
		export_bindpose(string(KnightAssets + "paladin_Bind.fbx").c_str(), string(KnightAssets + "KnightBind.bin").c_str());
		export_animation(string(KnightAssets + "paladin_Idle.fbx").c_str(), string(KnightAssets + "KnightIdle.bin").c_str());
		export_animation(string(KnightAssets + "paladin_Walk.fbx").c_str(), string(KnightAssets + "KnightWalk.bin").c_str());
		export_animation(string(KnightAssets + "paladin_attack.fbx").c_str(), string(KnightAssets + "KnightAttack.bin").c_str());
		export_animation(string(KnightAssets + "paladin_Death.fbx").c_str(), string(KnightAssets + "KnightDeath.bin").c_str());
		//export_animation(string(KnightAssets + "paladin_BlockPose.fbx").c_str(), string(KnightAssets + "KnightBlock.bin").c_str());
		export_animation(string(KnightAssets + "Block.fbx").c_str(), string(KnightAssets + "Block.bin").c_str());
	}

	/**************************************************** Mage *************************************************************************/
	if (false)
	{
		export_skinned_mesh(string(MageAssets + "BattleMage" + ".fbx").c_str(), string(MageAssets + "Mage.bin").c_str());
		export_bindpose(string(MageAssets + "Idle" + ".fbx").c_str(), string(MageAssets + "MageBind" + ".bin").c_str());
		export_animation(string(MageAssets + "Idle" + ".fbx").c_str(), string(MageAssets + "MageIdle" + ".bin").c_str());
		export_animation(string(MageAssets + "Walk" + ".fbx").c_str(), string(MageAssets + "MageWalk" + ".bin").c_str());
		export_animation(string(MageAssets + "MageAttack" + ".fbx").c_str(), string(MageAssets + "MageAttack" + ".bin").c_str());
		export_animation(string(MageAssets + "MageDeath" + ".fbx").c_str(), string(MageAssets + "MageDeath" + ".bin").c_str());
		export_animation(string(MageAssets + "run" + ".fbx").c_str(), string(MageAssets + "MageRun" + ".bin").c_str());
		export_animation(string(MageAssets + "MageHurt.fbx").c_str(), string(MageAssets + "MageHurt.bin").c_str());
	}

	/**************************************************** Other mesh's *************************************************************************/
	if (false)
	{
		//export_simple_mesh(string(LevelAssets + "ParticleQuad.fbx").c_str(), string(LevelAssets + "ParticleQuad.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Arrow.fbx").c_str(), string(LevelAssets + "Arrow.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Potion.fbx").c_str(), string(LevelAssets + "Potion.bin").c_str());
		export_simple_mesh(string(LevelAssets + "Firewall.fbx").c_str(), string(LevelAssets + "Firewall.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "ExportThisToquad.fbx").c_str(), string(LevelAssets + "quad.bin").c_str());
		//export_simple_mesh(string(MageAssets + "Projectile.fbx").c_str(), string(MageAssets + "Projectile.bin").c_str());
		//export_simple_mesh(string(MageAssets + "Spawn_Point.fbx").c_str(), string(MageAssets + "Spawn_Point.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level1.fbx").c_str(), string(LevelAssets + "Level1.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level2.fbx").c_str(), string(LevelAssets + "Level2New.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level3.fbx").c_str(), string(LevelAssets + "Level3.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Level5.fbx").c_str(), string(LevelAssets + "Level5t.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Wall.fbx").c_str(), string(LevelAssets + "Wall.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Door.fbx").c_str(), string(LevelAssets + "Door.bin").c_str());
		export_simple_mesh(string(LevelAssets + "OpenDoor.fbx").c_str(), string(LevelAssets + "OpenDoor.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "SpikeTrap.fbx").c_str(), string(LevelAssets + "SpikeTrap.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Pillar.fbx").c_str(), string(LevelAssets + "Pillar.bin").c_str());
		//export_simple_mesh(string(LevelAssets + "Torch.fbx").c_str(), string(LevelAssets + "Torch.bin").c_str());
	}

	/**************************************************** Brendens mesh's *************************************************************************/
	if (true)
	{
		export_simple_mesh(string(BrendenFBX + string("barrel01.fbx")).c_str(), string(BrendenFBX + string("barrel01.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("barrel02.fbx")).c_str(), string(BrendenFBX + string("barrel02.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Bookcase.fbx")).c_str(), string(BrendenFBX + string("BookCase.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Cart.fbx")).c_str(), string(BrendenFBX + string("cart.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Chair.fbx")).c_str(), string(BrendenFBX + string("chair.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("chest.fbx")).c_str(), string(BrendenFBX + string("chest.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("flag.fbx")).c_str(), string(BrendenFBX + string("logs.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Rock01.fbx")).c_str(), string(BrendenFBX + string("Rock01.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Rock02.fbx")).c_str(), string(BrendenFBX + string("Rock02.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Rock03.fbx")).c_str(), string(BrendenFBX + string("Rock03.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Sack.fbx")).c_str(), string(BrendenFBX + string("Sack.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Statue.fbx")).c_str(), string(BrendenFBX + string("Statue.bin")).c_str());
		export_simple_mesh(string(BrendenFBX + string("Table.fbx")).c_str(), string(BrendenFBX + string("Table.bin")).c_str());
	}

	/**************************************************** Creating animations in textures *************************************************************************/
	if (true)
	{
		const char* pchFilepaths[] =
		{ "..//FireGolemAssets//FireGolemIdle.bin", "..//FireGolemAssets//FireGolemWalk.bin", "..//FireGolemAssets//FireGolemMelee.bin", 
			"..//FireGolemAssets//FireGolemIdle" , "FireGolem" };
		//LoadAnimations(pchFilepaths, 3);

		const char* pchFilepaths2[] = 
		{ "..//MageAssets//MageIdle.bin", "..//MageAssets//MageWalk.bin", "..//MageAssets//MageAttack.bin", "..//MageAssets//MageDeath.bin", "..//MageAssets//MageHurt.bin",
		  "..//MageAssets//MageHurt" , "Mage" };
		LoadAnimations(pchFilepaths2, 5);

		const char* pchFilepaths3[] = 
		{ "..//MageAssets//VikingIdle.bin", "..//MageAssets//VikingWalk.bin", "..//MageAssets//VikingAttack.bin", "..//MageAssets//VikingDeath.bin", 
			"..//MageAssets//VikingIdle" , "Viking" };
		//LoadAnimations(pchFilepaths3, 4);

		const char* pchFilepaths4[] =
		{ 
			"..//MageAssets//ArcherIdle.bin", "..//MageAssets//ArcherWalk.bin", "..//MageAssets//ArcherAttack.bin", "..//MageAssets//ArcherDeath.bin", "..//MageAssets//ArcherHurt.bin",
			"..//MageAssets//test_archer_001" , "Archer" };
		LoadAnimations(pchFilepaths4, 5);

		const char* pchFilepaths5[] =
		{ "..//KnightAssets//KnightIdle.bin", "..//KnightAssets//KnightWalk.bin", "..//KnightAssets//KnightAttack.bin", "..//KnightAssets//KnightDeath.bin", "..//KnightAssets//KnightBlock.bin",
			"..//KnightAssets//paladin_Bind" , "Knight" };
		LoadAnimations(pchFilepaths5, 5);
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