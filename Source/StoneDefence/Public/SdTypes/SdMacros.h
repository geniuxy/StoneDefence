#pragma once

#ifdef PLATFORM_WINDOWS
#define SEND_DATA(InProtocol, ...) \
if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>()) \
{\
	SIMPLE_CLIENT_SEND(ClientGameInstance->GetClient(), InProtocol, ##__VA_ARGS__);\
}
#else
#define SEND_DATA(InProtocol, args...) \
if (USdGameInstance* ClientGameInstance = GetGameInstance<USdGameInstance>()) \
{\
SIMPLE_CLIENT_SEND(ClientGameInstance->GetClient(), InProtocol, ##args);\
}
#endif
