#pragma once

#include "CoreMinimal.h"

enum class ECellType
{
	Empty,
	Mine,
};

enum class ECellState
{
	Hidden,
	Revealed,
	Exploded,
};

enum class EMinesweeperGameState
{
	Running,
	GameOver_Win,
	GameOver_Lose,
};

enum class EInputType
{
	Visit,
	Flag,
};

struct FPlayerInput
{
	FIntPoint  Pos;
	EInputType Type;
};

struct FMineCell
{
	int32      NeighborMineCount;
	ECellType  CellType;
	ECellState CellState;

	FORCEINLINE bool IsMine() const
	{
		return CellType == ECellType::Mine;
	}

	FORCEINLINE bool IsRevealed() const
	{
		return CellState == ECellState::Revealed;
	}
};

struct FMinesweeperGameConfig
{
	static constexpr int32 MIN_ROW = 9;
	static constexpr int32 MIN_COL = 9;
	static constexpr int32 MAX_ROW = 16;
	static constexpr int32 MAX_COL = 30;
	static constexpr int32 DEFAULT_ROW = MIN_ROW;
	static constexpr int32 DEFAULT_COL = MIN_COL;
	static constexpr int32 MIN_MINE_COUNT = 10;
	static constexpr int32 DEFAULT_MAX_MINE_COUNT = DEFAULT_ROW * DEFAULT_COL - 1;
	static constexpr int32 DEFAULT_MINE_COUNT = MIN_MINE_COUNT;

	FIntPoint        GridSize;
	int32            MineCount;
	TOptional<int32> RandomSeed;

	static FMinesweeperGameConfig MakeDefaultConfig()
	{
		return FMinesweeperGameConfig{{DEFAULT_ROW, DEFAULT_COL}, DEFAULT_MINE_COUNT, TOptional<int32>{}};
	}

	FORCEINLINE bool IsValid() const
	{
		const bool bIsValidCellSize = GridSize.X >= MIN_COL && GridSize.X <= MAX_COL
				&& GridSize.Y >= MIN_ROW && GridSize.Y <= MAX_ROW;
		const bool bIsValidMineCount = MineCount < (GridSize.X * GridSize.Y);
		return bIsValidCellSize && bIsValidMineCount;
	}
};

struct FMinesweeperGameState
{
	TArray<FMineCell>     Cells;
	EMinesweeperGameState State;
};