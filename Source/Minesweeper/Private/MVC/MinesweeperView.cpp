// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperView.h"
#include "MinesweeperGame.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#define LOCTEXT_NAMESPACE "FMinesweeperView"

namespace
{
	FLinearColor GetMineCellColor(ECellState CellState, ECellType CellType)
	{
		static const FLinearColor MineCellColor = FLinearColor::Black;
		static const FLinearColor ExplodedMineColor = FLinearColor::Red;
		static const FLinearColor RevealedCellColor = FLinearColor::Gray;
		static const FLinearColor HiddenCellColor = FLinearColor::White;

		FLinearColor Color;

		switch (CellState)
		{
		case ECellState::Revealed:
			switch (CellType)
			{
				case ECellType::Empty:
					Color = RevealedCellColor;
					break;
				case ECellType::Mine:
					Color = MineCellColor;
					break;
			}
			break;
		case ECellState::Exploded:
			Color = ExplodedMineColor;
			break;
		case ECellState::Hidden:
		default:
			Color = HiddenCellColor;
			break;
		}
		return Color;
	}

	FLinearColor GetMineCellTextColor(const FMineCell& MineCell)
	{
		static const FLinearColor TextColor[] =
		{
			FLinearColor::Transparent,
			FLinearColor::Blue,
			FLinearColor::Green,
			FLinearColor::Red,
			FLinearColor{0.0F, 0.0F, 0.5F}, // Purple
			FLinearColor{0.5F, 0.0F, 0.0F}, // Maroon
			FLinearColor{0.0F, 0.5F, 0.5F}, // Turquoise
			FLinearColor::Black,
			FLinearColor{0.2F, 0.2F, 0.2F},
		};

		const int32 NeighborCellCount = MineCell.NeighborMineCount;
		const bool bIsMineCell = MineCell.IsMine();
		const bool bIsRevealed = MineCell.IsRevealed();
		const bool bHasText = !bIsMineCell && bIsRevealed;

		if (bHasText)
		{
			return TextColor[NeighborCellCount];
		}

		return FLinearColor::Transparent;
	}
}

FMinesweeperView::FMinesweeperView()
{
}

TSharedRef<SDockTab> FMinesweeperView::CreateMinesweeperView(
	const FSpawnTabArgs& SpawnTabArgs,
	FMinesweeperGameConfig GameConfig)
{
	const TSharedPtr<SWidget> InputWidget = CreateInputWidget();
	MineGridWidget = SNew(SUniformGridPanel);

	const TSharedPtr<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Top)
			  .Padding(10.0F)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				  .HAlign(HAlign_Left)
				  .VAlign(VAlign_Top)
				  .AutoHeight()
				  .Padding(10.0F)
				[
					InputWidget.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				  .HAlign(HAlign_Left)
				  .VAlign(VAlign_Top)
				  .AutoHeight()
				  .Padding(10.0F)
				[
					SAssignNew(GameStateWidget, STextBlock)
					.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 20))
				]
				+ SVerticalBox::Slot()
				  .HAlign(HAlign_Left)
				  .VAlign(VAlign_Top)
				  .AutoHeight()
				  .Padding(10.0F)
				[
					MineGridWidget.ToSharedRef()
				]
			]
		];

	RebuildMineGridWidget(GameConfig);

	return DockTab.ToSharedRef();
}

TSharedPtr<SWidget> FMinesweeperView::CreateInputWidget()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		  .Padding(0.0F, 10.0F)
		  .HAlign(HAlign_Left)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.MinDesiredWidth(50.0F)
				.Text(FText::FromString("Width: "))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12))
			]
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Center)
			[
				SAssignNew(WidthSpinBox, SSpinBox<int32>)
				.MinDesiredWidth(100.0F)
				.MinValue(FMinesweeperGameConfig::MIN_COL)
				.MaxValue(FMinesweeperGameConfig::MAX_COL)
				.MinSliderValue(FMinesweeperGameConfig::MIN_COL)
				.MaxSliderValue(FMinesweeperGameConfig::MAX_COL)
				.Value(FMinesweeperGameConfig::DEFAULT_COL)
				.Delta(1)
				.OnValueChanged_Lambda([this](int32)
				{
					ValidateMineCountInput();
				})
			]
			+ SHorizontalBox::Slot()
			  .Padding(20.0F, 0.0F, 0.0F, 0.0F)
			  .AutoWidth()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.MinDesiredWidth(50.0F)
				.Text(FText::FromString("Height: "))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12))
			]
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Center)
			[
				SAssignNew(HeightSpinBox, SSpinBox<int32>)
				.MinDesiredWidth(100.0F)
				.MinValue(FMinesweeperGameConfig::MIN_ROW)
				.MaxValue(FMinesweeperGameConfig::MAX_ROW)
				.MinSliderValue(FMinesweeperGameConfig::MIN_ROW)
				.MaxSliderValue(FMinesweeperGameConfig::MAX_ROW)
				.Value(FMinesweeperGameConfig::DEFAULT_ROW)
				.Delta(1)
				.OnValueChanged_Lambda([this](int32)
				{
					ValidateMineCountInput();
				})
			]
		]
		+ SVerticalBox::Slot()
		  .Padding(0.0F, 10.0F)
		  .HAlign(HAlign_Left)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .HAlign(HAlign_Left)
			  .VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.MinDesiredWidth(120.0F)
				.Text(FText::FromString("Number Of Mines: "))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12))
			]
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .HAlign(HAlign_Center)
			  .VAlign(VAlign_Center)
			[
				SAssignNew(MineCountSpinBox, SSpinBox<int32>)
				.MinDesiredWidth(100.0F)
				.MinValue(FMinesweeperGameConfig::MIN_MINE_COUNT)
				.MaxValue(FMinesweeperGameConfig::DEFAULT_MAX_MINE_COUNT)
				.MinSliderValue(FMinesweeperGameConfig::MIN_MINE_COUNT)
				.MaxSliderValue(FMinesweeperGameConfig::DEFAULT_MAX_MINE_COUNT)
				.Value(FMinesweeperGameConfig::DEFAULT_MINE_COUNT)
				.Delta(1)
			]
		]
		+ SVerticalBox::Slot()
		  .Padding(0.0F, 10.0F)
		  .HAlign(HAlign_Left)
		[
			SAssignNew(NewGameButton, SButton)
			.Text(FText::FromString("Start New Game"))
			.OnClicked_Lambda([this]()
			{
				BroadcastOnStartNewGame();
				return FReply::Handled();
			})
		];
}

