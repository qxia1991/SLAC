#include "org_exo_analysis_jni_EXOInput.h"
#include "EXOInputJNI.hh"
#include "EXOUtilities/EXOWaveform.hh"
#include "EXOUtilities/EXOEventData.hh"
#include "EXOAnalysisManager/EXOInputModule.hh"
#include "TSystem.h"
#include <dlfcn.h>

#include <iostream>
using namespace std;

#if SIZEOF_VOID_P == 8
  typedef int64_t ptr_cast;
#elif SIZEOF_VOID_P == 4
  typedef uint32_t ptr_cast;
#else
# error "Size of pointer not 4 or 8"
#endif

#define DEFINE_INPUT_PTR_FROM_SOURCE(ptr, src) \
  EXOInputJNI* ptr;                            \
  ptr = (EXOInputJNI*) (ptr_cast(src));       

JNIEXPORT jlong JNICALL Java_org_exo_analysis_jni_EXOInput_initialize
(JNIEnv *env, jobject object) {

    gSystem->ResetSignal(kSigSegmentationViolation,kTRUE);
   // Ensure that the correct symbols are exported.  This is because ROOT
   // plugins aren't properly setup to deal with dlopen.  The following calls
   // enable later loading of plugins to work.
#define DLOPEN_CHK(aname) \
  (dlopen(gSystem->DynamicPathName(#aname, true), RTLD_NOW | RTLD_GLOBAL ) == NULL)
   if (DLOPEN_CHK(libCore) || 
       DLOPEN_CHK(libCint) || 
       DLOPEN_CHK(libRIO)  || 
       DLOPEN_CHK(libNet)) {
      // throw an exception?
      return 0; 
    }
    EXOInputJNI* jni = new EXOInputJNI();
    return (jlong) jni;
}

/*
 * Class:     org_exo_analysis_jni_EXOInput
 * Method:    openFile
 * Signature: (JLjava/lang/String;)V
 */
JNIEXPORT jboolean JNICALL Java_org_exo_analysis_jni_EXOInput_openFile
  (JNIEnv *env, jobject object, jlong jniPtr, jstring fileName) {
   DEFINE_INPUT_PTR_FROM_SOURCE(jni, jniPtr); 
   const char* file = env->GetStringUTFChars(fileName, NULL);
   bool rc = jni->openFile(file);
   env->ReleaseStringUTFChars(fileName, file);
   return (jboolean) rc;
}

/*
 * Class:     org_exo_analysis_jni_EXOInput
 * Method:    search
 * Signature: (JI)V
 */
JNIEXPORT jboolean JNICALL Java_org_exo_analysis_jni_EXOInput_search
  (JNIEnv *env, jobject object, jlong jniPtr, jint eventNumber, jobject event) {
   DEFINE_INPUT_PTR_FROM_SOURCE(jni, jniPtr); 
   bool rc = jni->search(env, eventNumber,event);
   return (jboolean) rc;
}

/*
 * Class:     org_exo_analysis_jni_EXOInput
 * Method:    delete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_exo_analysis_jni_EXOInput_delete
  (JNIEnv *env, jobject object, jlong jniPtr) {
   DEFINE_INPUT_PTR_FROM_SOURCE(jni, jniPtr); 
   delete jni; 
}

EXOInputJNI::EXOInputJNI() : 
  input(NULL)
{
}

EXOInputJNI::~EXOInputJNI() {
   if (input) EXOAnalysisModuleFactory::GetInstance().DestroyAnalysisModule( input ); 
}

bool EXOInputJNI::openFile(std::string fileName) {
   if (input) EXOAnalysisModuleFactory::GetInstance().DestroyAnalysisModule( input ); 
   input = EXOAnalysisModuleFactory::GetInstance().FindInputModuleAndOpenFile( fileName );
   if (!input) return false;
   return input;
}
bool EXOInputJNI::search(JNIEnv *env, int eventNumber, jobject event) {
   EXOEventData *ed = input->SearchEvent(eventNumber);
   if (ed) { 
      jclass eventClass = env->GetObjectClass(event);
      jmethodID setTrigCount = env->GetMethodID(eventClass, "setTrigCount", "(I)V");
      env->CallVoidMethod(event, setTrigCount, ed->fEventNumber);

      EXOEventHeader* eh = &ed->fEventHeader;
      jmethodID setEventHeader = env->GetMethodID(eventClass, "setEventHeader", "(IIIIIIIIIIII)V");
      env->CallVoidMethod(event, setEventHeader, eh->fSampleCount, eh->fSumTriggerRequest, eh->fSumTriggerThreshold, eh->fSumTriggerValue, eh->fTriggerDrift, 
                                                 eh->fTriggerMicroSeconds, eh->fTriggerOffset, eh->fTriggerSeconds, eh->fTriggerSource,
                                                 eh->fIndividualTriggerRequest, eh->fIndividualTriggerThreshold, eh->fMaxValueChannel);

      jmethodID setWaveformData = env->GetMethodID(eventClass, "setWaveformData", "(I[I)V");
      const EXOWaveformData* data = ed->GetWaveformData();

      for (size_t i=0; i<data->GetNumWaveforms(); i++) {
          const EXOWaveform* wave = data->GetWaveform(i);
          const size_t len = wave->GetLength();
          const Int_t* array = wave->GetData();
          jintArray jArray = env->NewIntArray(len);
          env->SetIntArrayRegion(jArray,0,len,array);
          env->CallVoidMethod(event,setWaveformData,wave->fChannel,jArray);
      }
   }
   return ed;
}
