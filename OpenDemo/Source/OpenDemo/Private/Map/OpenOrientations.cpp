// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/OpenOrientations.h"

bool UOpenOrientations::IsHorizontal(EDirection Direction)
{
	// 如果既不是向上也不是向下, 那就是水平方向
	return Direction != EDirection::ED_Down && Direction != EDirection::ED_Up;
}
