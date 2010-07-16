LOCAL_PATH=
PROJECT_BASE=/Users/shiretu/work/crtmpserver-trunk
PROJECT_SOURCES=$(PROJECT_BASE)/sources

PLATFORM_DEFINES = -DANDROID \
	-DLITTLE_ENDIAN_BYTE_ALIGNED \
	-DNET_EPOLL

FEATURES_DEFINES = -DHAS_PROTOCOL_HTTP \
	-DHAS_PROTOCOL_RTMP \
	-DHAS_PROTOCOL_LIVEFLV \
	-DHAS_PROTOCOL_RTP \
	-DHAS_PROTOCOL_TS \
	-DHAS_PROTOCOL_VAR \
	-DHAS_LUA

GENERIC_DEFINES=$(PLATFORM_DEFINES) $(FEATURES_DEFINES)

# --- lua ---
include $(CLEAR_VARS)
LOCAL_MODULE=lua
LOCAL_C_INCLUDES=$(PROJECT_BASE)/3rdparty/lua-dev
LOCAL_SRC_FILES=$(shell find $(PROJECT_BASE)/3rdparty/lua-dev -type f -name *.c)
LOCAL_CFLAGS=$(GENERIC_DEFINES)
include $(BUILD_SHARED_LIBRARY)

# --- common ---
include $(CLEAR_VARS)
LOCAL_MODULE=common
LOCAL_C_INCLUDES=$(PROJECT_BASE)/3rdparty/lua-dev $(PROJECT_SOURCES)/$(LOCAL_MODULE)/include
LOCAL_SRC_FILES=$(shell find $(PROJECT_SOURCES)/$(LOCAL_MODULE)/src -type f -name *.cpp)
LOCAL_CFLAGS=$(GENERIC_DEFINES)
LOCAL_LDLIBS := -lcrypto -lssl
LOCAL_SHARED_LIBRARIES=lua
include $(BUILD_SHARED_LIBRARY)

