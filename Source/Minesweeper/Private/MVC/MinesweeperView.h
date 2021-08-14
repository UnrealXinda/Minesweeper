#pragma once

#include "CoreMinimal.h"
#include "MinesweeperGame.h"
#include "UI/MineCellWidget.h"
#include "Widgets/Input/SSpinBox.h"

DECLARE_DELEGATE_OneParam(FOnStartNewGame, FMinesweeperGameConfig);
DECLARE_DELEGATE_OneParam(FOnPlayerInput, FPlayerInput);

/**
 * View of minesweeper editor window in MVC pattern. Designed for:
 * 1. Propagates event when starting a new game and upon player input on mine cell.
 * 2. Render the whole mine grid given game config and game state.
 */
class FMinesweeperView
{
public:
	FMinesweeperView();

	FMinesweeperView(const FMinesweeperView&) = delete;
	FMinesweeperView& operator=(const FMinesweeperView&) = delete;

	TSharedRef<SDockTab> CreateMinesweeperView(const FSpawnTabArgs& SpawnTabArgs, FMinesweeperGameConfig GameConfig);
	void RebuildGameLayout(FMinesweeperGameConfig NewConfig);
	void UpdateGameLayout(FMinesweeperGameConfig GameConfig, const FMinesweeperGameState& GameState);

public:
	FOnStartNewGame OnStartNewGame;
	FOnPlayerInput  OnPlayerInput;

private:
	TSharedPtr<SWidget> CreateInputWidget();
	void BroadcastOnStartNewGame();
	void ValidateMineCountInput();
	void RebuildMineGridWidget(FMinesweeperGameConfig GameConfig);
	void UpdateMineGridWidget(FMinesweeperGameConfig GameConfig, const FMinesweeperGameState& GameState);
	void UpdateGameStateWidget(EMinesweeperGameState State);

private:
	TArray<FMineCellWidget> MineCellWidgets;

	TSharedPtr<class SButton>   NewGameButton;
	TSharedPtr<SSpinBox<int32>> WidthSpinBox;
	TSharedPtr<SSpinBox<int32>> HeightSpinBox;
	TSharedPtr<SSpinBox<int32>> MineCountSpinBox;

	TSharedPtr<class SUniformGridPanel> MineGridWidget;
	TSharedPtr<class STextBlock>        GameStateWidget;
};