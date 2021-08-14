#include "MineCellWidget.h"

FMineCellWidget::FMineCellWidget()
{
	ContainerWidget = SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(ButtonWidget, SButton)
			.OnClicked_Lambda([=]()
			{
				OnWidgetClicked.ExecuteIfBound();
				return FReply::Handled();
			})
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SAssignNew(TextWidget, STextBlock)
				]
			]
		];

	SetCellText(FText::AsNumber(0), FLinearColor::Transparent);
	SetCellColor(FLinearColor::White);
	GetWidget()->Invalidate(EInvalidateWidgetReason::Paint);
}

TSharedRef<SWidget> FMineCellWidget::GetWidget() const
{
	return ContainerWidget.ToSharedRef();
}

void FMineCellWidget::SetCellText(FText Text, FLinearColor TextColor)
{
	TextWidget->SetText(Text);
	TextWidget->SetColorAndOpacity(TextColor);
}

void FMineCellWidget::SetCellColor(FLinearColor Color)
{
	ButtonWidget->SetBorderBackgroundColor(Color);
}