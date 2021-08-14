#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE(FOnWidgetClicked)

/**
 * Wrapper of mine cell widget that is able to respond to mouse click event.
 * Can also set background color and text of a cell.
 */
class FMineCellWidget
{
public:
	FMineCellWidget();

	FMineCellWidget(const FMineCellWidget&) = delete;
	FMineCellWidget& operator=(const FMineCellWidget&) = delete;

	TSharedRef<SWidget> GetWidget() const;
	void SetCellText(FText Text, FLinearColor TextColor);
	void SetCellColor(FLinearColor Color);

public:
	FOnWidgetClicked OnWidgetClicked;

private:
	TSharedPtr<SWidget> ContainerWidget;
	TSharedPtr<class SButton> ButtonWidget;
	TSharedPtr<class STextBlock> TextWidget;
};