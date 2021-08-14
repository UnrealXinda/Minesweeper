#pragma once

#include "CoreMinimal.h"
#include "MinesweeperGame.h"

DECLARE_DELEGATE_OneParam(FOnGameConfigUpdated, FMinesweeperGameConfig)
DECLARE_DELEGATE_TwoParams(FOnMineGridChanged, FMinesweeperGameConfig, const FMinesweeperGameState&)

/**
 * Model of minesweeper editor window in MVC pattern.
 * This is the minimum amount of data contained for the editor state.
 * Also defined two delegates that notifies the subscribers when either
 * game config is updated or mine grid needs redrawing.
 */
struct FMinesweeperModel
{
	FOnGameConfigUpdated OnGameConfigUpdated;
	FOnMineGridChanged   OnMineGridChanged;

	FMinesweeperGameConfig GameConfig;
	FMinesweeperGameState  GameState;

	FMinesweeperModel() :
		GameConfig{FMinesweeperGameConfig::MakeDefaultConfig()},
		GameState{}
	{
	}
};