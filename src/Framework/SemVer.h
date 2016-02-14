#ifndef SemVer_h
#define SemVer_h

// Put call to DECLARE_SEMVER in the declaration file xx.h in the private
// section of the device driver class declaration.  For example:
//
// class TrialDriver {
// public:
//   int yada();
// private:
//   DECLARE_SEMVER
// };

#define DECLARE_SEMVER \
static const PROGMEM uint8_t releaseVersion[];\
static const PROGMEM char scopeName[];\
static const PROGMEM char preReleaseLabel[];\
static const PROGMEM char buildLabel[];

// Put call to DEFINE_SEMVER in the definition file xx.cpp, but outside
// of any constructor or method.  For example:
//
// DEFINE_SEMVER(TrialDriver,1,0,3)
//
// Note that the scopeName argument is NOT a string literal and so has no
// quotes around it in the call to the macro.  It is stringified where
// necessary in the body of the macro.
//
// A pre-release label and a build meta-data label can be added using
// alternate forms of the DEFINE macro.  For example:
//
// DEFINE_SEMVER_PRE(TrialDriver,0,5,0,beta)
// DEFINE_SEMVER_PRE_BLD(TrialDriver,0,5,0,beta,__DATE__)

#define DEFINE_SEMVER(theScopeName, major, minor, patch) \
DEFINE_SEMVER_ALL(theScopeName, major, minor, patch,,)

#define DEFINE_SEMVER_PRE(theScopeName, major, minor, patch,prLabel) \
DEFINE_SEMVER_ALL(theScopeName, major, minor, patch,prLabel,)

#define DEFINE_SEMVER_BLD(theScopeName, major, minor, patch,bLabel) \
DEFINE_SEMVER_ALL(theScopeName, major, minor, patch,,bLabel)

#define DEFINE_SEMVER_PRE_BLD(theScopeName, major, minor, patch, prLabel, bLabel) \
DEFINE_SEMVER_ALL(theScopeName, major, minor, patch, prLabel, bLabel)

#define DEFINE_SEMVER_ALL(theScopeName, major, minor, patch,prLabel,bLabel) \
const PROGMEM uint8_t theScopeName::releaseVersion[] = {3, major, minor, patch};\
const PROGMEM char theScopeName::scopeName[] = {#theScopeName}; \
const PROGMEM char theScopeName::preReleaseLabel[] = {#prLabel}; \
const PROGMEM char theScopeName::buildLabel[] = {#bLabel};

#endif
