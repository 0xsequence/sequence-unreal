// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SequenceUnrealPlugin/Public/RequestHandler.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeRequestHandler() {}
// Cross Module References
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	SEQUENCEUNREALPLUGIN_API UClass* Z_Construct_UClass_URequestHandler();
	SEQUENCEUNREALPLUGIN_API UClass* Z_Construct_UClass_URequestHandler_NoRegister();
	UPackage* Z_Construct_UPackage__Script_SequenceUnrealPlugin();
// End Cross Module References
	void URequestHandler::StaticRegisterNativesURequestHandler()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(URequestHandler);
	UClass* Z_Construct_UClass_URequestHandler_NoRegister()
	{
		return URequestHandler::StaticClass();
	}
	struct Z_Construct_UClass_URequestHandler_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_URequestHandler_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_SequenceUnrealPlugin,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_URequestHandler_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "IncludePath", "RequestHandler.h" },
		{ "ModuleRelativePath", "Public/RequestHandler.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_URequestHandler_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<URequestHandler>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_URequestHandler_Statics::ClassParams = {
		&URequestHandler::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_URequestHandler_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_URequestHandler_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_URequestHandler()
	{
		if (!Z_Registration_Info_UClass_URequestHandler.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_URequestHandler.OuterSingleton, Z_Construct_UClass_URequestHandler_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_URequestHandler.OuterSingleton;
	}
	template<> SEQUENCEUNREALPLUGIN_API UClass* StaticClass<URequestHandler>()
	{
		return URequestHandler::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(URequestHandler);
	URequestHandler::~URequestHandler() {}
	struct Z_CompiledInDeferFile_FID_jandohring_Documents_Unreal_Projects_SequenceUnreal_Plugins_SequenceUnrealPlugin_Source_SequenceUnrealPlugin_Public_RequestHandler_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_jandohring_Documents_Unreal_Projects_SequenceUnreal_Plugins_SequenceUnrealPlugin_Source_SequenceUnrealPlugin_Public_RequestHandler_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_URequestHandler, URequestHandler::StaticClass, TEXT("URequestHandler"), &Z_Registration_Info_UClass_URequestHandler, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(URequestHandler), 1140530111U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_jandohring_Documents_Unreal_Projects_SequenceUnreal_Plugins_SequenceUnrealPlugin_Source_SequenceUnrealPlugin_Public_RequestHandler_h_565512060(TEXT("/Script/SequenceUnrealPlugin"),
		Z_CompiledInDeferFile_FID_jandohring_Documents_Unreal_Projects_SequenceUnreal_Plugins_SequenceUnrealPlugin_Source_SequenceUnrealPlugin_Public_RequestHandler_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_jandohring_Documents_Unreal_Projects_SequenceUnreal_Plugins_SequenceUnrealPlugin_Source_SequenceUnrealPlugin_Public_RequestHandler_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
