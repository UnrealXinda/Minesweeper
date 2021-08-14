#include "Minesweeper.h"
#include "MinesweeperStyle.h"
#include "MinesweeperCommands.h"
#include "MVC/MinesweeperView.h"
#include "MVC/MinesweeperModel.h"
#include "MVC/MinesweeperController.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "ToolMenus.h"

static const FName MinesweeperTabName("Minesweeper");

#define LOCTEXT_NAMESPACE "FMinesweeperModule"

void FMinesweeperModule::StartupModule()
{
	FMinesweeperStyle::Initialize();
	FMinesweeperStyle::ReloadTextures();

	FMinesweeperCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMinesweeperCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMinesweeperModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MinesweeperTabName,
	                                                  FOnSpawnTab::CreateRaw(
		                                              this, &FMinesweeperModule::OnSpawnPluginTab))
	                        .SetDisplayName(LOCTEXT("FMinesweeperTabTitle", "Minesweeper"))
	                        .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMinesweeperModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FMinesweeperStyle::Shutdown();
	FMinesweeperCommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MinesweeperTabName);
}

TSharedRef<SDockTab> FMinesweeperModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	PluginModel = MakeUnique<FMinesweeperModel>();
	PluginController = MakeUnique<FMinesweeperController>(PluginModel.Get());
	PluginView = MakeUnique<FMinesweeperView>();

	PluginView->OnPlayerInput.BindRaw(PluginController.Get(), &FMinesweeperController::HandleOnPlayerInput);
	PluginView->OnStartNewGame.BindRaw(PluginController.Get(), &FMinesweeperController::HandleOnStartNewGame);

	PluginModel->OnGameConfigUpdated.BindRaw(PluginView.Get(), &FMinesweeperView::RebuildGameLayout);
	PluginModel->OnMineGridChanged.BindRaw(PluginView.Get(), &FMinesweeperView::UpdateGameLayout);

	return PluginView->CreateMinesweeperView(SpawnTabArgs, PluginModel->GameConfig);
}

void FMinesweeperModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(MinesweeperTabName);
}

void FMinesweeperModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMinesweeperCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(FMinesweeperCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMinesweeperModule, Minesweeper)