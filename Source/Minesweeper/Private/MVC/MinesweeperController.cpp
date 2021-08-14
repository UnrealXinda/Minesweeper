#include "MinesweeperController.h"
#include "MinesweeperModel.h"
#include "MinesweeperView.h"
#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
#include "Algo/Count.h"
#include "Algo/ForEach.h"

namespace
{
	const TArray<FIntPoint> NEIGHBOR_OFFSETS =
	{
		{-1, -1}, { 0, -1}, { 1, -1},
		{-1,  0},           { 1,  0},
		{-1,  1}, { 0,  1}, { 1,  1},
	};

	void RandomPopulateMines(TArray<FMineCell>& Cells, int32 MineCount, TOptional<int32> Seed = TOptional<int32>{})
	{
		check(MineCount > 0 && MineCount <= Cells.Num());

		FRandomStream Stream;
		Stream.Initialize(Seed ? *Seed : FMath::Rand());

		for (int32 Idx = 0; Idx < MineCount; ++Idx)
		{
			Cells[Idx].CellType = ECellType::Mine;
		}

		// Shuffling via placing random element at the top of the array
		for (int32 Idx = 0; Idx < Cells.Num(); ++Idx)
		{
			const int32 RandomIdx = Stream.RandRange(Idx, Cells.Num() - 1);
			Cells.Swap(Idx, RandomIdx);
		}
	}

	bool IsValidCellPosition(FIntPoint GridSize, FIntPoint Pos)
	{
		return Pos.X >= 0 && Pos.X < GridSize.X
			&& Pos.Y >= 0 && Pos.Y < GridSize.Y;
	}

	FMineCell& GetCell(TArray<FMineCell>& Cells, FIntPoint GridSize, FIntPoint Pos)
	{
		check(IsValidCellPosition(GridSize, Pos));
		const int32 Index = Pos.Y * GridSize.X + Pos.X;
		return Cells[Index];
	}
}

FMinesweeperController::FMinesweeperController(FMinesweeperModel* InModel) :
	Model{InModel}
{
	InitializeGame(FMinesweeperGameConfig::MakeDefaultConfig());
}

void FMinesweeperController::HandleOnStartNewGame(FMinesweeperGameConfig NewConfig)
{
	check(NewConfig.IsValid())
	InitializeGame(NewConfig);
	Model->OnGameConfigUpdated.ExecuteIfBound(NewConfig);
}

void FMinesweeperController::HandleOnPlayerInput(FPlayerInput Input)
{
	if (Model->GameState.State == EMinesweeperGameState::Running)
	{
		const bool bHasGridChanged = AdvanceGame(Input);
		if (bHasGridChanged)
		{
			Model->OnMineGridChanged.ExecuteIfBound(Model->GameConfig, Model->GameState);
		}
	}
}

void FMinesweeperController::InitializeGame(FMinesweeperGameConfig NewConfig)
{
	FMinesweeperGameConfig& GameConfig = Model->GameConfig;
	FMinesweeperGameState& GameState = Model->GameState;

	GameConfig = NewConfig;

	const FIntPoint GridSize = GameConfig.GridSize;
	const int32 ColCount = GridSize.X;
	const int32 RowCount = GridSize.Y;
	const int32 CellCount = RowCount * ColCount;

	GameState.Cells.Empty();
	GameState.Cells.AddZeroed(CellCount);
	GameState.State = EMinesweeperGameState::Running;

	RandomPopulateMines(GameState.Cells, GameConfig.MineCount);

	// Calculate neighbor mine count
	for (int32 Idx = 0; Idx < CellCount; ++Idx)
	{
		const int32 X = Idx % ColCount;
		const int32 Y = Idx / ColCount;
		FMineCell& Cell = GetCell(GameState.Cells, GridSize, {X, Y});

		Cell.NeighborMineCount = Algo::CountIf(NEIGHBOR_OFFSETS, [&](const FIntPoint Offset)
		{
			const FIntPoint Neighbor = FIntPoint{X, Y} + Offset;
			return IsValidCellPosition(GameConfig.GridSize, Neighbor) && GetCell(GameState.Cells, GridSize, Neighbor).IsMine();
		});
	}
}

