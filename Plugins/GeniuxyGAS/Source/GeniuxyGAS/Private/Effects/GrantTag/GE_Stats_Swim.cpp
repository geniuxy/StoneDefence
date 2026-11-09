// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/GrantTag/GE_Stats_Swim.h"

#include "Tags/StatsTags.h"

UGE_Stats_Swim::UGE_Stats_Swim()
{
	// 本 GE 只挂「游泳中」这一个状态标签。
	// 进出水的判定、浮力、MOVE_Swimming 全部不在 GE 里做 —— GE 只表达「我现在带着这个状态」。
	AddGrantedTag(StatsTags::Ge_Stats_Swim);
}
