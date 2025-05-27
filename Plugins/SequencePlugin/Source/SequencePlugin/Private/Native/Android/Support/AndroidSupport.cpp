#include "AndroidSupport.h"

#if PLATFORM_ANDROID

/*
 * Converts an FString into a java string
 */
jstring ConvertToJavaString(JNIEnv* jenv, const FString& string)
{
   const jstring localString = jenv->NewStringUTF(TCHAR_TO_UTF8(*string));
   const jstring globalString{static_cast<jstring>(jenv->NewGlobalRef(localString))};
   jenv->DeleteLocalRef(localString);
   return globalString;
}

/*
 * Logs an FString into Android Logs
*/
void JavaLog(const FString& message)
{
	
}
#endif