bool FMinesweeperController::AdvanceGame(FPlayerInput Input)
{
	const FIntPoint GridSize = Model->GameConfig.GridSize;
	FMinesweeperGameState& GameState = Model->GameState;

	check(GameState.State == EMinesweeperGameState::Running);
	check(IsValidCellPosition(GridSize, Input.Pos));

	bool bGridHasChanged;

	switch (Input.Type)
	{
	case EInputType::Visit:
	default:
		bGridHasChanged = VisitCell(Input.Pos);
		break;
	case EInputType::Flag:
		bGridHasChanged = FlagCell(Input.Pos);
		break;
	}

	if (bGridHasChanged)
	{
		UpdateGameState();
	}

	return bGridHasChanged;
}

bool FMinesweeperController::VisitCell(FIntPoint Pos)
{
	const FMinesweeperGameConfig& GameConfig = Model->GameConfig;
	FMinesweeperGameState& GameState = Model->GameState;
	FMineCell& InputCell = GetCell(GameState.Cells, GameConfig.GridSize, Pos);

	// Clicked on mine, game over
	if (InputCell.IsMine())
	{
		InputCell.CellState = ECellState::Exploded;
		GameState.State = EMinesweeperGameState::GameOver_Lose;
		return true;
	}

	// Recursively visit neighbors that can be revealed
	const bool bGridHasChanged = FloodFill(Pos);

	return bGridHasChanged;
}

bool FMinesweeperController::FlagCell(FIntPoint Pos)
{
	UE_LOG(LogTemp, Warning, TEXT("Flagging not currently supported."));
	return false;
}

bool FMinesweeperController::FloodFill(FIntPoint Pos)
{
	const FMinesweeperGameConfig& GameConfig = Model->GameConfig;
	FMinesweeperGameState& GameState = Model->GameState;

	if (IsValidCellPosition(GameConfig.GridSize, Pos))
	{
		FMineCell& Cell = GetCell(GameState.Cells, GameConfig.GridSize, Pos);
		const bool bShouldReveal = !Cell.IsMine() && !Cell.IsRevealed();

		if (bShouldReveal)
		{
			Cell.CellState = ECellState::Revealed;

			// Recursively flood-filling cells whose neighbors have no mines
			if (Cell.NeighborMineCount == 0)
			{
				Algo::ForEach(NEIGHBOR_OFFSETS, [Pos, this](const FIntPoint Offset)
				{
					const FIntPoint NeighborPos = Pos + Offset;
					FloodFill(NeighborPos);
				});
			}
		}

		return bShouldReveal;
	}

	return false;
}

void FMinesweeperController::UpdateGameState()
{
	FMinesweeperGameState& GameState = Model->GameState;

	const bool bGameOverLose = Algo::AnyOf(GameState.Cells, [](const FMineCell& Cell)
	{
		return Cell.CellState == ECellState::Exploded;
	});

	if (bGameOverLose)
	{
		// Reveal all mines except the exploded mine
		Algo::ForEach(GameState.Cells, [](FMineCell& Cell)
		{
			if (Cell.IsMine() && Cell.CellState != ECellState::Exploded)
			{
				Cell.CellState = ECellState::Revealed;
			}
		});
		GameState.State = EMinesweeperGameState::GameOver_Lose;
		return;
	}

	const bool bGameOverWin = Algo::AllOf(GameState.Cells, [](const FMineCell& Cell)
	{
		return Cell.IsRevealed() ^ Cell.IsMine();
	});

	if (bGameOverWin)
	{
		// Reveal all cells
		Algo::ForEach(GameState.Cells, [](FMineCell& Cell)
		{
			Cell.CellState = ECellState::Revealed;
		});
		GameState.State = EMinesweeperGameState::GameOver_Win;
	}
}