#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/5555977b/Scs.o \
	${OBJECTDIR}/_ext/5555977b/ScsAddress.o \
	${OBJECTDIR}/_ext/5555977b/ScsClient.o \
	${OBJECTDIR}/_ext/5555977b/ScsCommon.o \
	${OBJECTDIR}/_ext/5555977b/ScsReceiveQueue.o \
	${OBJECTDIR}/_ext/5555977b/ScsSendQueue.o \
	${OBJECTDIR}/_ext/5555977b/ScsServer.o \
	${OBJECTDIR}/_ext/5555977b/ScsSocket.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++17
CXXFLAGS=-std=c++17

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libscs.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libscs.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libscs.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libscs.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libscs.a

${OBJECTDIR}/_ext/5555977b/Scs.o: ../../../../Source/Scs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/Scs.o ../../../../Source/Scs.cpp

${OBJECTDIR}/_ext/5555977b/ScsAddress.o: ../../../../Source/ScsAddress.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/ScsAddress.o ../../../../Source/ScsAddress.cpp

${OBJECTDIR}/_ext/5555977b/ScsClient.o: ../../../../Source/ScsClient.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/ScsClient.o ../../../../Source/ScsClient.cpp

${OBJECTDIR}/_ext/5555977b/ScsCommon.o: ../../../../Source/ScsCommon.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/ScsCommon.o ../../../../Source/ScsCommon.cpp

${OBJECTDIR}/_ext/5555977b/ScsReceiveQueue.o: ../../../../Source/ScsReceiveQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/ScsReceiveQueue.o ../../../../Source/ScsReceiveQueue.cpp

${OBJECTDIR}/_ext/5555977b/ScsSendQueue.o: ../../../../Source/ScsSendQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/ScsSendQueue.o ../../../../Source/ScsSendQueue.cpp

${OBJECTDIR}/_ext/5555977b/ScsServer.o: ../../../../Source/ScsServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/ScsServer.o ../../../../Source/ScsServer.cpp

${OBJECTDIR}/_ext/5555977b/ScsSocket.o: ../../../../Source/ScsSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/ScsSocket.o ../../../../Source/ScsSocket.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libscs.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
