//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2016 Agog Labs Inc. All rights reserved.
//
// Component to associate a SkookumScript Mind with a UE4 actor
// 
// Author: Markus Breyer
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "SkookumScriptRuntimePrivatePCH.h"
#include "../Classes/SkookumScriptMindComponent.h"

//=======================================================================================
// Class Data
//=======================================================================================

//=======================================================================================
// Method Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------

USkookumScriptMindComponent::USkookumScriptMindComponent(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  {
  PrimaryComponentTick.bCanEverTick = false;
  bTickInEditor = false;
  bAutoActivate = true;
  bWantsInitializeComponent = true;
  }

//---------------------------------------------------------------------------------------

void USkookumScriptMindComponent::create_sk_instance()
  {
  SK_ASSERTX(!m_mind_instance_p, "Tried to create mind instance when instance already present!");

  // Find the actor I belong to
  AActor * actor_p = GetOwner();
  SK_ASSERTX(actor_p, "SkookumScriptMindComponent must be attached to an actor.");

  // Determine SkookumScript class of my Mind
  SkClass * class_p = nullptr;
  FString class_name = ScriptMindClassName;
  AString class_name_ascii(*class_name, class_name.Len());
  class_p = SkBrain::get_class(class_name_ascii.as_cstr());
  SK_ASSERTX(class_p, a_cstr_format("Cannot find Script Class Name '%s' specified in SkookumScriptMindComponent of '%S'. Misspelled?", class_name_ascii.as_cstr(), *actor_p->GetName()));
  if (!class_p)
    {
    class_p = SkBrain::ms_master_class_p; // Recover from bad user input
    }

  // Based on the desired class, create SkInstance or SkDataInstance
  SK_ASSERTX(class_p->is_mind_class(), a_str_format("Trying to create a SkookumScriptMindComponent of class '%s' which is not a Mind.", class_p->get_name_cstr_dbg()));
  m_mind_instance_p = class_p->new_instance();
  }

//---------------------------------------------------------------------------------------

void USkookumScriptMindComponent::delete_sk_instance()
  {
  SK_ASSERTX(m_mind_instance_p, "No Sk instance to delete!");
  m_mind_instance_p->clear_coroutines();
  m_mind_instance_p->dereference();
  m_mind_instance_p = nullptr;
  }

//---------------------------------------------------------------------------------------

void USkookumScriptMindComponent::OnRegister()
  {
  Super::OnRegister();
  }

//---------------------------------------------------------------------------------------

void USkookumScriptMindComponent::InitializeComponent()
  {
  Super::InitializeComponent();

  // Create SkookumScript instance, but only if we are located inside the game world
  if (GetOwner()->GetWorld() == SkUEClassBindingHelper::get_world())
    {
    SK_ASSERTX(SkookumScript::is_flag_set(SkookumScript::Flag_evaluate), "SkookumScript must be in initialized state when InitializeComponent() is invoked.");
    create_sk_instance();
    m_mind_instance_p->get_class()->resolve_raw_data();
    m_mind_instance_p->call_default_constructor();
    }
  }

//---------------------------------------------------------------------------------------

void USkookumScriptMindComponent::UninitializeComponent()
  {
  // Delete SkookumScript instance if present
  if (m_mind_instance_p)
    {
    SK_ASSERTX(SkookumScript::is_flag_set(SkookumScript::Flag_evaluate), "SkookumScript must be in initialized state when UninitializeComponent() is invoked.");
    delete_sk_instance();
    }

  Super::UninitializeComponent();
  }

//---------------------------------------------------------------------------------------

void USkookumScriptMindComponent::OnUnregister()
  {
  Super::OnUnregister();
  }