void FMinesweeperView::RebuildGameLayout(FMinesweeperGameConfig NewConfig)
{
	RebuildMineGridWidget(NewConfig);
	UpdateGameStateWidget(EMinesweeperGameState::Running);
}

void FMinesweeperView::UpdateGameLayout(FMinesweeperGameConfig GameConfig, const FMinesweeperGameState& GameState)
{
	UpdateMineGridWidget(GameConfig, GameState);
	UpdateGameStateWidget(GameState.State);
}

void FMinesweeperView::BroadcastOnStartNewGame()
{
	const int32 GridWidth = WidthSpinBox->GetValueAttribute().Get();
	const int32 GridHeight = HeightSpinBox->GetValueAttribute().Get();
	const int32 MineCount = MineCountSpinBox->GetValueAttribute().Get();
	const TOptional<int32> Seed{};
	const FMinesweeperGameConfig NewConfig{{GridWidth, GridHeight}, MineCount, Seed};

	OnStartNewGame.ExecuteIfBound(NewConfig);
}

void FMinesweeperView::ValidateMineCountInput()
{
	const int32 GridWidth = WidthSpinBox->GetValueAttribute().Get();
	const int32 GridHeight = HeightSpinBox->GetValueAttribute().Get();
	const int32 MineCount = MineCountSpinBox->GetValueAttribute().Get();
	const int32 MaxMineCount = GridWidth * GridHeight - 1;
	const int32 NewMineCount = FMath::Min(MineCount, MaxMineCount);

	MineCountSpinBox->SetMaxValue(MaxMineCount);
	MineCountSpinBox->SetMaxSliderValue(MaxMineCount);
	MineCountSpinBox->SetValue(NewMineCount);
}

void FMinesweeperView::RebuildMineGridWidget(FMinesweeperGameConfig GameConfig)
{
	const int32 GridWidth = GameConfig.GridSize.X;
	const int32 GridHeight = GameConfig.GridSize.Y;
	const int32 CellCount = GridWidth * GridHeight;

	MineGridWidget->ClearChildren();
	MineCellWidgets.Empty();
	MineCellWidgets.Reserve(CellCount);

	for (int32 Idx = 0; Idx < CellCount; ++Idx)
	{
		const int32 X = Idx % GridWidth;
		const int32 Y = Idx / GridWidth;
		const FIntPoint Coordinate{X, Y};

		SUniformGridPanel::FSlot& Slot = MineGridWidget->AddSlot(X, Y);
		FMineCellWidget& MineCellWidget = MineCellWidgets.Emplace_GetRef();
		MineCellWidget.OnWidgetClicked.BindLambda([this, Coordinate]()
		{
			const FPlayerInput Input{Coordinate, EInputType::Visit};
			OnPlayerInput.ExecuteIfBound(Input);
		});

		Slot.AttachWidget(MineCellWidget.GetWidget());
	}
}

void FMinesweeperView::UpdateMineGridWidget(FMinesweeperGameConfig GameConfig, const FMinesweeperGameState& GameState)
{
	const TArray<FMineCell>& MineCells = GameState.Cells;
	const int32 GridWidth  = GameConfig.GridSize.X;
	const int32 GridHeight = GameConfig.GridSize.Y;
	const int32 CellCount  = GridWidth * GridHeight;

	for (int32 Idx = 0; Idx < CellCount; ++Idx)
	{
		const FMineCell& MineCell = MineCells[Idx];
		const FLinearColor CellColor = GetMineCellColor(MineCell.CellState, MineCell.CellType);
		const FLinearColor CellTextColor = GetMineCellTextColor(MineCell);
		const FText CellText = FText::AsNumber(MineCell.NeighborMineCount);

		FMineCellWidget& MineCellWidget = MineCellWidgets[Idx];
		MineCellWidget.SetCellColor(CellColor);
		MineCellWidget.SetCellText(CellText, CellTextColor);
	}
}

void FMinesweeperView::UpdateGameStateWidget(EMinesweeperGameState State)
{
	switch (State)
	{
	case EMinesweeperGameState::GameOver_Win:
		GameStateWidget->SetText(FText::FromString("Congratulations! You won!"));
		GameStateWidget->SetColorAndOpacity(FLinearColor::Green);
		break;
	case EMinesweeperGameState::GameOver_Lose:
		GameStateWidget->SetText(FText::FromString("Oops! You stepped on a mine!"));
		GameStateWidget->SetColorAndOpacity(FLinearColor::Red);
		break;
	default:
		GameStateWidget->SetColorAndOpacity(FLinearColor::Transparent);
		break;
	}
}

#undef LOCTEXT_NAMESPACE