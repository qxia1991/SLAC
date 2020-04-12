#ifndef _EXOInputJNI_HH
#define	_EXOInputJNI_HH

#include <string>
class EXOInputModule;

class EXOInputJNI {
public:
    EXOInputJNI();
    virtual ~EXOInputJNI();
    bool openFile(std::string file);
    bool search(JNIEnv *env, int event, jobject jevent);

private:
    EXOInputModule* input;
};

#endif	/* _EXOInputJNI_HH */

