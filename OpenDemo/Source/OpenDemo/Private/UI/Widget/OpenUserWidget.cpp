#include "UI/Widget/OpenUserWidget.h"

void UOpenUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
