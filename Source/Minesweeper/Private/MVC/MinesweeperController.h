#pragma once

#include "CoreMinimal.h"

/**
 * Controller of minesweeper editor window in MVC pattern. Designed for:
 * 1. Governing rule of the minesweeper game.
 * 2. Handling any user input propagated from view and then updating model accordingly.
 */
class FMinesweeperController
{
public:
	explicit FMinesweeperController(struct FMinesweeperModel* InModel);

	FMinesweeperController(const FMinesweeperController&) = delete;
	FMinesweeperController& operator=(const FMinesweeperController&) = delete;

	void HandleOnStartNewGame(struct FMinesweeperGameConfig NewConfig);
	void HandleOnPlayerInput(struct FPlayerInput Input);

private:
	void InitializeGame(FMinesweeperGameConfig NewConfig);

	/** Advance game based on player input. Returns true if mine grid needs redrawing */
	bool AdvanceGame(FPlayerInput Input);

	/** Visit a cell at given position. Returns true if mine grid needs redrawing */
	bool VisitCell(FIntPoint Pos);

	/** Flag a cell at given position. Returns true if mine grid needs redrawing */
	bool FlagCell(FIntPoint Pos);

	/** Flood fill algorithm to recursively reveal cells if possible. */
	/** Returns true if mine grid needs redrawing */
	bool FloodFill(FIntPoint Pos);

	/** Examines current game state */
	void UpdateGameState();

private:
	struct FMinesweeperModel* Model;
};